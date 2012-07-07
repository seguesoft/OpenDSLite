/*
 *  dtplink netio implemention on linux platform header file(Reused by android platform)
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

#ifndef DTPLINK_LINUX_NETIO_H
#define DTPLINK_LINUX_NETIO_H

#include "dtplink_config.h"

// Open a UDP socket to receive on the given port - if port is 0, pick a random
// port, if interfaceIp!=0 then use ONLY the interface specified instead of all of them 
socket_t DTPLink_GetLocalEndpoint(UInt8 type);
socket_t DTPLink_OpenPort(UInt16 port, UInt32 interfaceIp);
// recive a dtplink message 
BOOL DTPLink_Recv(socket_t ep, Int8* msg, Int16* len, UInt32* srcIp, UInt16* srcPort);
// send a dtplink message 
BOOL DTPLink_Send(socket_t ep, Int8* msg, Int16 len, UInt32 dstIp, UInt16 dstPort);
// init network does nothing in *unix but needed for windows, we put it here just to keep the unified interface
BOOL DTPLink_InitNetwork();
Int32 DTPLink_SetNonblockingIO(socket_t fd);
ULong32 DTPLink_DNSResolve(Int8* dns);
void DTPLink_Wait(UInt32 ms);
void DTPLink_StartKeepAlive();

#endif /* DTPLINK_LINUX_NETIO_H */
