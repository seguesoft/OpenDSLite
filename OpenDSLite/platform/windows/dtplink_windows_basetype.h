/*
 *  dtplink base data type on windows and i386 platform
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

#ifndef DTPLINK_WINDOSW_BASETYPE_H
#define DTPLINK_WINDOSW_BASETYPE_H

#include <winsock2.h>
#include <stdio.h>

// define basic types for dtplink DS library
typedef char  Int8;
typedef short Int16;
typedef int   Int32;
typedef long  Long32;
typedef __int64 Int64;
typedef struct { char octet[16]; }  Int128;
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
typedef unsigned long  ULong32;
typedef unsigned __int64 UInt64;
typedef struct { unsigned char octet[16]; }  UInt128;
typedef UInt32 socket_t;

#define PRINTF printf
#endif /* DTPLINK_WINDOSW_BASETYPE_H */