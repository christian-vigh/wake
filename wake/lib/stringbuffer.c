/**************************************************************************************************************

    NAME
        stringbuffer.c

    DESCRIPTION
        Implements an array of string .

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/11]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include		"wake.h"
# include		"parse.h"


/*==============================================================================================================

    stringbuffer_init -
        Initializes a string buffer. The supplied line can be null.

  ==============================================================================================================*/
void	stringbuffer_init ( stringbuffer *  sb, char *  line, char  separator )
   {
	if  ( line  ==  NULL )
	   {
		sb -> data		=  
		sb -> next		=  NULL ;
	    }
	else
	   {
		sb -> data		=  
		sb -> next		=  xstrdup ( line ) ;
	    }
	sb -> separator		=  separator ;
    }


/*==============================================================================================================

    stringbuffer_reset -
        Resets a string buffer.

  ==============================================================================================================*/
void	stringbuffer_reset ( stringbuffer *  sb )
   {
	if  ( sb -> data  !=  NULL )
		free ( sb -> data ) ;

	sb -> data		=
	sb -> next		=  NULL ;
    }


/*==============================================================================================================

    stringbuffer_empty -
        Returns 1 if the specified string buffer is empty, false otherwise.

  ==============================================================================================================*/
int	stringbuffer_empty ( stringbuffer *  sb )
   {
	if  ( sb  ==  NULL  ||  sb -> data  ==  NULL )
		return ( 1 ) ;
	else
		return ( 0 ) ;
    }


/*==============================================================================================================

    stringbuffer_lines -
        Returns the number of lines remaining in the string buffer.

  ==============================================================================================================*/
int	stringbuffer_lines ( stringbuffer *  sb )
   {
	char *		p ;
	int		count	=  0 ;


	if  ( sb  !=  NULL  &&  sb -> next  !=  NULL )
	   {
		p	=  sb -> next ;

		while  ( * p )
		   {
			if  ( * p  ==  '\n' )
				count ++ ;

			p ++ ;
		    }

		if  ( * ( p - 1 )  !=  '\n' )
			count ++ ;
	    }

	return ( count ) ;
    }


/*==============================================================================================================

    stringbuffer_next -
        Returns the next available line in the specified string buffer, or NULL if empty or no more lines are
	available.

  ==============================================================================================================*/
char *	stringbuffer_next ( stringbuffer *  sb )
   {
	char *		p ;
	char *		result ;
	int		length ;


	// Strict checkings on stringbuffer integrity
	if  ( sb  ==  NULL  ||  sb -> data  ==  NULL  ||  sb -> next  ==  NULL )
		return ( NULL ) ;

	// Find next separator
	p	=  strchr ( sb -> next, sb -> separator ) ;

	// No separator found : this means that we have reached the last line of the buffer so duplicate the line
	// and reset the buffer
	if  ( p  ==  NULL )
	   {
		result		=  xstrdup ( sb -> next ) ;
		stringbuffer_reset ( sb ) ;

		return ( result ) ;
	    }
	// Otherwise, duplicate the string between current position and found separator
	else 
	   {
		length		=  p - sb -> next + 1 ;
		result		=  xmalloc ( length + 1 ) ;

		strncpy ( result, sb -> next, length ) ;
		result [ length ]	=  '\0' ;

		sb -> next	+=  length ;

		// Last string ended with a separator : don't count a additional string for that
		if  ( ! * ( sb -> next ) )
			stringbuffer_reset ( sb ) ;
	    }

	return ( result ) ;
    }


# endif		/*  WAKE_EXTENSIONS  */