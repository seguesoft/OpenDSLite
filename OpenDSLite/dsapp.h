#ifndef _DSAPP_H_
#define _DSAPP_H_

#include "dtplink_config.h"
#pragma pack(1)
/* index */
typedef struct {
	UInt16 id;
	Int32	d1;
	Int32	d2;
} CRequest_index;
extern void CRequest_index_packup(CRequest_index *message);
extern void CRequest_index_unpack(CRequest_index *message);

typedef struct {
	UInt16 id;
	Int32	d1;
	Int32	d2;
	Int32	r1;
	Int8	r2[256];
} CResponse_index;
extern void CResponse_index_packup(CResponse_index *message);
extern void CResponse_index_unpack(CResponse_index *message);

#endif
