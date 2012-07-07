/*
 *  dtplink communiction io function header file
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

#ifndef DTPLINK_IO_H
#define DTPLINK_IO_H

#include "dtplink_config.h"

// Include specific platform header
#if DS_PLATFORM_TYPE == DS_PLATFORM_WINDOWS
	#include "platform/windows/dtplink_windows_netio.h"
#elif (DS_PLATFORM_TYPE == DS_PLATFORM_LINUX) || (DS_PLATFORM_TYPE == DS_PLATFORM_ANDROID)
	#include "platform/linux/dtplink_linux_netio.h"
#endif

#endif /*DTPLINK_IO_H*/
