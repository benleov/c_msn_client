#ifndef CONNECTIONLISTENER_H_
#define CONNECTIONLISTENER_H_

#endif /*CONNECTIONLISTENER_H_*/

/* Requires -lpthread to be added into linker & complier settings. */

int startListening( int streamID );
void stopListening();
void *startThreadLoop( void *ptr );
