/*
 *  dtplink communication buffer implemention
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

#include "dtplink_buffer.h"

void CRequestBufferInit(CRequestBuffer *pBuf)
{
	UInt32 i = 0;
	pBuf->length = pBuf->head = 0;
	memset(pBuf->buffer, 0, MAX_RTP_MSG_SIZE);
	//for(i = 0; i < MAX_RTP_MSG_SIZE; i++){ // FIXME: ugly code used on some platform don't have memset...
	//	pBuf->buffer[i] = 0;
	//}
}

void CResponseBufferInit(CResponseBuffer *pBuf)
{
	UInt32 i = 0;
	pBuf->length = pBuf->tail = 0;
	memset(pBuf->buffer, 0, MAX_RTP_MSG_SIZE);
	//for(i = 0; i < MAX_RTP_MSG_SIZE; i++){ // FIXME: ugly code used on some platform don't have memset...
	//	pBuf->buffer[i] = 0;
	//}

}

void DTPLink_ResetBuffers(CRequestBuffer *pReq, CResponseBuffer *pResp)
{
	CRequestBufferInit(pReq);
	CResponseBufferInit(pResp);
}
