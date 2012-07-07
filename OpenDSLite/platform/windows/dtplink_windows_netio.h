/*
 *  dtplink netio implemention on windows and i386 platform header file
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

#ifndef DTPLINK_WINDOWS_NETIO_H
#define DTPLINK_WINDOWS_NETIO_H

#include <string.h>
#include "dtplink_config.h"

// Open a UDP socket to receive on the given port - if port is 0, pick a a
// port, if interfaceIp!=0 then use ONLY the interface specified instead of all of them  
socket_t DTPLink_GetLocalEndpoint(UInt8 type);
socket_t DTPLink_OpenPort( UInt16 port, UInt32 interfaceIp);
// recive a dtplink message 
BOOL DTPLink_Recv(socket_t ep, Int8* msg, Int16* len, UInt32* srcIp, UInt16* srcPort);
BOOL DTPLink_RecvTCP(socket_t ep, Int8* buf, Int16* len, UInt32* srcIp, UInt16* srcPort);
// send a dtplink message 
BOOL DTPLink_Send(socket_t ep, Int8* msg, Int16 len, UInt32 dstIp, UInt16 dstPort);
// set up network - does nothing in unix but needed for windows
BOOL DTPLink_InitNetwork();
// set socket IO mode
Int32 DTPLink_SetNonblockingIO(socket_t fd);
// DNS resolve
ULong32 DTPLink_DNSResolve(Int8* dns);
// TIME wait
void DTPLink_Wait(UInt32 ms);
// keep alive between DS and CS
void DTPLink_StartKeepAlive();

#endif /* DTPLINK_WINDOWS_NETIO_H */
