#include "main.h"
#include "debug.h"
#include "commandline.h"
#include "msn.h"

#include "ssl/msnSSLAuth.h"

#include <stdio.h>

/* Main File. */
int main(int argc, char *argv[])
{
	struct login loginDetails;
	loginDetails = init(argc, argv); /* Prompt for user details */
	
	debugPrintStr("main: username: ",loginDetails.username ); // TODO: URL Encoded this
  	debugPrintStr("main: password: ", loginDetails.password);// TODO: URL Encoded this

	//startSSLAuthentication( loginDetails.username , loginDetails.password , "lc=1033,id=507,tw=40,ru=http%3A%2F%2Fmessenger%2Emsn%2Ecom,ct=1175819023,kpp=1,kv=9,ver=2.1.6000.1,rn=QnKlBwob,tpf=980c0193c3f4fd73cb26dda447d55281" );

	/* By now we have username credentials. Lets start a connection.*/
	
	msnConnect(loginDetails.username,loginDetails.password, STATUS_ONLINE); //"email%address%2Ecom%0D%0A"
	
	return 0;
}


