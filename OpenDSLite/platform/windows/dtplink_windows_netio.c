/*
 * Copyright 2012 Seguesoft <seguesoft2010@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "dtplink_windows_netio.h"
#include "dtplink_datastruct.h"
#include "winsock2.h"

static Int32 getErrno() { return WSAGetLastError(); }

socket_t DTPLink_GetLocalEndpoint(UInt8 type)
{
	static socket_t localEndpoint4InternetComm = INVALID_SOCKET;
	static socket_t localEndpoint4IntranetComm = INVALID_SOCKET;
	if( type == INTERNET_EP ){ //Endpoint for Internet communication 
		if(localEndpoint4InternetComm == INVALID_SOCKET){
			localEndpoint4InternetComm = DTPLink_OpenPort(DEFAULT_DS_REMOTE_PORT, 0);
		}
		return localEndpoint4InternetComm;
	}else{					  //Endpoint for Intranet communication
		if(localEndpoint4IntranetComm == INVALID_SOCKET){
			localEndpoint4IntranetComm = DTPLink_OpenPort(DEFAULT_DS_LOCAL_PORT, 0);
		}
		return localEndpoint4IntranetComm;
	}
}

socket_t DTPLink_OpenPort( UInt16 port, UInt32 interfaceIp )
{   
	struct sockaddr_in addr;
	socket_t myFd = INVALID_SOCKET;
	myFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ( myFd == INVALID_SOCKET ){
		PRINTF("Could not create a UDP socket: %d\n", getErrno());
		return INVALID_SOCKET;
	}
    
   memset((Int8*)&(addr),0, sizeof((addr)));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(port);
    
   if ( (interfaceIp != 0) && (interfaceIp != 0x100007f) ){
      addr.sin_addr.s_addr = htonl(interfaceIp);
	  PRINTF("Binding to interface 0x%x\n", htonl(interfaceIp));
   }
	
   if ( bind( myFd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ){
      Int32 e = getErrno();
        
      switch (e){
         case 0:
				PRINTF("Could not bind socket\n");
				return INVALID_SOCKET;
         case WSAEADDRINUSE:
			    PRINTF("Port %d for receiving UDP is in use\n", port);
				return INVALID_SOCKET;
         case WSAEADDRNOTAVAIL:
			    PRINTF("Cannot assign requested address\n");
				return INVALID_SOCKET;
         default:
				PRINTF("Could not bind UDP receive port Error=%d %s\n", e, strerror(e));
				return INVALID_SOCKET;
      }
   }

   PRINTF("Opened port %d with fd %d\n", port, myFd);
   //set this fd to noblock mode
   DTPLink_SetNonblockingIO(myFd);
   return myFd;
}

Int32 DTPLink_SetNonblockingIO(socket_t fd)
{
    Int32 flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	#if DS_PLATFORM_TYPE == DS_PLATFORM_WINDOWS
		//-------------------------
		// Set the socket I/O mode: In this case FIONBIO
		// enables or disables the blocking mode for the 
		// socket based on the numerical value of iMode.
		// If iMode = 0, blocking is enabled; 
		// If iMode != 0, non-blocking mode is enabled.
		flags = 1;
		return ioctlsocket(fd, FIONBIO, (ULong32*)&flags);
	#else
		/* Otherwise, use the old way of doing it */
		flags = 1;
		return ioctl(fd, FIOBIO, &flags);
	#endif
#endif
} 

ULong32 DTPLink_DNSResolve(Int8 *DNSName)
{
	struct sockaddr_in serverAddr;
   	struct hostent *serverHost = NULL;

	serverHost = gethostbyname(DNSName); 
	serverAddr.sin_addr.s_addr = *(ULong32*) serverHost->h_addr_list[0];
	return serverAddr.sin_addr.s_addr; //In network order
} 

BOOL DTPLink_Recv(socket_t ep, Int8* buf, Int16* len, UInt32* srcIp, UInt16* srcPort)
{
   struct sockaddr_in from;
   Int32 fromLen = sizeof(from);
   Int32 originalSize = *len;
   if( originalSize <= 0 ) return FALSE;
	
   *len = recvfrom(ep, buf, originalSize, 0, (struct sockaddr *)&from, &fromLen);
	
   if ( *len == SOCKET_ERROR ){
      Int32 err = getErrno();
      switch (err){
         case WSAENOTSOCK:
			PRINTF("Error fd not a socket\n");
            break;
         case WSAECONNRESET:
			PRINTF("Error connection reset - host not reachable\n");
            break;
		 case WSAEWOULDBLOCK:
			 //Do nothing when got this error on noblocking socket, no an error
			//PRINTF("No available data on this socket now\n");
			 break;
         default:
			PRINTF("Socket Error=%d\n", err);
      }
      return FALSE;
   }
	
   if ( *len == 0 ){
	  PRINTF("socket closed? zero len\n");
      return FALSE;
   }
    
   *srcPort = ntohs(from.sin_port);
   *srcIp = ntohl(from.sin_addr.s_addr);
	
   if ( (*len)+1 >= originalSize ){
      PRINTF("Received a message that was too large\n");
      return FALSE;
   }

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
    
	if ( s == SOCKET_ERROR ){
		Int32 e = getErrno();
		switch (e){
			case WSAECONNREFUSED:
			case WSAEHOSTDOWN:
			case WSAEHOSTUNREACH:
			{
				// quietly ignore this 
			}
			break;
			case WSAEAFNOSUPPORT:
			{
				PRINTF("err EAFNOSUPPORT in send\n");
			}
			break;
			default:
			{
				PRINTF("err %d %s in send\n", e, strerror(e));
			}
		}
		return FALSE;
	}
    
   if ( s == 0 ){
	  PRINTF("no data sent in send\n");
      return FALSE;
   }
    
   if ( s != len ){
	  PRINTF("only %d out of %d bytes sent\n", s, len);
      return FALSE;
   }
    
   return TRUE;
}

BOOL DTPLink_InitNetwork()
{
#ifdef WIN32 
   WORD wVersionRequested = MAKEWORD( 2, 2 );
   WSADATA wsaData;
	
   Int32 err = WSAStartup( wVersionRequested, &wsaData );
   if ( err != 0 ){
      // could not find a usable WinSock DLL
	  PRINTF("Could not load winsock\n");
	  return FALSE;
   }
    
   /* Confirm that the WinSock DLL supports 2.2.*/
   /* Note that if the DLL supports versions greater    */
   /* than 2.2 in addition to 2.2, it will still return */
   /* 2.2 in wVersion since that is the version we      */
   /* requested.                                        */
    
   if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ){
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.                                  */
      WSACleanup();
	  PRINTF("Bad winsock verion\n");
	  return FALSE;
   }    

   return TRUE;
#endif
}

void DTPLink_Wait(UInt32 ms)
{
	Sleep(ms); //millisecond
}

static DWORD WINAPI KeepAliveThread(LPVOID lpParam) 
{
	extern void DTPLink_DSHeartBeat();
	DTPLink_DSHeartBeat();
	return 0;
}

void DTPLink_StartKeepAlive() 
{
	//Create keep-alive thread
	DWORD tid;
	CreateThread(NULL, 0, KeepAliveThread, NULL, 0, &tid);
}