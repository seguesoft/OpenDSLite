#include "dsapp.h"
#include "dtplink_util.h"

/* index */
void CRequest_index_packup(CRequest_index *message)
{
	WRITE_DWORD((const ULong32)(message->d1));
	WRITE_DWORD((const ULong32)(message->d2));
}

void CRequest_index_unpack(CRequest_index *message)
{
	READ_DWORD((ULong32*)(&message->d1));
	READ_DWORD((ULong32*)(&message->d2));
}

void CResponse_index_packup(CResponse_index *message)
{
	WRITE_DWORD((const ULong32)(message->d1));
	WRITE_DWORD((const ULong32)(message->d2));
	WRITE_DWORD((const ULong32)(message->r1));
	WRITE_STRING(message->r2);
}

void CResponse_index_unpack(CResponse_index *message)
{
	READ_DWORD((ULong32*)(&message->d1));
	READ_DWORD((ULong32*)(&message->d2));
	READ_DWORD((ULong32*)(&message->r1));
	READ_STRING(message->r2);
}
