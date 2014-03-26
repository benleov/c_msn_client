#include "debug.h"
#include <stdio.h>


void debugPrint ( char *line )
{
	if ( DEBUG )
	{
		printf ("++ %s\n",line);	
	}
}
void debugPrintInt ( char *line , int i ) // Debug println
{
	if ( DEBUG )
	{
		printf ("++ %s : %d\n",line , i);	
	}
}

void debugPrintStr ( char *line , char *cha ) // Debug println
{
	if ( DEBUG )
	{
		printf ("++ %s : %s\n",line,cha);	
	}
}
