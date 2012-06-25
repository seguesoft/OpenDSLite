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
file	: dtplink_util.c
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink communiction buffer operate function implemention
*/
#include "dtplink_config.h"
#include "dtplink_util.h"

//FIXME: Are these functions portable?
//atoi()/ntohX()/htonX()inet_addr()/memcpy()/

//reference
extern CServerContext *pGlobalContext;
Int32 ParseRelayInfoOnDS(Int8 *p, UInt16 len, IPv4Address * ip, Int8 **pRelayMessage)
{
	//ClientPublicAddress|RequestContext   //have skipped 0x61¡õ before calling this function
	Int8 *pbak = p;
	UInt32 iplen = 0;
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

	iplen = p - pbak; //include the char '|'
	*pRelayMessage = p;
	return len-iplen;
}

Int32 ParseRelayHttpInfoOnDS(Int8 *p, UInt32 len, IPv4Address * ip, UInt32 *pRelayPort, Int8 **pRelayMessage)
{
	//ClientPublicAddress|RequestContext   //have skipped 0x61¡õ before calling this function
	Int8 *pbak = p;
	UInt32 iplen = 0;
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
	*pRelayPort = addr;
	p++; //skip '|'

	iplen = p - pbak; //include the char '|'
	*pRelayMessage = p;
	return len-iplen;
}

Int32 WRITE_WORD(const UInt16 value)
{
	Int32 ret = -1;
	UInt16 ivalue = htons(value);
	if( pGlobalContext->RESPONSE_INFO.length + sizeof(UInt16) <= MAX_RTP_MSG_SIZE ){
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, &ivalue,sizeof(Int16));
		pGlobalContext->RESPONSE_INFO.tail += sizeof(Int16);
		pGlobalContext->RESPONSE_INFO.length = pGlobalContext->RESPONSE_INFO.tail;
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 WRITE_DWORD(const ULong32 value)
{
	Int32 ret = -1;
	ULong32 ivalue = htonl(value);
	if( pGlobalContext->RESPONSE_INFO.length + sizeof(ULong32) <= MAX_RTP_MSG_SIZE ){
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, &ivalue,sizeof(ULong32));
		pGlobalContext->RESPONSE_INFO.tail += sizeof(ULong32);
		pGlobalContext->RESPONSE_INFO.length = pGlobalContext->RESPONSE_INFO.tail;
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 WRITE_FLOAT(const float value)
{
	Int32 ret = -1;
	UInt8* pValue = (UInt8*)(&value);
	if( pGlobalContext->RESPONSE_INFO.length + sizeof(float) <= MAX_RTP_MSG_SIZE ){
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, pValue, sizeof(float));
		pGlobalContext->RESPONSE_INFO.tail += sizeof(float);
		pGlobalContext->RESPONSE_INFO.length = pGlobalContext->RESPONSE_INFO.tail;
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 WRITE_BYTE(const UInt8 value)
{
	Int32 ret = -1;
	if( pGlobalContext->RESPONSE_INFO.length + sizeof(UInt8) <= MAX_RTP_MSG_SIZE ){
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, &value,sizeof(Int8));
		pGlobalContext->RESPONSE_INFO.tail += sizeof(Int8);
		pGlobalContext->RESPONSE_INFO.length = pGlobalContext->RESPONSE_INFO.tail;
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 WRITE_STRING(const Int8* value)
{
	Int32 ret = -1;
	if( pGlobalContext->RESPONSE_INFO.length + strlen(value) + 1 <= MAX_RTP_MSG_SIZE ){ //Include string teminiater '\0'
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, value, strlen(value));
		pGlobalContext->RESPONSE_INFO.tail += strlen(value);
		pGlobalContext->RESPONSE_INFO.tail++; //Reserved one string end(\0) in the buffer.
		pGlobalContext->RESPONSE_INFO.length = pGlobalContext->RESPONSE_INFO.tail;
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 WRITE_RAW(const void * value, Int16 len)
{
	Int32 ret = -1;
	Int16 datalen = len;
	datalen = htons(datalen);
	if( pGlobalContext->RESPONSE_INFO.length + len * sizeof(UInt8) <= MAX_RTP_MSG_SIZE ){
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, &datalen, sizeof(Int16));//Write raw data length
		pGlobalContext->RESPONSE_INFO.tail += sizeof(Int16);
		memcpy(pGlobalContext->RESPONSE_INFO.buffer + pGlobalContext->RESPONSE_INFO.tail, value, sizeof(Int8)*len);
		pGlobalContext->RESPONSE_INFO.tail += (sizeof(Int8)*len);
		ret = pGlobalContext->RESPONSE_INFO.length;
	}
	return ret;
}

Int32 READ_WORD(UInt16 *value)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(Int16)<=pGlobalContext->REQUEST_INFO.length ){
		memcpy(value, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(Int16));
		*value = ntohs(*value);
		pGlobalContext->REQUEST_INFO.head += sizeof(Int16);
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}

Int32 READ_DWORD(ULong32 *value)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(ULong32)<=pGlobalContext->REQUEST_INFO.length ){
		memcpy(value, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(ULong32));
		*value = ntohl(*value);
		pGlobalContext->REQUEST_INFO.head += sizeof(ULong32);
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}

Int32 READ_FLOAT(float *value)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(float)<=pGlobalContext->REQUEST_INFO.length ){
		memcpy(value, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(float));
		pGlobalContext->REQUEST_INFO.head += sizeof(float);
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}

Int32 READ_BYTE(UInt8 *value)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(Int8)<=pGlobalContext->REQUEST_INFO.length ){
		memcpy(value, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(Int8));
		pGlobalContext->REQUEST_INFO.head += sizeof(Int8);
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}

Int32 READ_STRING(Int8* value)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(Int8)<=pGlobalContext->REQUEST_INFO.length ){
		strncpy(value, (Int8*)pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, strlen((Int8*)pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head) );
		pGlobalContext->REQUEST_INFO.head += strlen(value);
		pGlobalContext->REQUEST_INFO.head += 1; //skip the string endian '\0'
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}

Int32 READ_RAW(void * value, Int16* len)
{
	Int32 ret = -1;
	if( pGlobalContext->REQUEST_INFO.head + sizeof(Int8)<=pGlobalContext->REQUEST_INFO.length ){
		memcpy(len, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(Int16));
		*len = ntohs(*len);
		pGlobalContext->REQUEST_INFO.head += sizeof(Int16);//skip raw data length field
		memcpy(value, pGlobalContext->REQUEST_INFO.buffer + pGlobalContext->REQUEST_INFO.head, sizeof(Int8)*(*len));
		pGlobalContext->REQUEST_INFO.head += (sizeof(Int8)*(*len));
		ret = pGlobalContext->REQUEST_INFO.head;
	}
	return ret;
}