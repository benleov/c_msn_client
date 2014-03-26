/* Processes command line input from user. */ 

#define _REENTRANT

#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

//#include <ncurses.h> TODO: look at getch()

#define MAX_PASS	30
	
char *username; // User's username
char *password; // User's password

char typedpassword[MAX_PASS];



/** Prints usage */
void printUsage ( )
{
   printf("usage: msn username@email.com -p password");
}
/** Infinite command line loop*/
void startCommandLineListener(int socket)
{
	char buffer[256];
	
	for(;;) 
	{
		strcpy( buffer, "Type 'X' to quit\n" );
   
		write( (int)socket, buffer, strlen( buffer ) );

		if ( read( (int)socket, buffer, 255 ) > 0 ) 
		{
			if ( buffer[0] == 'X' ) 
			{
				close( (int)socket );
				pthread_exit( NULL );
			}
		}	
	}
}

/** Retrieves arguments from commandline. Returns -1 if compulsary 
 *  arguments wern't passed in (a username in this case, athough we 
 *  really need at least a username and password to login.) */
int processArgs(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)  /* Skip argv[0] (program name). */
    {
        if (strcmp(argv[i], "-p") == 0)  /* Process password argument */
        {
        	i++;
            password = argv[i];
        }
        else if (strcmp(argv[i], "--help") == 0)    // If --help specified, print usage.
        { 
			return -1;
        }
		else // Get usename.
		{
			username = argv[i];
		}
	}
	
	if ( username != NULL) // We have all the details we need.
	{
		return 1;
	}
	
	return -1;
}
/** Returns one if succesfully retrieves user's login credentials. */
int passwordPrompt()
{
		puts("Please enter your password: ");
		
		// getch - Hidden password
		
//		for( ;; ) 
//		{
//	 	char x = getch();
//	 	
//	 	printf("Char:   %c ",x );
//	 	
//	 	if ( x == 'x')
//	 		break;
//	 	
//		}
		
		// Non- hidden password
		fgets(typedpassword, MAX_PASS, stdin);
		
		if (typedpassword[strlen(typedpassword)-1] == '\n') 
		{
			typedpassword[strlen(typedpassword)-1] = '\0'; // Remove /n char
		}
		password = typedpassword;	
        return 1;
}

/** Collects username / password from user. */
struct login init( int argc, char *argv[] )
{
        printf( "Starting Command Line MSN \n" );

        if ( processArgs(argc,argv) == -1 ) 
        {
			 printUsage();
			 exit(0);
        } 
        else // user has entered compulsary credentials.
        {
        	// Check if password was specified as command-line arg
        	if ( password == NULL )
        	{
        	
        	 	if ( passwordPrompt() != 1) //  Failed to get password
        	 	{
        	 		
        	 		printUsage();
        	 		exit(0);
        	 	}
        	} 	
        }

	    struct login currentUser;

		currentUser.username = username;
		currentUser.password = password;

        return currentUser;	
}
