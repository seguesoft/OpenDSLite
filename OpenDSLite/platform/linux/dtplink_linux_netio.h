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
#ifndef DTPLINK_LINUX_NETIO_H
#define DTPLINK_LINUX_NETIO_H

#include "dtplink_config.h"

/// Open a UDP socket to receive on the given port - if port is 0, pick a a
/// port, if interfaceIp!=0 then use ONLY the interface specified instead of
/// all of them 
socket_t DTPLink_GetLocalEndpoint(UInt8 type);
socket_t DTPLink_OpenPort( UInt16 port, UInt32 interfaceIp);
/// recive a dtplink message 
BOOL DTPLink_Recv(socket_t ep, Int8* msg, Int16* len, UInt32* srcIp, UInt16* srcPort);
/// send a dtplink message 
BOOL DTPLink_Send(socket_t ep, Int8* msg, Int16 len, UInt32 dstIp, UInt16 dstPort);
/// set up network - does nothing in unix but needed for windows
BOOL DTPLink_InitNetwork();
Int32 DTPLink_SetNonblockingIO(socket_t fd);
ULong32 DTPLink_DNSResolve(Int8 *DNSName);
void DTPLink_Wait(UInt32 ms);
void DTPLink_StartKeepAlive();
#endif /*DTPLINK_LINUX_NETIO_H*/
