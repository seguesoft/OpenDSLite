/*
 *  dtplink main data structure header file
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

#ifndef DTPLINK_DATASTRUCT_H
#define DTPLINK_DATASTRUCT_H

#pragma pack(push)
#pragma pack(1)
#include "dtplink_config.h"
#include "dtplink_buffer.h"

// IPv4 address format
typedef struct {
    UInt16 port;
    UInt32 addr;
}IPv4Address;

// Device server status
typedef enum {
	DS_STATUS_STARTUP = 0,		// startup
	DS_STATUS_WAIT_CS_PHASE1,	// wait response from server1 port1(3478)
	DS_STATUS_WAIT_CS_PHASE2,	// wait response from server1 port2(3479)
	DS_STATUS_WAIT_CS_PHASE3,	// wait response from server2 port1(3479)
	DS_STATUS_WAIT_DNS,			// wait dns register response
	DS_STATUS_ONLINE			// wait connection from client
}DS_STATUS ;

// Device server main struct, this is the only data struct stand for DS in the system
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
#pragma pack(pop)

#endif /* DTPLINK_DATASTRUCTS_H */