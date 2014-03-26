#ifndef MSNSSLAUTH_H_
#define MSNSSLAUTH_H_

#endif /*MSNSSLAUTH_H_*/

#define		DALOGIN                       "DALogin="
#define		DASTATUS                      "da-status="
#define		TICKET                        "from-PP="
#define		SUCCESS                       "success"
#define		KEY_PASSPORT_URLS             "PassportURLs"
#define		KEY_LOCATION                  "Location"
#define		KEY_AUTHENTICATION_INFO       "Authentication-Info"

#define		AUTH_STR_PRE		"Passport1.4 OrgVerb=GET,OrgURL=http%3A%2F%2Fmessenger%2Emsn%2Ecom,sign-in="
#define		AUTH_STR_MID		",pwd="
#define		AUTH_STR_POST		","
#define		AUTH_PORT			443

//#define		PASSPORT_LIST_SERVER_ADDRESS  "https://nexus.passport.com/rdr/pprdr.asp"

#define		PASSPORT_LIST_SERVER_ADDRESS  "nexus.passport.com"
#define		PASSPORT_LIST_SERVER_PAGE  "/rdr/pprdr.asp"

#define		PASSPORT_LIST_SERVER_PORT	  80

#define SSL_KEY_FILE 					  "client.pem"
#define SSL_PASSWD						  "password"

char *startSSLAuthentication( char *username, char *password, char *strChallenge );
