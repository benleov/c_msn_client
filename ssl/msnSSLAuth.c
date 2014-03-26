#include "msnSSLAuth.h"

#include "../debug.h"
#include "common.h"
#include "client.h"

#include "../connection.h" /* Standard connection */

int sendTicket ( int streamID , char *ticket );

	SSL *ssl;
	SSL_CTX *ctx;
	BIO *sbio;
	int sock;
	int r,len;
	char buf[1024];
	char buf2[1024];
/** Users password is sent via ssl after inital connection handshaking. 
 *  After we are authenticated, we chat using the standard connection.
 *  @param line is the line we have just recieved from the server
 *  Full line: STMP : USR 22 TWN S lc=1033,id=507,tw=40,ru=http%3A%2F%2Fmessenger%2Emsn%2Ecom,ct=1175819023,kpp=1,kv=9,ver=2.1.6000.1,rn=QnKlBwob,tpf=980c0193c3f4fd73cb26dda447d55281
 *  Function should have trim'ed upto lc= off already. 
 *  Returns authentication ticket , or NULL on fail. */
char* startSSLAuthentication( char *username, char *password, char *strChallenge )
{

	/* Get list of passport servers via ssl connection */
	char *header = generateHTTPHeader(PASSPORT_LIST_SERVER_PAGE,PASSPORT_LIST_SERVER_ADDRESS,AUTH_PORT);
	ctx = initialize_ctx(SSL_KEY_FILE,SSL_PASSWD);
	sock = tcp_connect( PASSPORT_LIST_SERVER_ADDRESS , AUTH_PORT );
    ssl=SSL_new(ctx);
    sbio=BIO_new_socket(sock,BIO_NOCLOSE);
    SSL_set_bio(ssl,sbio,sbio);
    
    if( SSL_connect(ssl)<= 0 ) 
   {
    	debugPrint("ERROR: SSL Connection failed. aborting. ");
    	return NULL;
    }
    int request_len =  strlen(header);
    r=SSL_write(ssl,header,request_len);
    
    r=SSL_read(ssl,buf,BUFSIZZ);
   	debugPrintStr("msnSSLAuth : startSSLAuthentication : received1 ", buf);

    char *recvd = buf;
    
	/* Parse Login URL */
	/* Example:
	 PassportURLs: DARealm=Passport.Net,DALogin=login.live.com/login2.srf,DAReg=https://accountservices.passport.net/UIXPWiz.srf,Properties=https://accountservices.msn.com/editprof.srf,Privacy=https://accountservices.passport.net/PPPrivacyStatement.srf,GeneralRedir=http://nexusrdr.passport.com/redir.asp,Help=https://accountservices.passport.net,ConfigVersion=14
	 */
	
	 char * loginURLline = strstr ( recvd, DALOGIN );
	 loginURLline +=(strlen(DALOGIN));
	 
	 int loginURLEnd = strcspn(loginURLline,"/"); /* complete URL includeing login page */
	 char * passportLoginURL = malloc(loginURLEnd); 
	 passportLoginURL = strndup( loginURLline , loginURLEnd );
	 
	 int loginPageStart =  strcspn( loginURLline , "/" );
	 int loginPageEnd = strcspn( loginURLline , "," );
	 char * passportLoginPage = malloc( loginPageEnd - loginPageStart );
	 
	 passportLoginPage = strndup( loginURLline , loginPageEnd );
	 passportLoginPage+= loginPageStart;
	 
	 debugPrintStr( "msnSSLAuth : startSSLAuthentication : loginURL : ", passportLoginURL );
	 debugPrintStr( "msnSSLAuth : startSSLAuthentication : loginPage : ", passportLoginPage );
	  
	 /* Create auth string 
	   
	   "Passport1.4 OrgVerb=GET,OrgURL=http%3A%2F%2Fmessenger%2Emsn%2Ecom,sign-in="), 
	   your URL-encoded passport, ",pwd=", 
	   your URL-encoded password, ",",
	   and the challenge string given to you by the Notification Server.
	   */
	 
	 int memSize = strlen(AUTH_STR_PRE)+ strlen (username) + strlen (AUTH_STR_MID) + strlen (password) +  strlen(AUTH_STR_POST)+ strlen(strChallenge) + 1;
	 char  *strAuthString = malloc(memSize);
	 memset(strAuthString, 0, memSize);
	 	 
	 strcat(strAuthString, AUTH_STR_PRE);
	 strcat(strAuthString, username);
	 strcat(strAuthString, AUTH_STR_MID);
	 strcat(strAuthString, password);
	 strcat(strAuthString, AUTH_STR_POST);
	 strcat(strAuthString, strChallenge);
	 
	 debugPrintStr( "msnSSLAuth : startSSLAuthentication : authString : ", strAuthString );
	 
	/* Create HTTP header */ 

	header = generateHTTPAuthHeader ( passportLoginPage , passportLoginURL , AUTH_PORT ,strAuthString );
	
	/* Create authentication SSL connection */
	
	ctx = initialize_ctx(SSL_KEY_FILE,SSL_PASSWD);
	sock = tcp_connect( passportLoginURL, AUTH_PORT);
    ssl=SSL_new(ctx);
    sbio=BIO_new_socket(sock,BIO_NOCLOSE);
    SSL_set_bio(ssl,sbio,sbio);
    
    if( SSL_connect(ssl)<= 0 ) 
   {
    	debugPrint("ERROR: SSL Connection failed. aborting. ");
    	return NULL;
    }
	
    request_len =  strlen(header);
    r=SSL_write(ssl,header,request_len);
    
    debugPrint("msnSSLAuth : startSSLAuthentication : reading response ");
      /* Now read the server's response */    
	char *receivedHeader;
	receivedHeader = malloc(3000);
	memset(receivedHeader, 0, 3000);
		
    while(1)
    {
   	int breakFlag = 0;
	
	r=SSL_read(ssl,buf,BUFSIZZ);
		
      	switch(SSL_get_error(ssl,r))
      	{
        	case SSL_ERROR_NONE:
          		len=r;
          		printf(stderr,"No Error\n");
          		break;
        	case SSL_ERROR_ZERO_RETURN:
          		breakFlag=1;
          		break;
        	case SSL_ERROR_SYSCALL:
          		fprintf(stderr,
            	"SSL Error: Premature close\n");
          		breakFlag=1;
          		break;
        	default:
          		berr_exit("SSL read problem");
          		breakFlag=1;
          		break;
      }
        
		if ( breakFlag == 1)
		{
			break;
		}
		else
		{
			char *newLine = malloc(r+1); /* Add new line to string */
			memcpy(newLine,buf,r);
			sprintf(receivedHeader, "%s%s", receivedHeader,newLine);
		}
    }

   	 debugPrintStr("msnSSLAuth : startSSLAuthentication : buffer: ", receivedHeader);
     //fwrite(buf,1,len,stdout);
    
	 char *daStatusline = strstr( receivedHeader, DASTATUS );
	 daStatusline +=(strlen(DASTATUS));
	 int statusEnd = strcspn(daStatusline,","); 
	 
	 char *daStatus = malloc(statusEnd); 
	 memset(daStatus, 0, statusEnd); 
	 daStatus = strndup( daStatusline , statusEnd );
	 
	 if (0 == strcmp(daStatus, SUCCESS)) /* Get ticket  */
	 {	 	
	 	debugPrint("msnSSLAuth : startSSLAuthentication : login success");
	 	
	 	char *ticketLine = strstr( receivedHeader, TICKET );
	 	ticketLine +=(strlen(TICKET)) + 1;
	 	int ticketEnd = strcspn(ticketLine,"'"); 
	 	
	 	char *ticket = malloc(ticketEnd+1);
	 	memset(ticket, 0, ticketEnd+1); 
	 	ticket = strndup( ticketLine , ticketEnd );
	 	return ticket;
	 }
	 else
	 {
	 	debugPrint("msnSSLAuth : startSSLAuthentication : login fail");
	 	return NULL;
	 }

}

