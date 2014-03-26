/** Low level connection class. */

#include "debug.h"
#include "connection.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFFER_SIZE     8192 /* Receive buffer size */


/** Returns connection number, or -1 if connection couldnt be made. 
 *  If @param ip is set to CONNECTION_IP, hostname is expected to be an 
 *  ip address thus doesn't need resovling. IF ip is set to CONNECTION_URL
 *  string will try and be resolved.
 * */
int createConnection(char *hostname , int port )
{
	struct in_addr addr;
	struct sockaddr_in pin;
	struct hostent *hp;
	
	int sd = socket(AF_INET, SOCK_STREAM, 0); /* Socket descriptor */
	
	/* grab an Internet domain socket */
	if ( sd == -1 ) 
	{
		debugPrint("connection : createConnection : Socket Error ");
		perror("socket");
		return -1;
	}

	debugPrintStr( "connection : createConnection : Connecting via URL: " , hostname );
	memset(&pin, 0, sizeof(pin));
	
	hp = gethostbyname(hostname); 

	if (hp == NULL)
	 {
	 	debugPrint("connection : createConnection : Host is null. ");
		return -1;
	 }

	/* fill in the socket structure with host information */
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_family = AF_INET;
	pin.sin_port = htons(port);
	
	/* connect to PORT on HOST */
	if ( connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) < 0 ) 
	{
		debugPrint("connection : createConnection : Connecting Error. ");
		perror("connect");
		return -1;
	}
	else
	{
		debugPrint("connection : createConnection : Succesful Connection ");
		return sd; /* Connection has been successfuly made. */
	}
}
/** Sends data down stream. Returns 0 on success.  */
int sendData ( int streamID , char* message )
{
	if (send(streamID, message, strlen(message), 0) == -1) 
	{
		debugPrintStr( "msn : connection : sendData : Error sending message : " , message );
		perror("send");
		return -1;
	}
	else
	{
		debugPrintStr( "msn : connection : sent : " , message );
		return 0;
	}
}
/* Recieve data on specified stream. Return NULL on fail.  */
char* receive ( int streamID )
{
	char *dir=malloc(BUFFER_SIZE);
	
	int len = recv(streamID, dir, BUFFER_SIZE, 0);
	
	if ( len > BUFFER_SIZE )
	{
		debugPrint( "WARN : msn : connection : receive : Buffer too small!");
		len = BUFFER_SIZE;
	}
	
        if (len == -1) 
        {
        	debugPrint( "INFO : msn : connection : receive : Connection ending.");
            perror("recv");
            return NULL;
        }
        else
        {
			debugPrintStr( "INFO : msn : connection : receive : " , dir );
        	return dir;
        }
}

/** Non blocking read. If @param stopPtr is set to one, this will return. */
char* recvNonBlock ( int streamID , int *stopPtr )
{
	char *readBlock;
	readBlock = malloc(BUFFER_SIZE);
	memset(readBlock, 0, BUFFER_SIZE);
	//readBlock[0]=0; /* terminate string */
	
	char *line=malloc(BUFFER_SIZE);
	memset(line, 0, BUFFER_SIZE);
	
	int ret = 0; /* Return flag */
	
	while ( 1 )
	{
		int len = recv(streamID, line, BUFFER_SIZE, MSG_ERRQUEUE );
		
		debugPrintStr(" INFO : connection : recvNonBlock : line : ",line);
		debugPrintInt(" INFO : connection : recvNonBlock : length : ",len);
		debugPrintInt(" INFO : connection : recvNonBlock : stop flag : ",*stopPtr);
		switch (len)
		{
			case -1: /* Socket error */
				debugPrint( "WARN : msn : connection : recvNonBlock : Socket Error");
				ret=1;
				break;
			case 0: /* Socket close */
				debugPrint( "INFO : msn : connection : recvNonBlock : Socket Close");
				ret=1;
				break;
			case EWOULDBLOCK: /* Non block would block on this */
				debugPrint( "INFO : msn : connection : recvNonBlock : EAGAIN Block");
				ret=1;
				break;
			default: /* Recived data. Keep reading. */
				//line[len]=0; 								/* terminate string */
				// Reallocate readblock
				
				readBlock = (char*)realloc( readBlock, strlen(readBlock) + strlen(line) );
				sprintf(readBlock, "%s%s", readBlock,line); /* Copy to readBlock */
				memset(line, 0, BUFFER_SIZE); 			/* Clear line */
				ret = 1;
		}

		if (ret == 1 || *stopPtr == 1)
		{
			break;   // Error, or stop pointer has been set to 1
		}
	}
	readBlock[strlen(readBlock) -1] = 0;
	return readBlock;
}

/* Closes stream */
int closeConnection ( int streamID )
{
	debugPrintInt("connection : connetion closed: ", streamID);
	return close(streamID);
}
/** Returns HTTP Get request header. */
char* generateHTTPHeader ( char *page , char *host , int port )
{
	char *request=0;
    int request_len;
	char *reqType = "GET ";
	char *reqHeader = " HTTP/1.0\r\nUser-Agent:EKRClient\r\nHost: %s:%d\r\n\r\n";
	
	char *requestTemplate= malloc(strlen(reqType) + strlen(page) + strlen(reqHeader));
	requestTemplate = strcat( requestTemplate , reqType );
	requestTemplate = strcat( requestTemplate , page );
	requestTemplate = strcat( requestTemplate , reqHeader );
	
    request_len=strlen(requestTemplate) + strlen(page) + 6; /* Cheating here - valid port not going to be more than 6 characters */
    
    if(!(request=(char *)malloc(request_len)))
    {
      debugPrint("connection : generateHTTPHeader : memory allocation error ");
    }
    memset(request, 0, request_len);
    snprintf(request,request_len,requestTemplate,host,port);
	debugPrintStr("connection : generateHTTPHeader : request : ", request);
	return request;
}

/** Returns HTTPS Get request header. This could be extended to take an aray 
 *  of request properties instead of just auth. */
char* generateHTTPAuthHeader ( char *page , char *host , int port , char *authProperty )
{
	char *request=0;
	char *reqHeader = "GET %s HTTP/1.0\r\nUser-Agent:EKRClient\r\nHost: %s:%d\r\nAuthorization: %s\r\n\r\n";
	
	int memSize = strlen(reqHeader) + strlen(page) + strlen(host) + 6 + strlen(authProperty);

    if(!(request=(char *)malloc(memSize)))
    {
      debugPrint("connection : generateHTTPAuthHeader : memory allocation error ");
    }
    
    memset(request, 0, memSize);
    
    snprintf(request,memSize,reqHeader,page,host,port, authProperty);
	debugPrintStr("connection : generateHTTPHeader : header : ", request);
	return request;
}

