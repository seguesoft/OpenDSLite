/*
 *  dtplink device server open API header file
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

#ifndef DTPLINK_DS_H
#define DTPLINK_DS_H

#include "dtplink_config.h"
#include "dtplink_datastruct.h"
#include "dtplink_buffer.h"
#include "dtplink_util.h"

#define STUN_PORT 3478 // default hole punch port

// command define section
// Command for CS connecting
#define BIND_REQ_TO_SERVER1_PORT1           (0x31) // DS/C->CS
#define BIND_REQ_TO_SERVER1_PORT2           (0x32) // DS/C->CS
#define BIND_REQ_TO_SERVER2_PORT1           (0x33) // DS/C->CS
#define BIND_REQ_TO_SERVER2_PORT2           (0x34) // DS/C->CS
// Command for DS registering
#define REGIST_DS_IP_TO_DNS_SERVER          (0x41) // DS->CS
#define UNREGIST_DS_IP_TO_DNS_SERVER        (0x42) // DS->CS
// Command for DS address query at Client side
#define QUERY_DS_IP_TO_DNS_SERVER           (0x43) // C->CS
#define QUERY_DS_IP_TO_LOCAL_DS             (0x44) // C->LDS
// Command for query data to remote DS
#define QUERY_DATA_TO_REMOTE_DS             (0x51) // C->RDS
#define REMOTE_DS_DATA_RESP_TO_CLIENT       (0x52) // DS->C
// Command for query data to local DS
#define QUERY_DATA_TO_LOCAL_DS              (0x53) // C->LDS
#define LOCAL_DS_DATA_RESP_TO_CLIENT        (0x54) // LDS->C
// Command for query data to remote DS via UDP relay on CS
#define QUERY_DATA_RELAY_TO_REMOTE_DS       (0x61) // C->CS->DS
#define REMOTE_DS_DATA_RESP_RELAY_TO_CLIENT (0x62) // DS->CS->C 
// Command for query data to remote DS via TCP relay on CS
#define QUERY_DATA_HTTP_TO_REMOTE_DS        (0x63) // C->CS->DS
#define REMOTE_DS_DATA_RESP_HTTP_TO_CLIENT  (0x64) // DS->CS->C
// Command for keep-alive shakehand between two communication peer end
#define SERVER_PUNCHING                     (0x7F)

// data define section
#define NET_ADAPTER_MAX         (5)
#define IP2STR(ep)              ((ep).addr >> 24) & 0xff, \
                                ((ep).addr >> 16) & 0xff, \
                                ((ep).addr >> 8)  & 0xff, \
                                ((ep).addr) & 0xff, (ep).port
#define IP_FMT                  "%u.%u.%u.%u:%u"
#define NOPUBLIC_IP_PORT        (0)
#define RELAY_IP_PORT           (-1)
#define REMOTE_DIRECT_MODE      (0)
#define REMOTE_RELAY_MODE       (1)
#define LOCAL_MODE              (2)
#define EQUAL_ADDR(Svr1, Svr2)  (Svr1.addr == Svr2.addr && Svr1.port == Svr2.port)
#define SPACE                   (' ');
#define SEPARATER               ('|');
// Open APIs
#ifdef __cplusplus
extern "C" {
#endif
	BOOL DTPLink_DSInit(Int8* id, Int8* dns);
	void DTPLink_DSLoop();
	void DTPLink_DSHeartBeat();
	extern void device_server_main(Int16 id); // this funcation is provided by user
#ifdef __cplusplus
};
#endif

#endif /* DTPLINK_DS_H */