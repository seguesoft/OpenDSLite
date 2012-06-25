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
file	: dtplink_buffer.h
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink communication buffer define header file
*/
#ifndef DTPLINK_BUFFER_H
#define DTPLINK_BUFFER_H

#include "dtplink_config.h"

typedef struct {
	UInt8 buffer[MAX_RTP_MSG_SIZE];
	UInt16 tail;
	UInt16 length;
} CResponseBuffer;

typedef struct 
{
	UInt8 buffer[MAX_RTP_MSG_SIZE];
	UInt16 head;
	UInt16 length;
} CRequestBuffer;

void CRequestBufferInit(CRequestBuffer *pBuf);
void CResponseBufferInit(CResponseBuffer *pBuf);
void DTPLink_ResetBuffers(CRequestBuffer *pReq, CResponseBuffer *pResp);
#endif /*DTPLINK_BUFFER_H*/
