/**************************************************************************************************************

    NAME
        message.c

    DESCRIPTION
        Builtin functions for displaying messages during the build process.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/02]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"wake.h"


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_message - Message-display function.
 *
 *   PROTOTYPE
 *	$(<log>		text...)
 *	$(<warning>	text...)
 *	$(<error>	text...)
 *	$(<print>	text...)
 *
 *   DESCRIPTION
 *	Displays a message to standard output. The <error> function terminates the program, while the <log> one
 *	logs it both to standard output and to the file specified by the <LOGFILE> variable.
 *
 *   PARAMETERS
 *	text -
 *		Text to be displayed. A newline is always appended if the supplied value does not end with a 
 *		newline character.
 *
 *==============================================================================================================*/
# define	MESSAGE_TYPE_UNKNOWN			0	/* Unknown function has been invoked - this means that the function table has an incorrect entry */
# define	MESSAGE_TYPE_WARNING			1	/* The <warning> function has been invoked	*/
# define	MESSAGE_TYPE_ERROR			2	/* <error> function				*/
# define	MESSAGE_TYPE_LOG			3	/* <log> function				*/
# define	MESSAGE_TYPE_PRINT			4	/* <print> function				*/

WAKE_BUILTIN ( wake_builtin_message )
   {
	time_t			now	=  time ( NULL ) ;
	struct tm *		tmp	=  localtime ( & now ) ;
	char			timestamp [ 64 ] ;
	int			i,
				size	=  0 ;
	char *			p ;
	char *			message ;
	int			message_type		=  MESSAGE_TYPE_UNKNOWN ;
	int			include_timestamp	=  1 ;


	// Determine the type of function invoked
	if  ( ! strcmp ( funcname, "<warning>" ) )
		message_type	=  MESSAGE_TYPE_WARNING ;
	else if  ( ! strcmp ( funcname, "<error>" ) )
		message_type	=  MESSAGE_TYPE_ERROR ;
	else if  ( ! strcmp ( funcname, "<log>" ) )
		message_type	=  MESSAGE_TYPE_LOG ;
	else if  ( ! strcmp ( funcname, "<print>" )  ||  ! strcmp ( funcname, "<message>" ) )
	   {
		message_type		=  MESSAGE_TYPE_PRINT ;
		include_timestamp	=  0 ;
	    }

	// Complain if incorrect function name has been supplied. This means that the wake_function_table table contains an entry
	// that points to this function, but having a name not handled so far
	if  ( message_type  ==  MESSAGE_TYPE_UNKNOWN )
		OS ( error, NILF, "Internal error : Unknown function \"%s\" defined in Thrak function table for wake_builtin_message().", funcname ) ;

	// Build a timestamp
	strftime ( timestamp, sizeof ( timestamp ), "[%Y-%m-%d %H:%M:%S] \b", tmp ) ;

	// Compute the size of the final message (approximately...)
	for  ( i = 0 ; argv [i]  !=  NULL ; i ++ )
		size	+=  strlen ( argv [i] ) ;

	size		+= 128 ;					// Approximate computation
	message		 = ( char * ) xmalloc ( size ) ;		// Allocate space for the message to be displayed

	if  ( include_timestamp )
		strcpy ( message, timestamp ) ;					// Copy timestamp

	// <warning> and <error> messages generate an additional prefix string
	switch ( message_type )
	   {
		case	MESSAGE_TYPE_WARNING :
			strcat ( message, "*** warning *** " ) ;
			break ;

		case	MESSAGE_TYPE_ERROR :
			strcat ( message, "***  error  *** " ) ;
			break ;
	    }

	// Update pointer to end of message string
	p		 =  message + strlen ( message ) ;

	// Copy remaining arguments to the message
	for  ( i = 0 ; argv [i]  !=  NULL ; i ++ )
	   {
		// There may be several arguments because the caller may have specified a message with unescaped commas.
		// Make sure they are present in the message output
		if  ( i )
			* p ++	=  ',' ;

		strcpy ( p, argv [i] ) ;
		p += strlen ( argv [i] ) ;
	    }

	* p	=  '\0' ;		// The final zero

	// Format the message
	p	=  wake_format_string ( message, wake_line_width, 0 ) ;

	// Error message - abandoned using error() since the last contributor (after version 3.82 and up to version 4.1) has decided weird things about it
	// - and did not consider useful to document them : after all, we all know that our beautiful code has always been self explanatory, isn't it ?
	if  ( message_type  ==  MESSAGE_TYPE_ERROR )
	    {
		fprintf ( stderr, p ) ;
		fprintf ( stderr, "\n" ) ;
		die ( MAKE_FAILURE ) ;
		//OS ( error, NILF, p, NULL ) ; // This one generates a core dump : Once again, thanks for this stupid macro and the change in the error() function
						// which prevents any general usage - must come from a doctorate or a PHD ?
	     }

	// If message type is LOG, and a <LOGFILE> variable is defined, write to the file its specifies.
	if  ( message_type  ==  MESSAGE_TYPE_LOG )
	   {
		struct variable *	vp	=  lookup_variable ( STRING_SIZE_TUPLE ( WAKE_LOGFILE_VARNAME ) ) ;

		if  ( vp  !=  NULL  &&  vp -> value  !=  NULL  &&  * vp -> value )
		   {
			char *	filename	=  wake_trim ( vp -> value ) ;
			FILE *	fp ;


			if  ( filename  ==  NULL  ||  ! * filename )
				filename	=  WAKE_DEFAULT_LOGFILE ;

			if  ( ( fp = fopen ( vp -> value, "a" ) )  ==  NULL )
				OSS ( error, NILF, "Cannot open the file specified by the \"%s\" variable (%s) for logging messages.",
						WAKE_LOGFILE_VARNAME, filename ) ;

			fprintf ( fp, "%s\n", p ) ;
			fclose ( fp ) ;
		    }
	    }

	// Print result
	printf ( "%s\n", p ) ;

	// All done, return
	free ( message ) ;
	free ( p ) ;
	return ( output ) ;
    }


# endif		/*  WAKE_EXTENSIONS  */