#include "strings.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define BUFFER_SIZE 128

typedef struct string_t string_t;

/** Create new string strut. */
string_t* string_create( )
{
	string_t* string = (string_t*)malloc( sizeof( string_t ) );
	string->buffer = (char*)malloc( BUFFER_SIZE );
	memset( string->buffer, 0, BUFFER_SIZE );
	string->length = BUFFER_SIZE;
	string->index = 0;

	return string;
}

void string_clear( string_t* string )
{
	string->index = 0;
	string->buffer[ string->index ] = 0;
}

int string_grow( string_t* string )
{
	string->buffer = (char*)realloc( string->buffer, string->length + BUFFER_SIZE );

	string->length += BUFFER_SIZE;

	memset( string->buffer + string->index, 0, string->length - string->index );

	return string->length - string->index;
}

void string_append( string_t* string, char* value )
{
	int length = strlen( value );

	while( ( string->length - string->index ) < length + 1 )
	{
		string_grow( string );
	}

	strcpy( string->buffer + string->index, value );

	string->index += length;
}

void string_append_char( string_t* string, char character )
{
	if( ( string->length - string->index ) < 2 )
	{
		string_grow( string );
	}

	string->buffer[ string->index ++ ] = character;
}

void string_appendf( string_t* string, char* format, ... )
{
	int available = 0;
	int needed = 0;

	while( 1 )
	{
		va_list ap;
		va_start( ap, format );

		available = string->length - string->index;

		needed = vsnprintf( string->buffer + string->index, available, format, ap );

		va_end( ap );

		if( needed > -1 && needed < available )
		{
			string->index += needed;
			return;
		}

		while( available <= needed )
		{
			available = string_grow( string );
		}
	}
}

/** Returns the string length of @param number */
int string_length( int number )
{ 
	char *buf;
	buf = (char*)malloc( 1 );
	memset( buf, 0, 1 );
	int currSize = 1;
	
	while (1)
	{
		int nxtSize = snprintf( buf ,strlen(buf), "%d");
		
		if ( nxtSize == -1 )
		{
			return currSize;
		}
		else
		{
			currSize += nxtSize;
			buf = (char*)realloc( buf, currSize );
			memset( buf, 0, currSize );
		}
	}
	return currSize;
}
