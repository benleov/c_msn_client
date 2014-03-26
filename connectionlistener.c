#define _REENTRANT

#include "connectionlistener.h"
#include "debug.h"
#include "connection.h"
#include "commandparser.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_t thread1; // Listening thread
int listening = 0;


/** Start Listening on this socket. */
int startListening( int streamID )  {
	
     int *socketIDPntr = &streamID;
     int  iret1;
     
     debugPrintInt("connection listener : startListening : streamID" , streamID);
     
    /* Create independent threads each of which will execute function */
     iret1 = pthread_create( &thread1, NULL, startThreadLoop, (void*)socketIDPntr);

	if (iret1)
	{
		debugPrint("connection listener : startListening : ERROR creating thread ");
	}
	
     debugPrintInt("connection listener : startListening : finished. Returned",iret1);
     return 1;
}

void *startThreadLoop( void *socketIDPntr )
{
	 int socketID = *((int *) socketIDPntr);
	 
     debugPrintInt("connectionlistener: startThreadLoop : started:", socketID );
    
	listening = 0;

    while (listening == 0)
    {
     	char* recvd = receive ( socketID );
     	
     	if ( recvd == NULL || parseLine(recvd) == -1 )
     	{
     		debugPrint("connectionlistener : startThreadLoop : NULL recieved.");
     		// listening = 1;
     	}
     	else
     	{
     		parseLine(recvd);
     	}
     	
     	free ( recvd ); /* Free received data */
     	
    } /* End listening loop */
     
     return socketIDPntr;
}

void stopListening()
{
	 listening = 1;
     pthread_join( thread1, NULL); 
}



