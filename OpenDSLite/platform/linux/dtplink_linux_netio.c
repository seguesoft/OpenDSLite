/*
 *  dtplink netio implemention on linux platform(reused by android platform)
 *  Copyright (C) 2012  seguesoft2010@gmail.com

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dtplink_linux_netio.h"
#include "dtplink_datastruct.h"
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h> // for struct sockaddr_in
#include <sys/ioctl.h>
#include <arpa/inet.h>
static Int32 getErrno() { return errno; }

socket_t DTPLink_GetLocalEndpoint(UInt8 type)
{
	static socket_t localEndpoint4InternetComm = INVALID_SOCKET;
	static socket_t localEndpoint4IntranetComm = INVALID_SOCKET;
	if(type == INTERNET_EP){ // Endpoint for Internet communication 
		if(localEndpoint4InternetComm == INVALID_SOCKET){
			localEndpoint4InternetComm = DTPLink_OpenPort(DEFAULT_DS_REMOTE_PORT, 0);
		}
		return localEndpoint4InternetComm;
	}else{					 // Endpoint for Intranet communication
		if(localEndpoint4IntranetComm == INVALID_SOCKET){
			localEndpoint4IntranetComm = DTPLink_OpenPort(DEFAULT_DS_LOCAL_PORT, 0);
		}
		return localEndpoint4IntranetComm;
	}
}

socket_t DTPLink_OpenPort(UInt16 port, UInt32 interfaceIp)
{   
	struct sockaddr_in addr;
	socket_t myFd = INVALID_SOCKET;
	myFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(myFd == INVALID_SOCKET){
		PRINTF("Could not create a UDP socket: %d\n", getErrno());
		return INVALID_SOCKET;
	}
    
	memset((Int8*) &(addr),0, sizeof((addr)));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
    
	if(interfaceIp != 0 && interfaceIp != 0x100007f){
		addr.sin_addr.s_addr = htonl(interfaceIp);
		PRINTF("Binding to interface 0x%x\n", htonl(interfaceIp));
	}

	if(bind(myFd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
		int e = getErrno();
		switch (e){
		case 0:
			PRINTF("Could not bind socket\n");
			break;
		case EADDRINUSE:
			PRINTF("Port %d for receiving UDP is in use\n", port);
			break;
		case EADDRNOTAVAIL:
			PRINTF("Cannot assign requested address\n");
			break;
		default:
			PRINTF("Could not bind UDP receive port Error=%d %s\n", e, strerror(e));
			break;
		}
		return INVALID_SOCKET;
	}

	PRINTF("Opened port %d with fd %d\n", port, myFd);
	// set this fd to noblock mode
	DTPLink_SetNonblockingIO(myFd);

	return myFd;
}

int DTPLink_SetNonblockingIO(socket_t fd)
{
    Int32 flags;

    // If they have O_NONBLOCK, use the Posix way to do it 
#if defined(O_NONBLOCK)
    // FIXME: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5.
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))){
		flags = 0;
	}
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	#if DS_PLATFORM_TYPE == DS_PLATFORM_WINDOWS
		// Set the socket I/O mode: In this case FIONBIO
		// enables or disables the blocking mode for the 
		// socket based on the numerical value of iMode.
		// If iMode = 0, blocking is enabled; 
		// If iMode != 0, non-blocking mode is enabled.
		flags = 1;
		return ioctlsocket(fd, FIONBIO, (ULong32*)&flags);
	#else
		// Otherwise, use the old way of doing it
		flags = 1;
		return ioctl(fd, FIONBIO, &flags);
	#endif
#endif
} 

ULong32 DTPLink_DNSResolve(char* dns)
{
	struct sockaddr_in serverAddr;
	struct hostent *serverHost = NULL;

	serverHost = gethostbyname(dns); 
	PRINTF("DNSName: %s, serverHost = %d\n", dns, serverHost);
	serverAddr.sin_addr.s_addr = *(ULong32*) serverHost->h_addr_list[0];
	return serverAddr.sin_addr.s_addr; // In network order
} 

BOOL DTPLink_Recv(socket_t ep, Int8* buf, Int16* len, UInt32* srcIp, UInt16* srcPort)
{
	struct sockaddr_in from;
	Int32 fromLen = sizeof(from);
	Int32 originalSize = *len;
	if(originalSize <= 0){return FALSE;}
	
	*len = recvfrom(ep, buf, originalSize, 0, (struct sockaddr *)&from, &fromLen);
	if (*len == -1){
		Int32 err = getErrno();
		switch (err){
		case ENOTSOCK:
			PRINTF("Error fd not a socket\n");
			break;
		case ECONNRESET:
			PRINTF("Error connection reset - host not reachable\n");
			break;
		case EWOULDBLOCK:
			 // keep quietly on this error
			 break;
		default:
			PRINTF("Socket Error=%d\n", err);
			break;
		}
		return FALSE;
	}

	if(*len == 0){
		PRINTF("socket closed? zero len\n");
		return FALSE;
	}

	if((*len + 1) >= originalSize){
		PRINTF("Received a message that was too large\n");
		return FALSE;
	}

	*srcPort = ntohs(from.sin_port);
	*srcIp = ntohl(from.sin_addr.s_addr);
	buf[*len]=0;

	return TRUE;
}

BOOL DTPLink_Send(socket_t ep, Int8* buf, Int16 len, UInt32 dstIp, UInt16 dstPort)
{       
	struct sockaddr_in to;
	Int32 s = 0;
	Int32 toLen = sizeof(to);
	memset(&to, 0, toLen);
        
	to.sin_family = AF_INET;
	to.sin_port = htons(dstPort);
	to.sin_addr.s_addr = htonl(dstIp);
        
	s = sendto(ep, buf, len, 0,(struct sockaddr*)&to, toLen);
	if(s == -1){
		Int32 e = getErrno();
		switch (e){
			case ECONNREFUSED:
			case EHOSTDOWN:
			case EHOSTUNREACH:
				// quietly ignore these errors 
				break;
			case EAFNOSUPPORT:
				PRINTF("err EAFNOSUPPORT in send\n");
				break;
			default:
				PRINTF("err %d %s in send\n", e, strerror(e));
				break;
		}
		return FALSE;
	}
    
	if(s == 0){
		PRINTF("no data sent in send\n");
		return FALSE;
	}

	if(s != len){
		PRINTF("only %d out of %d bytes sent\n", s, len);
		return FALSE;
	}

	return TRUE;
}

BOOL DTPLink_InitNetwork()
{
	// Do nothing on linux platform
	return TRUE;
}

void DTPLink_Wait(UInt32 ms)
{
	UInt32 us = ms*1000;
	usleep(us); // usleep uses microseconds other than milliseconds
}

static void *KeepAliveThread(void* lpParam) 
{
	extern void DTPLink_DSHeartBeat(); // This function is provided by outside package
	DTPLink_DSHeartBeat();
	return NULL;
}

void DTPLink_StartKeepAlive() 
{
	// Create keep-alive thread
	pthread_t tid;
	pthread_create(&tid, NULL, KeepAliveThread, NULL);
}