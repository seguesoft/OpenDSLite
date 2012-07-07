/*
 *  dtplink config header file(Put all system level config param here)
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

#ifndef DTPLINK_CONFIG_H
#define DTPLINK_CONFIG_H

// binded language list
// JAVA, Python, Objective-C

// Supported platform list
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
// Define other specific DS_PLATFORM_TYPE here below but above UNKNOWN.
#define DS_PLATFORM_UNKNOWN (0xFF)

// Define the current plaform type
#ifdef WIN32
#define DS_PLATFORM_TYPE DS_PLATFORM_WINDOWS 
#else
#ifndef DS_PLATFORM_TYPE
#define DS_PLATFORM_TYPE DS_PLATFORM_LINUX
#endif
#endif

#ifdef _USRDLL 
//#define DS_BIND_LANGUAGE_JAVA
//#define DS_BIND_LANGUAGE_PYTHON
//#define DS_BIND_LANGUAGE_OBJECTIVEC
#endif
#if DS_PLATFORM_TYPE == DS_PLATFORM_LINUX  || DS_PLATFORM_TYPE == DS_PLATFORM_ANDROID
//#define DS_BIND_LANGUAGE_JAVA
//#define DS_BIND_LANGUAGE_PYTHON
//#define DS_BIND_LANGUAGE_OBJECTIVEC
#endif

// Include specific platform header
#if DS_PLATFORM_TYPE == DS_PLATFORM_WINDOWS
	#include "platform/windows/dtplink_windows_basetype.h"
#elif (DS_PLATFORM_TYPE == DS_PLATFORM_LINUX) || (DS_PLATFORM_TYPE == DS_PLATFORM_ANDROID)
	#include "platform/linux/dtplink_linux_basetype.h"
#endif

// Invaild socket result
#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (-1)
#endif

// boolean related macro
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

// data define section
#define MAX_RTP_MSG_SIZE            (256)
#define MAX_DS_ID                   (256)
#define DEFAULT_DNS                 ("dns.seguesoft.net")
#define DS_HEARTBEAT_INTERVAL       (30*1000) //ms
#define RECV_RETRY_COUNT_MAX        (8)
#define RECV_RETRY_INTERVAL         (500) //ms
#define SEND_RETRY_COUNT_MAX        (8)
#define INTRANET_EP                 (0)
#define INTERNET_EP                 (1)
#define DEFAULT_DS_LOCAL_PORT       (20011) 
#define DEFAULT_DS_REMOTE_PORT      (20012) 
#define DEFAULT_CLIENT_LOCAL_PORT   (20021)
#define DEFAULT_CLIENT_REMOTE_PORT  (20022)

#endif /* DTPLINK_CONFIG_H */