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

/*
file	: dtplink_datastruct.h
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink main data structure header file
*/
#ifndef DTPLINK_DATASTRUCT_H
#define DTPLINK_DATASTRUCT_H
#pragma pack(1)
#include "dtplink_config.h"
#include "dtplink_buffer.h"

//Struct define
typedef struct {
    UInt16 port;
    UInt32 addr;
} IPv4Address;

typedef enum {
	DS_STATUS_STARTUP = 0,		//startup
	DS_STATUS_WAIT_CS_PHASE1,	//wait response from server1 port1(3478)
	DS_STATUS_WAIT_CS_PHASE2,	//wait response from server1 port2(3479)
	DS_STATUS_WAIT_CS_PHASE3,	//wait response from server2 port1(3479)
	DS_STATUS_WAIT_DNS,			//wait dns register response
	DS_STATUS_ONLINE			//wait for client to connect
}DS_STATUS ;

typedef struct {
	IPv4Address mDNS;
	IPv4Address mCS1;
	IPv4Address mCS2;
	IPv4Address mIP;
	CRequestBuffer	REQUEST_INFO;
	CResponseBuffer RESPONSE_INFO;
	Int8 mID[MAX_DS_ID];
	UInt8 mRunningMode;
	DS_STATUS mStatus;
	socket_t mRemoteFd;
	socket_t mLocalFd;
	UInt32 mHearbeatTime;
	UInt8 mRecvRetryCount;
	UInt8 mSendRetryCount;
} CServerContext;

#endif /*DTPLINK_DATASTRUCTS_H*/