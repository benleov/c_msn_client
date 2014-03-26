#include "msn.h"
#include "connection.h"
#include "commandparser.h"
#include "connectionlistener.h"
#include "ssl/msnSSLAuth.h"
#include "debug.h"
#include "util/integers.h"
#include "commandline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


/** MSN controller  */

#define NOTIFCATION_PORT         1863 /*4444 1863 */   /* primary server port */
#define NOTIFICATION_URL        "messenger.hotmail.com"  /* primary server url */
			
#define  DISPATCH_URL  			"gateway.messenger.hotmail.com"	/* secondary server url */
#define  DISPATCH_PORT 			80	
			
								
#define MSN_VERSION				"VER 0 MSNP8 CVR0\r\n" /* Version String */

#define MSN_CLIENT_PRE			"CVR 2 0x0409 win 4.10 i386 MSNMSGR 5.0.0544 MSMSGS "
#define MSN_CLIENT_POST			"\r\n"

#define MSN_USER_1 				"USR "
#define MSN_USER_2				" TWN I "

#define MSN_REDIRECT_PRE		"XFR"

/* Variables */

static int trID= 20; 	/* Connection identifcation number */
int streamID = 0; 		/* Connection ID flag */
char *redirectLine; 	/* Redirect IP, if one has been recieved from the server. */
						/* If none has been recieved, this will be null. */

char *user;				/* Username TODO: Move this to user struct, and create array */
char *pass;				/* Password       for multiple logins */
int currPort;					  
char *currUrl;
char *challengedHash;

/* --- Contact list --- */
char **contacts;   /* Array of contacts */ 
int contactCount = -1; /* Number of contacts */

int msnConnect(char *username , char *password , int status )
{
	int sock = msnConnectUser(username , password , NOTIFICATION_URL , NOTIFCATION_PORT );
	 
	debugPrintInt("INFO  msnConnect : returned",sock);
	
	if ( sock == -1 )
	{
		debugPrint("WARN: msnConnect : Couldnt connect to notification server. Trying dispatch server.");
		sock = msnConnectUser(username , password , DISPATCH_URL , DISPATCH_PORT ); 
	}
	
	if ( sock == -1 )
	{
		debugPrint("ERROR: msnConnect : Both primary & secondary server failed. Aborting. ");
		return -1; /* Both primary  & secondary URLS failed. */
	}
	else /* Connection established. Start authentication then listening thread. */
	{
		debugPrint("INFO: msnConnect : Connection Established ");
		
		/* Get passport login server (msnSSLAuth) */
		
		char *ticket = startSSLAuthentication( username, password, challengedHash );
		
		if ( ticket != NULL )
		{
			if ( sock == -1)
			{
				debugPrint("ERROR: msnConnect : Could not send ticket : Connection closed");
			}
			else
			{
			/* Send ticket to server. */
			sendTicket ( sock , ticket );
			char *recvd = receive( sock );
			
			/* Now we are fully authenticated. */
			
			sendSync(sock); /* Sync - Server sends back contacts & passport info */
			
			int stopRead = 0;			// Flag to indicate that all contacts have 
			int *stopReadPtr = &stopRead; // been read in
			int currContact = 0;
			
			while (stopRead != 1)
			{
				recvd = recvNonBlock( sock,stopReadPtr );
				
					if (contactCount == -1) // Contact count hasnt been recieved
					{
						contactCount = contactCountRecvd(recvd);
						
						if ( contactCount != -1) // We have recieved contact count..start malloc'ing
						{
							debugPrintInt("INFO: msnConnect : Contact count received: ", contactCount);
							contacts = malloc(sizeof(char*) * contactCount);
							currContact += parseContacts(recvd,currContact);
						}
						// else { it failed... ignore }
					}
					else // contact number received..read them in
					{
						
						debugPrintInt("INFO: msnConnect : Parsing contact number : ", currContact);
						/* TODO: Receive in contacts details and save to array.
						 * once done, set contactsRecvd to one  */
						 
						currContact += parseContacts(recvd,currContact);
						
						debugPrintInt("INFO: msnConnect : Contact Count : ", currContact);
												
						if ( currContact >= (contactCount) ) // We are done reading in contacts 
						{
							debugPrint("INFO: msnConnect : All contacts parsed! ");
							stopRead = 1;
							break;
						}
					}
				}
				
				/* send MSN out status */
				
				sendStatus( sock , status );
				
				// Seperate thread that calls command parser
				// that have been recieved from the server
				
  				pthread_t dummy_thr;
    			pthread_create( &dummy_thr, NULL, startCommandLineListener, (void*)sock );
				
									  
				// startServerListener();// listeners to commands from the user	
				// Perhaps move to command parser??
				// while (/* break */)
				// {				
				//	 *recvd = receive( sock );
				// } 
				
			}
		}
		else // SSL Auth failed.
		{
			debugPrint("INFO: msnConnect : Ticket not received. Auth fail ");
			
		}
		 
		/*startListening( streamID ); */
		/* stopListening();*/     				
		return 1;
	}
		
}

/** Creates connection to MSN server. Will recursivly call itself if a redirect
 *  commmand is issued.  */
int msnConnectUser(char *username , char *password , char* url , int port )
{
	user = username;
	pass = password;
	currUrl = url;
	currPort = port;
	
    streamID = createConnection( url , port ); 
   
   	if( streamID != -1 )
	{ 
		debugPrintInt( "DEBUG : msn : msnConnectUser : connected streamID :" , streamID );

		switch ( initiate( streamID , username )) // Send user data, pre SSL connection.
		{
			case 0: 
			/* Now we have a connection established our listening thread can
			 * do the rest. */  	 
				debugPrint("INFO : msn : msnConnectUser : initiated");
				return streamID;
			break;
			case -1: // Connection error
				debugPrint("ERROR : msn : msnConnectUser : connection error during initiate");
				return -1;
			break;
			case -2: // Redirect
				debugPrint("INFO : msn : msnConnectUser : redirect");
				disconnectMSN();
				return serverRedirect( redirectLine ); /* Recursivly calls this to reconnect. */
			break;
			default: // Unknown return
				debugPrint( "ERROR: msn : msnConnectUser : unknown initiate return");
				return -1;
			break;
		}
	}
	else /* Connection stream invalid */
	{
		debugPrintStr("ERROR : msn : msnConnectUser : couldnt connect to server",url);
		return -1;
	}
}

int startIncommingChannel()
{
	// Load command parser. Let it deal out the work.
	return -1;
}

/** Initiates connection. Return -1 if connection is invalid, -2 if
 *  server has issued redirect, or 0 if everything is OK. 
 *  Apon completion challange hash char* will be set. */
int initiate( int streamID , char *username )
{
	if ( sendMSNVersion ( streamID ) ) /* Send MSN version  */
	{
		debugPrint("ERROR : msn : initiate : couldnt send msn version" );
		return -1; // connection invalid
	} 
	else
	{
		if ( serverRedirectRequested ( receive( streamID )) == 0 )
		{
			debugPrint("INFO : msn : initiate server redirect 1" );
			return -2;	// Redirect
		}
		
		if ( sendClientInfo ( streamID , username ) ) /* Send Client Info */
		{
			debugPrint("ERROR : msn : initiate : couldnt send client info" );
			return -1;	// connection invalid
		}
		else
		{
			if ( serverRedirectRequested ( receive( streamID )) == 0 )
			{
				return -2; 	/* Redirect */
			}
			if ( sendUserInfo ( streamID , username ) == -1 ) /* Send User Info */
			{
				return -1; 	/* connection invalid */
			}
			else
			{
				char *recvd = receive( streamID );
				
				if ( serverRedirectRequested ( recvd ) == 0 )
				{
					return -2;	/* Redirect */
				}
				else if ( challangeHashReceived ( recvd ) == 0 )
				{
					debugPrint("INFO : msn : initiate : success" );
					return 0;	/* Success */
				}
				else 
				{
					return -1; /* Challange hash hasn't been sent. Abort. */
				}
			}
		}
	}
}

/** Returns connection ID. */
static int getTrID()
{
	trID++;	
	return trID % 4294960 ;
}

int sendMSNVersion ( int streamID )
{
	int ret = sendData ( streamID , MSN_VERSION ); 
    return ret;
}
int sendClientInfo ( int streamID , char* username )
{
	int memSize = strlen(MSN_CLIENT_PRE)+ strlen (username) + strlen(MSN_CLIENT_POST)+1;
	char  *sendBuffer = malloc(memSize);
	memset(sendBuffer, 0, memSize);
	
	strcat(sendBuffer, MSN_CLIENT_PRE);	
	strcat(sendBuffer, username);
	strcat(sendBuffer, MSN_CLIENT_POST);
	
	int ret = sendData ( streamID , sendBuffer ); 
	//free(sendBuffer);
	return ret;
}

int sendUserInfo ( int streamID , char* username )
{
	int memSize = strlen(MSN_USER_1) + strlen(MSN_USER_2) + strlen(username) + strlen(MSN_CLIENT_POST)  + 1;
	char *sendBuffer = malloc(memSize);
	memset(sendBuffer, 0, memSize);
	
    char id [ 100 ];  
 	sprintf(id, "%d", getTrID());
 	
	strcat( sendBuffer, MSN_USER_1 );	
	strcat( sendBuffer, id );
	strcat( sendBuffer, MSN_USER_2 );
	strcat( sendBuffer, username );
	strcat( sendBuffer, MSN_CLIENT_POST );

	int ret = sendData ( streamID , sendBuffer ); 
	//free(sendBuffer);
	return ret;
}
/** Final part of authentication - send ticket to server. */
int sendTicket ( int streamID , char *ticket )
{
	
	int memSize = strlen(ticket)  + 50; //T TODO: Calculate size of getTRID.
	
	char  *sendBuffer = malloc(memSize);
	memset(sendBuffer, 0, memSize);
	sprintf(sendBuffer, "%s%d%s%s%s", "USR ",getTrID()," TWN S ",ticket,"\r\n");
	
	debugPrintStr("INFO : msn : sendTicket : sending Ticket :", sendBuffer );
	
	int retVal = sendData( streamID , sendBuffer );
	//free(sendBuffer);
	return retVal;
	
}

/** Process data sent from the server before we have established a proper connection.
 *  We need this before we start listening as sometimes we will have to redirect to
 *  another msn server. Returns 0 if we should reconnect. */
int serverRedirectRequested ( char *line ) 
{
	debugPrintStr("INFO : msn : serverRedirectRequested line " , line );
	
	if ( strlen (line ) > 2 ) // At least 3 chars
	{	
		char cmdStr[4];				// Copy first 3 chars
		strncpy(cmdStr, line, 3);	// TODO: Does strncpy terminate string?
		cmdStr[3] = 0;
		int comp = strcmp(cmdStr,MSN_REDIRECT_PRE);

		if ( comp == 0 ) // Server is issuing a redirect.
		{
			debugPrint("INFO : msn : serverRedirectRequested : Redirecting... ");
			redirectLine = line;
			return 0;
		}
		else
		{
			debugPrint("INFO : msn : serverRedirectRequested : No redirect " );
			return 1;
		}
	}
	else
	{
		return 1; // Connection still good.
	}
	return 1;
}
/** Disconnects, parses new IP and port from url then attempts to reconnect.
 *  Example redirect line: 
 *  "XFR 2 NS <new server ip>:<new server port> 0 207.46.104.20:1863\r\n"
 * */
int serverRedirect(char *line )
{
	int ipOffset= 9; // How many characters from the start of the line
 
	int end = strcspn(line,":");						// Pointer to :
	char* ip = malloc(end-8);							// Malloc some space 
	ip = strndup( line + ipOffset+1, end - ipOffset -1);	// Copy the string
								//TODO: Shouldnt we malloc end-ipOffset?

	int portEnd = strcspn(line + end ," ");		// Pointer to " "
	char* port = malloc(portEnd); 
	port = strndup( line + end + 1 , portEnd );
	
	if ( DEBUG )
	{
		printf ("INFO : msn : serverRedirect: line : %s\n", line );
		printf ("INFO : msn : serverRedirect: ip : %s\n" , ip);
		printf ("INFO : msn : serverRedirect: port : %s\n" , port);
	}

	/* Update class variables. */
	
	currUrl = ip;			
	currPort = atoi(port); // Convert to int
	
	/* Lets try reconnecting */
	return msnConnectUser(user , pass , currUrl , currPort );
}

/* 
  USR 3 TWN S lc=1033,id=507,tw=40,fs=1,ru=http%3A%2F%2Fmessenger%2Emsn%2Ecom,ct=1062764229,kpp=1,kv=5,ver=2.1.0173.1,tpf=43f8a4c8ed940c04e3740be46c4d1619\r\n
 and i have to take the "lc= .. 9" 
  */

int challangeHashReceived( char *line )
{
	int chIndx = strcspn(line,"lc=");
	challengedHash = line + chIndx;
	debugPrintStr( "msn : challangeHashReceived : " , challengedHash );
	return 0;
} 

int sendSync( int sock )
{
	return sendData (sock , "SYN 13 0\r\n"); 
}
/** Returns number of contacts recieved. Returns -1 if line doesn't contain
 *  current number of contacts. We are looking for this line: SYN 13 1 5 1 */
int contactCountRecvd (char *line)
{
	char *idx = strstr(line,"SYN");

	if (idx == NULL) /* Doesnt exist. */
	{
		return -1;	
	}
	else
	{
		debugPrintStr( "INFO msn : contactCountRecvd : contacts recieved " , idx );
		
		// The string should be like this, so split on " ", and
		// get the second to last number (ie 7) SYN 13 1 7 1
		
		char* token = strtok (idx, " ");
		int cnt = 0;
		
		char* contactCnt = NULL;
		
		while ( token != NULL) // SYN 13 1 7 1
		{
			token = strtok (NULL, " ");
			if ( cnt == 2)
			{
				contactCnt = token;
				break;
			}
			
			cnt++;
		}
		
		debugPrintStr( "INFO msn : contactCountRecvd : Contact Count : " , contactCnt );
		return atoi(contactCnt);	// Convert token to intger 
	}
	
}
/** Checks for occurances of LST, and expects
 *  line to be in the format:
 *  LST ben_leov@hotmail.com mpd%20ftw! 11 0
*/
int parseContacts( char* line, int currContact )
{
		debugPrintStr( "INFO msn : parseContact : contact line : " , line );
			
		char* token = NULL;
		token = strtok (line, "L"); // First look for an "L"
		
		int contactCnt = 0;
		
		while ( token != NULL)   // Then look for ST after it.
		{
			if ( strlen(token) > 2 && token[0] == 'S' && token[1] == 'T' )
			{
				contacts[currContact++] = token;
				debugPrintStr( "INFO msn : parseContact : contact parsed : " , token );
				contactCnt++;
			}
			token = strtok (NULL, "L");
		}
		
		return contactCnt;
}

/** Sends server out status */
int sendStatus( int sock , int status )
{
	char *statusStr;
	switch (status)
	{	
		case STATUS_ONLINE:	
			statusStr=STATUS_ONLINE_STR;
			break;
		case STATUS_OFFLINE: 
			statusStr=STATUS_OFFLINE_STR;
			break;
		case STATUS_AWAY:	
			statusStr=STATUS_AWAY_STR;
			break;
		case STATUS_BUSY:	
			statusStr=STATUS_BUSY_STR;
			break;
		case STATUS_IDLE:	
			statusStr=STATUS_IDLE_STR;
			break;
		case STATUS_ON_THE_PHONE:	
			statusStr=STATUS_ON_THE_PHONE_STR;
			break;
		case STATUS_OUT_TO_LUNCH:	
			statusStr=STATUS_OUT_TO_LUNCH_STR;
			break;
		default:
			statusStr=NULL;
		break;	
	}
	if (statusStr!=NULL)
	{
		int tr = getTrID();
		char *statusLine = malloc( 18 + strlen(statusStr) + integerLength(tr));
		sprintf(statusLine, "%s%d%s%s%s","CHG ",tr," ",statusStr," 268435456\r\n");
		debugPrintStr( "INFO : msn : sendStatus : " , statusLine );
		sendData(sock,statusLine);
		return 1;
	} 
	else
	{
		debugPrintInt( "ERROR : msn : sendStatus : Unknown Status : " , status );
		return -1;
	}
}

/* Sends a message */

int sendMessage (int sock, char* message)
{	
	int tr = getTrID();
	int trLen = integerLength(tr);
	
	int headerLen = strlen(MSG_2) + strlen(MSG_3) + strlen(MSG_4) + strlen(MSG_5) + strlen(message); 
	char *line1 = malloc(strlen(MSG_1A) + trLen + strlen(MSG_1B) + integerLength(headerLen) + strlen(MSG_1C));
	sprintf(line1,"%s%d%s%d%s",MSG_1A,tr,MSG_1B,headerLen,MSG_1C);
	char *mess = malloc( strlen(line1) + headerLen + 1); 
	sprintf(mess,"%s%s%s%s%s",line1,MSG_2,MSG_3,MSG_4,MSG_5);
	
	return sendData(sock,mess);
}


/** Stop any current connection with MSN. */
void disconnectMSN()
{
	if ( streamID != 0 )
	{
		closeConnection(streamID); 			// Close connection
		streamID = 0;
	}
	else
	{
		printf("WARN : msn : disconnectMSN : Stream ID is 0 : Ignoring ");	
	}
}


