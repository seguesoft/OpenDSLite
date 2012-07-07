/*
 *  dtplink communiction buffer operate function header file
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

#ifndef DTPLINK_UTIL_H
#define DTPLINK_UTIL_H

#include "dtplink_config.h"
#include "dtplink_datastruct.h"

// buffer utlities
Int32 ParseRelayInfoOnDS(Int8* p, UInt16 len, IPv4Address* ip, Int8** pRelayMessage);
Int32 ParseRelayHttpInfoOnDS(Int8* p, UInt32 len, IPv4Address* ip, Int32* pRelayPort, Int8** pRelayMessage);
Int32 WRITE_WORD(const UInt16 value);
Int32 WRITE_DWORD(const ULong32 value);
Int32 WRITE_FLOAT(const float value);
Int32 WRITE_BYTE(const UInt8 value);
Int32 WRITE_STRING(const Int8* value);
Int32 WRITE_RAW(const void* value, Int16 len);
Int32 READ_WORD(UInt16* value);
Int32 READ_DWORD(ULong32* value);
Int32 READ_FLOAT(float* value);
Int32 READ_BYTE(UInt8* value);
Int32 READ_STRING(Int8* value);
Int32 READ_RAW(void* value, Int16* len);

#endif /* DTPLINK_UTIL_H */