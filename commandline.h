#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#endif /*COMMANDLINE_H_*/

int processArgs(int argc, char *argv[]);
struct login init( int argc, char *argv[] );
void startCommandLineListener(int socket);
struct login 
		{
	    	char *username;
	    	char *password;
		};
