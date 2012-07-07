/*
 *  dtplink communiction buffer operate function implemention
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

#include "dtplink_config.h"
#include "dtplink_util.h"

// FIXME: Are these functions portable between different ARCH?
// atoi()/ntohX()/htonX()inet_addr()/memcpy()/

// the only device server context in the system
extern CServerContext *pGlobalContext; //FIXME: better to wrap it into a getter function
#define response_info (pGlobalContext->RESPONSE_INFO)
#define request_info (pGlobalContext->REQUEST_INFO)

Int32 ParseRelayInfoOnDS(Int8* p, UInt16 len, IPv4Address* ip, Int8** pRelayMessage)
{
	// ClientPublicAddress|RequestContext have skipped 0x61¡õ before calling this function
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
	p++; // skip '|'

	iplen = p - pbak; // include the char '|'
	*pRelayMessage = p;

	return len-iplen;
}

Int32 ParseRelayHttpInfoOnDS(Int8* p, UInt32 len, IPv4Address* ip, Int32* pRelayPort, Int8** pRelayMessage)
{
	// ClientPublicAddress|RequestContext have skipped 0x61¡õ before calling this function
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
	p++; // skip '|'

	memcpy(&addr, p, 4);
	addr = ntohl(addr);
	p = p + 4;
	*pRelayPort = addr;
	p++; // skip '|'

	iplen = p - pbak; // include the char '|'
	*pRelayMessage = p;

	return len-iplen;
}

Int32 WRITE_WORD(const UInt16 value)
{
	Int32 ret = -1;
	UInt16 ivalue = htons(value);

	if((response_info.length + sizeof(UInt16)) <= MAX_RTP_MSG_SIZE){
		memcpy(response_info.buffer + response_info.tail, &ivalue, sizeof(Int16));
		response_info.tail += sizeof(Int16);
		response_info.length = response_info.tail;
		ret = response_info.length;
	}
	return ret;
}

Int32 WRITE_DWORD(const ULong32 value)
{
	Int32 ret = -1;
	ULong32 ivalue = htonl(value);

	if((response_info.length + sizeof(ULong32)) <= MAX_RTP_MSG_SIZE){
		memcpy(response_info.buffer + response_info.tail, &ivalue, sizeof(ULong32));
		response_info.tail += sizeof(ULong32);
		response_info.length = response_info.tail;
		ret = response_info.length;
	}
	return ret;
}

Int32 WRITE_FLOAT(const float value)
{
    Int32 ret = -1;
    UInt8* pValue = (UInt8*)(&value);

    if((response_info.length + sizeof(float)) <= MAX_RTP_MSG_SIZE){
        memcpy(response_info.buffer + response_info.tail, pValue, sizeof(float));
        response_info.tail += sizeof(float);
        response_info.length = response_info.tail;
        ret = response_info.length;
    }
    return ret;
}

Int32 WRITE_BYTE(const UInt8 value)
{
	Int32 ret = -1;

	if((response_info.length + sizeof(UInt8)) <= MAX_RTP_MSG_SIZE){
		memcpy(response_info.buffer + response_info.tail, &value, sizeof(UInt8));
		response_info.tail += sizeof(UInt8);
		response_info.length = response_info.tail;
		ret = response_info.length;
	}
	return ret;
}

Int32 WRITE_STRING(const Int8* value)
{
	Int32 ret = -1;

	if( (response_info.length + strlen(value) + 1) <= MAX_RTP_MSG_SIZE ){ // Include string teminiater '\0'
		memcpy(response_info.buffer + response_info.tail, value, strlen(value));
		response_info.tail += strlen(value);
		response_info.tail++; // reserved one string end(\0) in the buffer.
		response_info.length = response_info.tail;
		ret = response_info.length;
	}
	return ret;
}

Int32 WRITE_RAW(const void* value, Int16 len)
{
	Int32 ret = -1;
	Int16 datalen = len;

	datalen = htons(datalen);
	if((response_info.length + len * sizeof(UInt8)) <= MAX_RTP_MSG_SIZE){
		memcpy(response_info.buffer + response_info.tail, &datalen, sizeof(Int16)); // Write raw data length
		response_info.tail += sizeof(Int16);
		memcpy(response_info.buffer + response_info.tail, value, sizeof(UInt8) * len);
		response_info.tail += (sizeof(UInt8)*len);
		ret = response_info.length;
	}
	return ret;
}

Int32 READ_WORD(UInt16* value)
{
	Int32 ret = -1;

	if((request_info.head + sizeof(UInt16)) <= request_info.length){
		memcpy(value, request_info.buffer + request_info.head, sizeof(UInt16));
		*value = ntohs(*value);
		request_info.head += sizeof(UInt16);
		ret = request_info.head;
	}
	return ret;
}

Int32 READ_DWORD(ULong32* value)
{
	Int32 ret = -1;

	if( (request_info.head + sizeof(ULong32)) <= request_info.length ){
		memcpy(value, request_info.buffer + request_info.head, sizeof(ULong32));
		*value = ntohl(*value);
		request_info.head += sizeof(ULong32);
		ret = request_info.head;
	}
	return ret;
}

Int32 READ_FLOAT(float* value)
{
	Int32 ret = -1;

	if((request_info.head + sizeof(float)) <= request_info.length){
		memcpy(value, request_info.buffer + request_info.head, sizeof(float));
		request_info.head += sizeof(float);
		ret = request_info.head;
	}
	return ret;
}

Int32 READ_BYTE(UInt8 *value)
{
	Int32 ret = -1;

	if((request_info.head + sizeof(UInt8)) <= request_info.length ){
		memcpy(value, request_info.buffer + request_info.head, sizeof(UInt8));
		request_info.head += sizeof(UInt8);
		ret = request_info.head;
	}
	return ret;
}

Int32 READ_STRING(Int8* value)
{
	Int32 ret = -1;

	if((request_info.head + sizeof(Int8)) <= request_info.length){
		strncpy(value, (Int8*)request_info.buffer + request_info.head, strlen((Int8*)request_info.buffer + request_info.head) );
		request_info.head += strlen(value);
		request_info.head += 1; //skip the string endian '\0'
		ret = request_info.head;
	}
	return ret;
}

Int32 READ_RAW(void * value, Int16* len)
{
	Int32 ret = -1;

	if((request_info.head + sizeof(Int8)) <= request_info.length){
		memcpy(len, request_info.buffer + request_info.head, sizeof(Int16));
		*len = ntohs(*len);
		request_info.head += sizeof(Int16);//skip raw data length field
		memcpy(value, request_info.buffer + request_info.head, sizeof(Int8) * (*len));
		request_info.head += (sizeof(Int8) * (*len));
		ret = request_info.head;
	}
	return ret;
}