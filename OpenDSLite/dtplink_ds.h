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
file	: dtplink_ds.h
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink device server API header file
*/
#ifndef DTPLINK_DS_H
#define DTPLINK_DS_H

#include "dtplink_config.h"
#include "dtplink_datastruct.h"
#include "dtplink_buffer.h"
#include "dtplink_util.h"

#define STUN_PORT 3478

//command define section
//Command for CS connecting
#define BIND_REQ_TO_SERVER1_PORT1			(0x31) //DS/C->CS
#define BIND_REQ_TO_SERVER1_PORT2			(0x32) //DS/C->CS
#define BIND_REQ_TO_SERVER2_PORT1			(0x33) //DS/C->CS
#define BIND_REQ_TO_SERVER2_PORT2			(0x34) //DS/C->CS
//Command for DS registering
#define REGIST_DS_IP_TO_DNS_SERVER			(0x41) //DS->CS
#define UNREGIST_DS_IP_TO_DNS_SERVER		(0x42) //DS->CS
//Command for DS address query at Client side
#define QUERY_DS_IP_TO_DNS_SERVER			(0x43) //C->CS
#define QUERY_DS_IP_TO_LOCAL_DS				(0x44) //C->LDS
//Command for query data to remote DS
#define QUERY_DATA_TO_REMOTE_DS				(0x51) //C->RDS
#define REMOTE_DS_DATA_RESP_TO_CLIENT		(0x52)
//Command for query data to local DS
#define QUERY_DATA_TO_LOCAL_DS				(0x53)
#define LOCAL_DS_DATA_RESP_TO_CLIENT		(0x54)
//Command for query data to remote DS via UDP relay on CS
#define QUERY_DATA_RELAY_TO_REMOTE_DS		(0x61)
#define REMOTE_DS_DATA_RESP_RELAY_TO_CLIENT	(0x62)
//Command for query data to remote DS via TCP relay on CS
#define QUERY_DATA_HTTP_TO_REMOTE_DS		(0x63)
#define REMOTE_DS_DATA_RESP_HTTP_TO_CLIENT	(0x64)
//Command for punching
#define SERVER_PUNCHING						(0x7F)

//data define section
#define NET_ADAPTER_MAX (5)
#define IP2STR(ep)   ((ep).addr >> 24) & 0xff, ((ep).addr >> 16) & 0xff, ((ep).addr >> 8) & 0xff, ((ep).addr) & 0xff, (ep).port
#define IP_FMT   "%u.%u.%u.%u:%u"
#define NOPUBLIC_IP_PORT (0)
#define RELAY_IP_PORT (-1)
#define REMOTE_DIRECT_MODE (0)
#define REMOTE_RELAY_MODE  (1)
#define LOCAL_MODE  (2)
#define EQUAL_ADDR(Svr1, Svr2)  (Svr1.addr == Svr2.addr && Svr1.port == Svr2.port)

//Functions
#ifdef __cplusplus
extern "C" {
#endif
	BOOL DTPLink_DSInit(Int8 *ID, Int8 *DNS);
	void DTPLink_DSLoop();
	void DTPLink_DSHeartBeat();
	extern void device_server_main(Int16 id); //this funcation is provided by user
#ifdef __cplusplus
};
#endif
#endif /*DTPLINK_DS_H*/