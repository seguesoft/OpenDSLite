// Device server application logic implemention file, this is the main file which you have to code.
#include "dtplink_ds.h"
#include "dsapp.h"

int main(int argc, char* argv[])
{
	printf("Demo device server. My URL is dtp://hello.sample.com\n");
	DTPLink_DSInit("hello.sample.com", "dns.seguesoft.net");
	DTPLink_DSLoop();
	return 0;
}

void device_server_main(short id)
{
	CRequest_index req;
	CResponse_index resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	switch(id){
	case 1:
		CRequest_index_unpack(&req);
		resp.d1 = req.d1;
		resp.d2 = req.d2;
		resp.r1 = req.d1 + req.d2;
		strcpy(resp.r2, "Hello, DTPLink!");
		CResponse_index_packup(&resp);
		printf("resp.d1 = %d\n", resp.d1);
		printf("resp.d2 = %d\n", resp.d2);
		printf("resp.r1 = %d\n", resp.r1);
		printf("resp.r2 = %s\n", resp.r2);
		break;
	default:
		break;
	}
}

/* example code without CRequest and CResponse support
	int ret =0;
	int d1 = 0;
	int d2 = 0;
	int r1 = 0;
	char *r2 = "Hello, DTPLink!";

	switch(id){
	case 1:
		ret = READ_DWORD((ULong32*)&d1);
		if(ret == -1){
			WRITE_DWORD(d1);
			WRITE_DWORD(d2);
			WRITE_DWORD(r1);
			WRITE_STRING(r2);
		}else{
			READ_DWORD((ULong32*)&d2);
			r1 = d1 + d2;
			WRITE_DWORD(d1);
			WRITE_DWORD(d2);
			WRITE_DWORD(r1);
			WRITE_STRING(r2);
		}
	    break;
	default:
	    break;
	}
*/
