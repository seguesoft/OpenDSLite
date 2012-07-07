/*
 *  dtplink base type on linux platform(reused by android platform)
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

#ifndef DTPLINK_LINUX_BASETYPE_H
#define DTPLINK_LINUX_BASETYPE_H

// define basic types for dtplink DS library
typedef char  Int8;
typedef short Int16;
typedef int   Int32;
typedef long  Long32;
typedef long long Int64;
typedef struct { char octet[16]; }  Int128;
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
typedef unsigned long  ULong32;
typedef unsigned long long UInt64;
typedef struct { unsigned char octet[16]; }  UInt128;
typedef UInt32 socket_t;

typedef unsigned char BYTE;
#define PRINTF printf
#endif /* DTPLINK_LINUX_BASETYPE_H */