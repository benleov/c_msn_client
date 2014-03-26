#include <string.h>
#include "commandparser.h"
#include "debug.h"
#include "ssl/msnSSLAuth.h"

/* Message processing methods. */


/* Main incomming message parsing function. Returns -1 if
 * we should disconnect.  */
int parseLine(char *l)
{
	if(l != NULL)
	{				
		if(strcmp(l,"SYN"))  /* number of contacts */
		{		
			contactNumberRecvd(l);
			return -1;
		} 
		else if(strcmp(l,"LSG")) // a contact group
		{
			newContactGroupRecvd(l);
		}
		else if(strcmp(l,"LST")) // a contact info
		{
			newContactInfoRecvd(l);
		}
		else if(strcmp(l,"CHL"))
		{
			newChallangeRecvd (l);// this is a challenge 
		}
		else if(strcmp(l,"ILN")) // contact online status
		{
			contactStatusRecvd (l);
		}
		else if(strcmp(l,"FLN")) // a contact goes offline
		{
			contactOfflineRecvd(l);
		}
		else if(strcmp(l,"NLN")) // user coming online
		{
			contactOnlineRecvd(l);
		}
		else if(strcmp(l,"XFR"))
		{
			// received when a chat is already created !
			// new switchboard session
			//<<< XFR 10 SB 207.46.108.37:1863 CKI 17262740.1050826919.32308\r\n
			newSwitchBoardSessionRecvd(l);
		}
		else if(strcmp(l,"RNG"))
		{
			// invited to switchboard session
			// <<< RNG 11752013 207.46.108.38:1863 CKI 849102291.520491113 example@passport.com Example%20Name\r\n
			newSwitchBoardSessionRecvd(l);
		}
		else if(strcmp(l,"Inbox-Unread")) //	<<< Inbox-Unread: 102
		{
			emailsUnreadRecvd(l);
		}
		// TODO: Implement commented code. 
		//else if(startsWithaNumber(t)) //Out.write("NSI - error!");
		//{
		//	//this.checkError(t);
		//}
	}
	else
	{
			// null string passed
	}
	return 1;
} 

void contactNumberRecvd ( char *line )
{
	//TODO
	debugPrintStr("command parser: contact number recieved: ", line);

}
void newContactGroupRecvd ( char *line )
{
	//TODO
	debugPrintStr("command parser: newContactGroupRecvd: ", line);
}
void newContactInfoRecvd( char *line )
{
	//TODO	
	debugPrintStr("command parser: newContactInfoRecvd: ", line);
}
void newChallangeRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: newChallangeRecvd: ", line);
}
void contactStatusRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: contactStatusRecvd: ", line);
}
void contactOfflineRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: contactOfflineRecvd: ", line);
}
void contactOnlineRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: contactOnlineRecvd: ", line);
}
void newSwitchBoardSessionRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: newSwitchBoardSessionRecvd: ", line);
}

void emailsUnreadRecvd ( char *line )
{
	//TODO	
	debugPrintStr("command parser: emailsUnreadRecvd: ", line);
}
