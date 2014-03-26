#ifndef MSNCONNS_H_
#define MSNCONNS_H_

#endif /*MSNCONNS_H_*/

int createConnection(char *hostname , int port );
int sendData ( int streamID , char* message );
char* receive ( int streamID );
int closeConnection ( int streamID );
char* generateHTTPHeader ( char* page , char * host , int port );
char* generateHTTPAuthHeader ( char *page , char *host , int port , char *authProperty );
char* recvNonBlock ( int streamID , int *stopPtr );

