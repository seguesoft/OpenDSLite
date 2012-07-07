/*
 *  dtplink device server open API implemention
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

#include "dtplink_io.h"
#include "dtplink_util.h"
#include "dtplink_ds.h"
#include "dtplink_buffer.h"

// Global DS context, This data struct contain all the info which are used by application layer.
CServerContext globalContext; // FIXME: better to create it on heap memory
CServerContext *pGlobalContext = &globalContext;

#define DS_ADD_SEND_RETRY_COUNT() \
	pGlobalContext->mSendRetryCount++;

#define DS_RESET_SEND_RETRY_COUNT() \
	pGlobalContext->mSendRetryCount = 0;

#define DS_SEND_RETRY_COUNT() \
	(pGlobalContext->mSendRetryCount)

#define DS_ADD_RECV_RETRY_COUNT() \
	pGlobalContext->mRecvRetryCount++;

#define DS_RESET_RECV_RETRY_COUNT() \
	pGlobalContext->mRecvRetryCount = 0;

#define DS_RECV_RETRY_COUNT() \
	(pGlobalContext->mRecvRetryCount)

#define ResetAllRetryCounts() \
	pGlobalContext->mRecvRetryCount = 0; \
	pGlobalContext->mSendRetryCount = 0;

#define DS_SET_STATUS(status) \
	pGlobalContext->mStatus = status; 

#define DS_GET_STATUS() \
	pGlobalContext->mStatus;

#define SetCenterServerAddress(cs) \
	pGlobalContext->mCS1.addr = cs.addr; \
	pGlobalContext->mCS1.port = cs.port;

#define SetCenterServerAddress2(ip, pt) \
	pGlobalContext->mCS1.addr = ip; \
	pGlobalContext->mCS1.port = pt; 

#define SetAuxServerAddress(cs) \
	pGlobalContext->mCS2.addr = cs.addr; \
	pGlobalContext->mCS2.port = cs.port;

#define SetAuxServerAddress2(ip, pt) \
	pGlobalContext->mCS2.addr = ip; \
	pGlobalContext->mCS2.port = pt;

#define SetDNSAddress(dns) \
	pGlobalContext->mDNS.addr = dns.addr; \
	pGlobalContext->mDNS.port = dns.port;

#define SetDNSAddress2(ip, pt) \
	pGlobalContext->mDNS.addr = ip; \
	pGlobalContext->mDNS.port = pt;

#define SetRunningMode(mode) \
	pGlobalContext->mRunningMode = mode;

#define SetMappedIPAddress2(ip, pt) \
	pGlobalContext->mIP.addr = ip;  \
	pGlobalContext->mIP.port = pt;  \
	if(pGlobalContext->mIP.addr == RELAY_IP_PORT){ \
		SetRunningMode(REMOTE_RELAY_MODE); \
	}else if(pGlobalContext->mIP.addr == NOPUBLIC_IP_PORT){ \
		SetRunningMode(LOCAL_MODE); \
	}else{ \
		SetRunningMode(REMOTE_DIRECT_MODE); \
	}

#define SetMappedIPAddress(ip) \
	SetMappedIPAddress2(ip.addr, ip.port)

static BOOL InitDSContext(Int8* id, Int8* dns)
{
	ULong32 dnsAddr = 0;
	// sanity check
	if( id == NULL || *id == 0 ){
		PRINTF("Fatal error! Device server name can't be blank!");
		return FALSE; 
	}
	if(strlen(id) >= MAX_DS_ID){
		PRINTF("Fatal error! Device server name is longer than %d\n!", MAX_DS_ID);
		return FALSE; 
	}
	strcpy(pGlobalContext->mID, id);

	if( dns == NULL || *dns == 0 ){
		dnsAddr = DTPLink_DNSResolve(DEFAULT_DNS); 
	}else{
		dnsAddr = DTPLink_DNSResolve(dns);
	}	
	if( dnsAddr == 0 ){
		PRINTF("Fatal error! Can't find DNS!");
		return FALSE; 
	}
	SetDNSAddress2(ntohl(dnsAddr), STUN_PORT);
	SetCenterServerAddress2(ntohl(dnsAddr), STUN_PORT);// FIXME: currently CS1 = DNS, is submitted to change in the future

	pGlobalContext->mRemoteFd  = DTPLink_GetLocalEndpoint(INTERNET_EP); // Create Internet communication endpoint
	pGlobalContext->mLocalFd = DTPLink_GetLocalEndpoint(INTRANET_EP); // Create Intranet communication endpoint
	if( pGlobalContext->mRemoteFd == INVALID_SOCKET || pGlobalContext->mLocalFd  == INVALID_SOCKET ){
		  PRINTF("Fatal error! Can't create local end point!");
		  return FALSE; 
	}
	DTPLink_ResetBuffers(&(pGlobalContext->REQUEST_INFO), &(pGlobalContext->RESPONSE_INFO));
	pGlobalContext->mHearbeatTime = DS_HEARTBEAT_INTERVAL; // FIXME: should wrap this assignment into function call
	SetRunningMode(REMOTE_DIRECT_MODE);
	DS_SET_STATUS(DS_STATUS_STARTUP);
	ResetAllRetryCounts();

	return TRUE;
}

static void PingCenterServer()
{  
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = SERVER_PUNCHING;
	static UInt8 first_time = 1;
	IPv4Address rtpTo;
	rtpTo.addr = pGlobalContext->mCS1.addr;
	rtpTo.port = pGlobalContext->mCS1.port; // ping on port 1

	if(first_time == 1){
		// Blindly Ping center server before do the real communication, not comply with RFC?
		DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port);
		DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port+1);
		first_time = 0;
	}

	msg = BIND_REQ_TO_SERVER1_PORT1;
	DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port); // FIXME: check the function return value
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s via%d\n", BIND_REQ_TO_SERVER1_PORT1, sizeof(msg), IP2STR(rtpTo), &msg, myFd);
	
	DS_SET_STATUS(DS_STATUS_WAIT_CS_PHASE1);
}

static void PingCenterServerAgain()
{  
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = BIND_REQ_TO_SERVER1_PORT2;
	IPv4Address rtpTo;
	rtpTo.addr = pGlobalContext->mCS1.addr;
	rtpTo.port = pGlobalContext->mCS1.port+1; // ping on port 2

	DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port); // FIXME: check the function return value
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s via%d\n", BIND_REQ_TO_SERVER1_PORT2, sizeof(msg), IP2STR(rtpTo), &msg, myFd);
	
	DS_SET_STATUS(DS_STATUS_WAIT_CS_PHASE2);
}

static void PingAuxServer()
{  
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = SERVER_PUNCHING;
	static UInt8 first_time = 1;
	IPv4Address rtpTo;
	rtpTo.addr = pGlobalContext->mCS2.addr;
	rtpTo.port = pGlobalContext->mCS2.port; // default port1: 3479

	if(first_time == 1){
		// Blindly Ping server2 with two ports before do the real communication RFC?
		DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port);
		DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port-1);
		first_time = 0;
	}

	msg = BIND_REQ_TO_SERVER2_PORT1;
	DTPLink_Send(myFd, &msg, sizeof(msg), rtpTo.addr, rtpTo.port); // FIXME: check the function return value
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s via%d\n", BIND_REQ_TO_SERVER2_PORT1, sizeof(msg), IP2STR(rtpTo), &msg, myFd);
	
	DS_SET_STATUS(DS_STATUS_WAIT_CS_PHASE3);
}

static void RegisterDS()
{  
	IPv4Address rtpSrv1;
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg[MAX_RTP_MSG_SIZE] = {0};
	Int16 msgLen = sizeof(msg);
	rtpSrv1.addr = pGlobalContext->mDNS.addr;
	rtpSrv1.port = pGlobalContext->mDNS.port; 

	PRINTF("Begin register device server\n");
	if(pGlobalContext->mRunningMode == REMOTE_RELAY_MODE){
		sprintf(msg, "%c %s|R", REGIST_DS_IP_TO_DNS_SERVER, pGlobalContext->mID);
		msgLen = strlen(msg);
		PRINTF("I'm running in RELAY  Mode\n");
	}else{
		sprintf(msg, "%c %s|D", REGIST_DS_IP_TO_DNS_SERVER, pGlobalContext->mID);
		msgLen = strlen(msg);
		PRINTF("I'm running in DIRECT Mode\n");
	}
	DTPLink_Send(myFd, msg, msgLen, rtpSrv1.addr, rtpSrv1.port);
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s via%d\n", REGIST_DS_IP_TO_DNS_SERVER, msgLen, IP2STR(rtpSrv1), msg, myFd);

	DS_SET_STATUS(DS_STATUS_WAIT_DNS);
}

static void OnReceiveCenterServerResponse(Int8* msg, IPv4Address* ip, IPv4Address* cs2)
{  
	// ¡õClientPublicAddress|CloudServer2Address
	Int8* p = msg;
	ULong32 addr = 0;
	UInt16  port = 0;

	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	memcpy(&port, p, 2);
	port = ntohs(port);
	p = p + 2;
	ip->addr = addr;
	ip->port = port;

	p++; //skip '|'
	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	memcpy(&port, p, 2);
	port = ntohs(port);
	cs2->addr = addr;
	cs2->port = port;
	// FIXME: should wrap code above into function call

	PingCenterServerAgain();
}

static void OnReceiveCenterServerResponse2(Int8* msg, IPv4Address* ip)
{  
	// ¡õClientPublicAddress
	ULong32 addr = 0;
	UInt16  port = 0;
	Int8* p = msg;

	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	memcpy(&port, p, 2);
	port = ntohs(port);
	ip->addr = addr;
	ip->port = port;
	// FIXME: should wrap code above into function call

	PingAuxServer();
}

static void OnReceiveAuxServerResponse(Int8* msg, IPv4Address* ip)
{  
	// ¡õClientPublicAddress
	ULong32 addr = 0;
	UInt16  port = 0;
	Int8* p = msg;

	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	memcpy(&port, p, 2);
	port = ntohs(port);
	ip->addr = addr;
	ip->port = port;
	// FIXME: should wrap code above into function call

	RegisterDS(); // register server name
}

static void OnReceiveDSRegisterResponse(Int8* msg, UInt16 msgLen)
{
	// Have nothing to do so far.
}

static void ProcessDNSQueryRequest(IPv4Address* client)
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = SERVER_PUNCHING;

	DTPLink_Send(myFd, &msg, sizeof(msg), client->addr, client->port);
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s\n", SERVER_PUNCHING, sizeof(msg), IP2STR(*client), &msg);	
}

static void OnReceiveDNSQueryRequest(Int8* msg, UInt16 msgLen)
{
	IPv4Address devClient;
	// ¡õClientPublicAddress
	ULong32 addr = 0;
	UInt16  port = 0;
	Int8* p = msg;

	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	memcpy(&port, p, 2); 
	port = ntohs(port);
	devClient.addr = addr;
	devClient.port = port;
	p = p + 2;
	memcpy(&port, p, 2);
	port = ntohs(port);
	// FIXME: should wrap code above into function call

	ProcessDNSQueryRequest(&devClient);	
}

static void ProcessLocalDNSQueryRequest(IPv4Address* client)
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = QUERY_DS_IP_TO_LOCAL_DS;

	DTPLink_Send(myFd, &msg, sizeof(msg), client->addr, client->port);
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s\n", SERVER_PUNCHING, sizeof(msg), IP2STR(*client), &msg);	
}

static void OnReceiveLocalDNSQueryRequest(Int8* msg, UInt16 msgLen, IPv4Address* client)
{
	if(strcmp(pGlobalContext->mID, msg) == 0){
		PRINTF("Get Local DNS query from " IP_FMT " looking for server: %s\n", IP2STR(*client), msg);
		ProcessLocalDNSQueryRequest(client);
	}
}

// FIXME: functions can be lifted into caller
static void ProcessRemoteDataQueryRequest(Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 omsg[MAX_RTP_MSG_SIZE] = {0};
	Int16 omsgLen = sizeof(omsg);
	UInt16 id = 0;
	Int16 orgLen = 0;
	Int16 enLen = 0;

	PRINTF("ProcessRemoteDataQueryRequest\n");
	memcpy(pGlobalContext->REQUEST_INFO.buffer, imsg, imsgLen); // skip request command and ¡õ
	pGlobalContext->REQUEST_INFO.length = imsgLen;
	omsg[0] = REMOTE_DS_DATA_RESP_TO_CLIENT;
	omsg[1] = ' ';
	
	READ_WORD(&id); // Read id from request buffer
	WRITE_WORD(id); // Write back id to response buffer
	device_server_main(id); // This function is provided by user application

	// copy response buffer to net io layer output buffer
	memcpy(omsg+2, pGlobalContext->RESPONSE_INFO.buffer, pGlobalContext->RESPONSE_INFO.length);
	omsgLen = pGlobalContext->RESPONSE_INFO.length+2;
	DTPLink_Send(myFd, omsg, omsgLen, client->addr, client->port);
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s\n", REMOTE_DS_DATA_RESP_TO_CLIENT, omsgLen, IP2STR(*client), omsg);
}

static void OnReceiveRemoteDataQueryRequest(Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	ProcessRemoteDataQueryRequest(imsg, imsgLen, client);	
}

// FIXME: functions can be lifted into caller
static void ProcessLocalDataQueryRequest(Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	ProcessRemoteDataQueryRequest(imsg, imsgLen, client); // FIXME: currently, no diffenent between local and remote requests
}

static void OnReceiveLocalDataQueryRequest(Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	ProcessLocalDataQueryRequest(imsg, imsgLen, client);	
}

// FIXME: functions can be lifted into caller
static void ProcessRelayDataQueryRequest(UInt8 resp_id, Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	// 0x61¡õClientPublicAddress|RequestContext
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 omsg[MAX_RTP_MSG_SIZE] = {0};
	Int16 omsgLen = sizeof(omsg);
	IPv4Address clientIP;
	Int8* pRelayMessage = NULL;
	Int32 relayFd = -1;
	Int32 len = 0;
	UInt16 id = 0;	
	ULong32 addr = 0;
	UInt16  port = 0;

	if(resp_id == QUERY_DATA_RELAY_TO_REMOTE_DS){
		len = ParseRelayInfoOnDS(imsg, imsgLen, &clientIP, &pRelayMessage);
	}else{
		len = ParseRelayHttpInfoOnDS(imsg, imsgLen, &clientIP, &relayFd, &pRelayMessage);
	}

	memcpy(pGlobalContext->REQUEST_INFO.buffer, pRelayMessage, len); //skip request command and ¡õ
	pGlobalContext->REQUEST_INFO.length = len;

	READ_WORD(&id); // Read id from request buffer
	WRITE_WORD(id); // Write back id to response buffer
	addr = htonl(clientIP.addr);
	port = htons(clientIP.port);

	if(resp_id == QUERY_DATA_RELAY_TO_REMOTE_DS){ // Request reached DS directly
		//sprintf(omsg, "%c " IP_FMT "|", DEV_SERVER_RESP_RELAY_TO_CLIENT, IP2STR(clientIP));
		omsg[0] = REMOTE_DS_DATA_RESP_RELAY_TO_CLIENT;
		omsg[1] = SPACE;
		memcpy(omsg + 2, &addr, 4);
		memcpy(omsg + 6, &port, 2);
		omsg[8] = SEPARATER;
		len = 9;
	}else{ // Request reached DS via HTTP Tunnel
		//sprintf(omsg, "%c " IP_FMT "|%d|", DEV_SERVER_RESP_HTTP_TO_CLIENT, IP2STR(clientIP), relayPort);
		omsg[0] = REMOTE_DS_DATA_RESP_HTTP_TO_CLIENT;
		omsg[1] = ' ';
		memcpy(omsg + 2, &addr, 4);
		memcpy(omsg + 6, &port, 2);
		omsg[8] = SEPARATER;
		relayFd = htonl(relayFd);
		memcpy(omsg + 9, &relayFd, 4);
		omsg[13] = SEPARATER;
		len = 14;
		PRINTF("###########Process request via http tunnel############\n");
	}
	device_server_main(id); // This function is provided by user application
	omsgLen = len;

	// Copy response buffer to net io layer output buffer
	memcpy(omsg+omsgLen, pGlobalContext->RESPONSE_INFO.buffer, pGlobalContext->RESPONSE_INFO.length);
	omsgLen += pGlobalContext->RESPONSE_INFO.length;

	DTPLink_Send(myFd, omsg, omsgLen, client->addr, client->port);
	PRINTF("Send MSG(id=0x%x, len=%d) to " IP_FMT " content: %s\n", omsg[0], omsgLen, IP2STR(*client), omsg);
}

static void OnReceiveRelayDataQueryRequest(UInt8 resp_id, Int8* imsg, UInt16 imsgLen, IPv4Address* client)
{
	ProcessRelayDataQueryRequest(resp_id, imsg, imsgLen, client);	
}

static void OnReceiveKeepAlive(IPv4Address* client)
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg = SERVER_PUNCHING;

	DTPLink_Send(myFd, &msg, sizeof(msg), client->addr, client->port);
	PRINTF("Send Keep alive Response to " IP_FMT "\n", IP2STR(*client));	
}

static void ProcessClientRequests(UInt8 resp_id, Int8* msg, Int16 msgLen, IPv4Address* client)
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	// Ensure request and response buffer is empty before processing request
	DTPLink_ResetBuffers(&pGlobalContext->REQUEST_INFO, &pGlobalContext->RESPONSE_INFO);

	PRINTF("Get  MSG(id=0x%x, len=%d) from: " IP_FMT " content: %s @%d\n", resp_id, msgLen, IP2STR(*client), msg, myFd);
	if(resp_id == QUERY_DS_IP_TO_DNS_SERVER){ 
		OnReceiveDNSQueryRequest(msg, msgLen);
	}else if(resp_id == QUERY_DS_IP_TO_LOCAL_DS){ 
		OnReceiveLocalDNSQueryRequest(msg, msgLen, client);
	}else if(resp_id == QUERY_DATA_TO_REMOTE_DS){ 
		OnReceiveRemoteDataQueryRequest(msg, msgLen, client);
	}else if(resp_id == QUERY_DATA_TO_LOCAL_DS){
		OnReceiveLocalDataQueryRequest(msg, msgLen, client);
	}else if(resp_id == QUERY_DATA_RELAY_TO_REMOTE_DS || resp_id == QUERY_DATA_HTTP_TO_REMOTE_DS){
		OnReceiveRelayDataQueryRequest(resp_id, msg, msgLen, client);
	}else if(resp_id == REGIST_DS_IP_TO_DNS_SERVER){
		OnReceiveDSRegisterResponse(msg, msgLen);
	}else if(resp_id == SERVER_PUNCHING){
		//OnReceiveKeepAlive(client); ignore client keep-alive message
	}
}

// Public functions
BOOL DTPLink_DSInit(Int8* id, Int8* dns)
{
	BOOL ret = FALSE;

	// Init network layer
	ret = DTPLink_InitNetwork();
	if(!ret){ return FALSE; }
	// Init Device Server context
	ret = InitDSContext(id, dns);
	if (!ret){ return FALSE; }

	// Wait for a random time period to avoid traffic flood after DS reboot
	// int randwaittime =stunTimePeriod(); // FIXME: on some platform there is no rand generate function
	// PRINTF("Waiting for %d ms before device server initial!\n", randwaittime);
	DTPLink_Wait(2000); // FIXME: wait time should be given as a random number to avoid traffic flood when many ds reboot at the same time
	
	// now start the ds main funcation....
	PRINTF("Begin probing my public IP\n");
	PingCenterServer();

	return TRUE;
}

void DTPLink_DSLoop()
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	UInt8 resp_id = 0;
	UInt8 current_state = 0;
	Int8 msg[MAX_RTP_MSG_SIZE] = {0};
	Int16 msgLen = sizeof(msg);
	BOOL RecvFlag = 0;
	IPv4Address rtpFrom;
	IPv4Address CS1P2;
	IPv4Address MIP1;
	IPv4Address MIP2;
	BOOL ok = FALSE;
	CS1P2.addr = pGlobalContext->mCS1.addr;
	CS1P2.port = pGlobalContext->mCS1.port + 1;

	// Some low resource platform don't support poll or selet io, 
	// so we wait here. maybe not a good desingn, but easy for porting
	// In general, we suggest to run this DSLoop in a seperate thread or task.
	while(TRUE){
		DTPLink_Wait(RECV_RETRY_INTERVAL); 
		memset(msg, 0, MAX_RTP_MSG_SIZE);
		msgLen = MAX_RTP_MSG_SIZE;
		current_state = DS_GET_STATUS();
		ok = DTPLink_Recv(myFd, msg, &msgLen, &rtpFrom.addr, &rtpFrom.port);
		if(ok){
			resp_id = msg[0];
			switch(current_state){
			case DS_STATUS_WAIT_CS_PHASE1:
				if(resp_id == BIND_REQ_TO_SERVER1_PORT1 && EQUAL_ADDR(rtpFrom, pGlobalContext->mCS1) ){
					ResetAllRetryCounts();
					OnReceiveCenterServerResponse(msg+2, &MIP1, &pGlobalContext->mCS2);
				}
				break;
			case DS_STATUS_WAIT_CS_PHASE2:
				if(resp_id == BIND_REQ_TO_SERVER1_PORT2 && EQUAL_ADDR(rtpFrom, CS1P2)){
					ResetAllRetryCounts();
					OnReceiveCenterServerResponse2(msg+2, &MIP2);
					if(!EQUAL_ADDR(MIP1, MIP2)){
						SetMappedIPAddress2(RELAY_IP_PORT, RELAY_IP_PORT);
						PRINTF("Probe my public IP failed! Relay communication via centerial server\n");
					}
				}
				break;
			case DS_STATUS_WAIT_CS_PHASE3:
				if(resp_id == BIND_REQ_TO_SERVER2_PORT1 && EQUAL_ADDR(rtpFrom, pGlobalContext->mCS2)){
					ResetAllRetryCounts();
					// For ds non-http realy test Begin
					// SetMappedIPAddress2(RELAY_IP_PORT, RELAY_IP_PORT);
					// PRINTF("Probe my public IP failed! Relay communication via centerial server\n");
					// For ds non-http realy test End
					OnReceiveAuxServerResponse(msg+2, &MIP2);//FIXME: move RegisterDS out of this func
					if(!EQUAL_ADDR(MIP1, MIP2)){
						SetMappedIPAddress2(RELAY_IP_PORT, RELAY_IP_PORT);
						PRINTF("Probe my public IP failed! Relay communication via centerial server\n");
					}else{
						SetMappedIPAddress(MIP1);
						PRINTF("Probe my public IP finished! My mapped ip is " IP_FMT "\n", IP2STR(pGlobalContext->mIP));
					}
					DTPLink_StartKeepAlive(); //keep connection alive between DS and CS
				}
				break;
			case DS_STATUS_WAIT_DNS:
				if(resp_id == REGIST_DS_IP_TO_DNS_SERVER){
					OnReceiveDSRegisterResponse(msg + 2, msgLen - 2);
					PRINTF("Device server ###[%s]### regist successfully!\n", pGlobalContext->mID);
					PRINTF("Begin waiting for client requests\n");
					DS_SET_STATUS(DS_STATUS_ONLINE);
					ResetAllRetryCounts();	
				}
				break;
			case DS_STATUS_ONLINE:
				ProcessClientRequests(resp_id, msg + 2, msgLen - 2, &rtpFrom);
				break;
			default:
				break;
			}
		}else{
			DS_ADD_RECV_RETRY_COUNT();
			switch(current_state){
			case DS_STATUS_WAIT_CS_PHASE1:
				if(DS_SEND_RETRY_COUNT() >= SEND_RETRY_COUNT_MAX){
					SetMappedIPAddress2(NOPUBLIC_IP_PORT, NOPUBLIC_IP_PORT);
					PRINTF("Probe my public IP failed! I'm running in local network mode\n");
					DS_SET_STATUS(DS_STATUS_ONLINE);
					ResetAllRetryCounts();
					break;
				}
				if(DS_RECV_RETRY_COUNT() >= RECV_RETRY_COUNT_MAX){
					DS_RESET_RECV_RETRY_COUNT();
					PingCenterServer();
					DS_ADD_SEND_RETRY_COUNT();
				}
				break;
			case DS_STATUS_WAIT_CS_PHASE2:
				if(DS_SEND_RETRY_COUNT() >= SEND_RETRY_COUNT_MAX){
					SetMappedIPAddress2(RELAY_IP_PORT, RELAY_IP_PORT);
					PRINTF("Probe my public IP failed! Relay communication via centerial server\n");
					RegisterDS();
					ResetAllRetryCounts();
					break;
				}
				if(DS_RECV_RETRY_COUNT() >= RECV_RETRY_COUNT_MAX){
					DS_RESET_RECV_RETRY_COUNT();
					PingCenterServerAgain();
					DS_ADD_SEND_RETRY_COUNT();
				}
				break;
			case DS_STATUS_WAIT_CS_PHASE3:
				if(DS_SEND_RETRY_COUNT() >= SEND_RETRY_COUNT_MAX){
					SetMappedIPAddress2(RELAY_IP_PORT, RELAY_IP_PORT);
					PRINTF("Probe my public IP failed! Relay communication via centerial server\n");
					RegisterDS();
					ResetAllRetryCounts();
					break;
				}
				if(DS_RECV_RETRY_COUNT() >= RECV_RETRY_COUNT_MAX){
					DS_RESET_RECV_RETRY_COUNT();
					PingAuxServer();
					DS_ADD_SEND_RETRY_COUNT();
				}
				break;
			case DS_STATUS_WAIT_DNS:
				if(DS_SEND_RETRY_COUNT() >= SEND_RETRY_COUNT_MAX){
					SetMappedIPAddress2(NOPUBLIC_IP_PORT, NOPUBLIC_IP_PORT);
					PRINTF("Can't connect to DNS! Instead, I'm running in local network mode\n");
					DS_SET_STATUS(DS_STATUS_ONLINE);
					ResetAllRetryCounts();
					break;
				}
				if(DS_RECV_RETRY_COUNT() >= RECV_RETRY_COUNT_MAX){
					DS_RESET_RECV_RETRY_COUNT();
					RegisterDS();
					DS_ADD_SEND_RETRY_COUNT();
				}
				break;
			case DS_STATUS_ONLINE: 
				// have nothing to do here so far
				break;
			default:
				break;
			}
		}
	}
}

void DTPLink_DSHeartBeat() 
{
	socket_t myFd = pGlobalContext->mRemoteFd;
	Int8 msg[MAX_RTP_MSG_SIZE] = {0};
	UInt32 msgLen = sizeof(msg);

	if( pGlobalContext->mRunningMode == REMOTE_RELAY_MODE){
		sprintf(msg, "%c %s|R", REGIST_DS_IP_TO_DNS_SERVER, &pGlobalContext->mID);
		msgLen = strlen(msg);
	}else{
		sprintf(msg, "%c %s|D", REGIST_DS_IP_TO_DNS_SERVER, &pGlobalContext->mID);
		msgLen = strlen(msg);
	}

	while(TRUE){
		DTPLink_Wait(pGlobalContext->mHearbeatTime);
		DTPLink_Send(myFd, msg, msgLen, pGlobalContext->mDNS.addr, pGlobalContext->mDNS.port);
		PRINTF("Send KEEP_ALIVE Signal to " IP_FMT "\n", IP2STR(pGlobalContext->mDNS));
	}
}