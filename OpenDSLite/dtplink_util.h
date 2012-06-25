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
file	: dtplink_util.h
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink communiction buffer operate function header file
*/
#ifndef DTPLINK_UTIL_H
#define DTPLINK_UTIL_H

#include "dtplink_config.h"
#include "dtplink_datastruct.h"

//Functions
Int32 ParseRelayInfoOnDS(Int8 *p, UInt16 len, IPv4Address * ip, Int8 **pRelayMessage);
Int32 ParseRelayHttpInfoOnDS(Int8 *p, UInt32 len, IPv4Address * ip, UInt32 *pRelayPort, Int8 **pRelayMessage);
Int32 WRITE_WORD(const UInt16 value);
Int32 WRITE_DWORD(const ULong32 value);
Int32 WRITE_FLOAT(const float value);
Int32 WRITE_BYTE(const UInt8 value);
Int32 WRITE_STRING(const Int8*);
Int32 WRITE_RAW(const void * value, Int16 len);
Int32 READ_WORD(UInt16 *value);
Int32 READ_DWORD(ULong32 *value);
Int32 READ_FLOAT(float *value);
Int32 READ_BYTE(UInt8 *value);
Int32 READ_STRING(Int8* value);
Int32 READ_RAW(void * value, Int16 *len);
#endif /*DTPLINK_UTIL_H*/