#include <string.h>
extern char * chomp( char *str )
{
    int len ;
	len = strlen( str );
	if( len>=2 && str[len-2] == '\r' && str[len-1] == '\n' )
	{
	    str[len-2] = str[len-1] = 0;
	}
	else if( len >= 1 && (str[len-1] == '\r' || str[len-1] == '\n') )
	{
	    str[len-1] = 0;
	}
	return( str );
}


