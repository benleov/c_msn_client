#ifndef MSN_H_
#define MSN_H_

#endif /*MSN_H_*/

#define MSG_1A 		"MSG "
#define MSG_1B 		" A "
#define MSG_1C		"\r\n"

#define MSG_2		"MIME-Version: 1.0\r\n"
#define MSG_3		"Content-Type: text/plain; charset=UTF-8\r\n"
#define MSG_4		"X-MMS-IM-Format: FN=Arial; EF=I; CO=0; CS=0; PF=22\r\n"
#define MSG_5		"\r\n"


#define STATUS_ONLINE		1
#define STATUS_OFFLINE 		2
#define STATUS_AWAY			3
#define STATUS_BUSY			4
#define STATUS_IDLE			5
#define STATUS_ON_THE_PHONE	6
#define STATUS_OUT_TO_LUNCH	7

#define STATUS_ONLINE_STR		"NLN"
#define STATUS_OFFLINE_STR 		"HDN"
#define STATUS_AWAY_STR			"AWY"
#define STATUS_BUSY_STR			"BSY"
#define STATUS_IDLE_STR			"IDL"
#define STATUS_ON_THE_PHONE_STR	"PHN"
#define STATUS_OUT_TO_LUNCH_STR	"LUN"

int msnConnect(char *username , char *password , int status );
int msnConnectUser(char *username , char *password , char* url , int port );
int connect();
int sendMSNVersion ( int streamID );
int initiate( int streamID , char* username );
int sendClientInfo ( int streamID , char* username );
int sendUserInfo ( int streamID , char* username );
int serverRedirectRequested ( char * line );
void disconnectMSN();
int serverRedirect(char *line );
int challangeHashReceived( char *line );
int sendTicket ( int streamID , char *ticket );
int sendSync( int sock );
int contactCountRecvd ( char *line);
int sendStatus( int sock , int status );
int parseContacts( char *line , int currContact);

 