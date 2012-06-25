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
file	: dtplink_config.h
author	: seguesoft2010@gmail.com
date	: 2012/06/19
description:
        dtplink config header file(Put all system level config param here)
*/
#ifndef DTPLINK_CONFIG_H
#define DTPLINK_CONFIG_H

//binded language list
//JAVA

//Supported platform list
#define DS_PLATFORM_WINDOWS (0x01)
#define DS_PLATFORM_LINUX   (0x02)
#define DS_PLATFORM_ECOS    (0x03)
#define DS_PLATFORM_RTOS    (0x04)
#define DS_PLATFORM_UCOS    (0x05)
#define DS_PLATFORM_ARDUINO (0x06)
#define DS_PLATFORM_UITRON  (0x07)
#define DS_PLATFORM_TENGINE (0x08)
#define DS_PLATFORM_TKERNEL (0x09)
#define DS_PLATFORM_ANDROID (0x0A)
//Define other specific DS_PLATFORM_TYPE here below.
#define DS_PLATFORM_UNKNOWN (0xFF)

//Define the current plaform type
#ifdef WIN32
#define DS_PLATFORM_TYPE DS_PLATFORM_WINDOWS 
#else
#ifndef DS_PLATFORM_TYPE
#define DS_PLATFORM_TYPE DS_PLATFORM_LINUX
#endif
#endif

#ifdef _USRDLL 
//#define DS_BIND_LANGUAGE_JAVA
#define DS_BIND_LANGUAGE_PYTHON
#endif
#if DS_PLATFORM_TYPE == DS_PLATFORM_LINUX  || DS_PLATFORM_TYPE == DS_PLATFORM_ANDROID
#define DS_BIND_LANGUAGE_JAVA (0x01)
#endif

// Include specific platform header
#if DS_PLATFORM_TYPE == DS_PLATFORM_WINDOWS
	#include "platform/windows/dtplink_windows_basetype.h"
#elif DS_PLATFORM_TYPE == DS_PLATFORM_LINUX || DS_PLATFORM_TYPE == DS_PLATFORM_ANDROID
	#include "platform/linux/dtplink_linux_basetype.h"
#endif

//Invaild socket result
#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (-1)
#endif

//Boolean related macro
#ifndef BOOL
#define BOOL UInt8
#endif
#ifndef TRUE 
#define TRUE (1)
#endif
#ifndef FALSE 
#define FALSE (0)
#endif
#ifndef NULL 
#define NULL (0)
#endif

//data define section
#define MAX_RTP_MSG_SIZE (256)
#define MAX_DS_ID (256)
#define DEFAULT_DNS ("dns.seguesoft.net")
#define DS_HEARTBEAT_INTERVAL (30*1000) //ms
#define RECV_RETRY_COUNT_MAX (8)
#define RECV_RETRY_INTERVAL (500) //ms
#define SEND_RETRY_COUNT_MAX (8)
#define INTRANET_EP (0)
#define INTERNET_EP (1)
#define DEFAULT_DS_LOCAL_PORT	(20011) 
#define DEFAULT_DS_REMOTE_PORT	(20012) 
#define DEFAULT_CLIENT_LOCAL_PORT	(20021)
#define DEFAULT_CLIENT_REMOTE_PORT	(20022)

#endif /*DTPLINK_CONFIG_H*/