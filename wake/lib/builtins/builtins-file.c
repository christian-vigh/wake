/**************************************************************************************************************

    NAME
        builtins-file.c

    DESCRIPTION
        Builtin functions related to files.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/06]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"wake.h"


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_file_tempfile - Creates a temporary file
 *
 *   PROTOTYPE
 *	$(<tempfile> [ directory [, prefix [, suffix] )
 *
 *   DESCRIPTION
 *	Expands to a new, unique temporary file name.
 *
 *   NOTES
 *	Since this function generates a new filename upon each invocation, a make variable must be defined using
 *	the ":=" assignement operation instead of "=" if you want the same filename to be kept :
 *
 *		TMPFILE		:=  (<tempfile>)
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_file_tempfile )
   {
	int		argc			=  wake_function_argc ( argv ) ;
	char *		directory		=  NULL ;
	char *		prefix			=  NULL ;
	char *		suffix			=  NULL ;
	char *		result ;

	
	// Get optional arguments
	if  ( argc  >  0 )
	   {
		directory	=  wake_trim ( argv [0] ) ;

		if  ( ! * directory )
			directory	=  NULL ;

		if  ( argc  >  1 )
		  {
			prefix		=  wake_trim ( argv [1] ) ;

			if  ( ! * prefix )
				prefix	=  NULL ;

			if  ( argc  >  2 )
			   {
				suffix	=  wake_trim ( argv [2] ) ;

				if  ( ! * suffix )
					suffix	=  NULL ;
			    }
		   }
	    }

	// Generate temp/unique filename
	result	=  tempnam ( directory, prefix ) ;

	// Copy the result to the output
	output	=  variable_buffer_output ( output, result, strlen ( result ) ) ;

	if  ( suffix  !=  NULL )	// don't forget optional suffix
		output	=  variable_buffer_output ( output, suffix, strlen ( suffix ) ) ;

	// Free allocated pointers
	free ( result ) ;

	if  ( directory  !=  NULL )
		free ( directory ) ;

	if  ( prefix  !=  NULL ) 
		free ( prefix ) ;

	if  ( suffix  !=  NULL )
		free ( suffix ) ;

	// All done, return
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_file_filecontents - Reads a file contents
 *
 *   PROTOTYPE
 *	$(<filecontents> filename )
 *
 *   DESCRIPTION
 *	Expands to the contents of the specified file.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_file_filecontents )
   {
	char *		filename	=  argv [0] ;
	struct stat	st ;


	if  ( access ( filename, F_OK | R_OK )  ==  -1 )
		fatal ( reading_file, 0, "Cannot access file '%s' for reading", filename ) ;

	stat ( filename, & st ) ;

	if  ( st. st_size )
	   {
		char *		buffer		=  xmalloc ( st. st_size ) ;
		FILE *		fp		=  fopen ( filename, "r" ) ;
	
		fread ( buffer, st. st_size, 1, fp ) ;
		fclose ( fp ) ;

		output		=  variable_buffer_output ( output, buffer, st. st_size ) ;
	    }

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_file_stat - Entry point for stat() queries.
 *
 *   PROTOTYPE
 *	$(<stat.mode>	filename)
 *	$(<stat.ctime>	filename)
 *	$(<stat.mtime>	filename)
 *	$(<stat.atime>	filename)
 *	$(<stat.size>	filename)
 *	$(<stat.uid>	filename)
 *	$(<stat.gid>	filename)
 *
 *   DESCRIPTION
 *	Returns information about a file.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_file_stat )
   {
	char *		filename	=  argv [0] ;
	char		buffer [64] ;
	struct stat	st ;
	long		value		=  -1 ;
	int		length ;


	if  ( stat ( filename, & st )  !=  0 )
		fatal ( reading_file, 0, "Cannot stat file '%s'", filename ) ;

	if  ( ! strcmp ( funcname, "<stat.mode>" ) )
		value	=  ( long ) st. st_mode ;
	else if  ( ! strcmp ( funcname, "<stat.ctime>" ) )
		value	=  ( long ) st. st_ctime ;
	else if  ( ! strcmp ( funcname, "<stat.mtime>" ) )
		value	=  ( long ) st. st_mtime ;
	else if  ( ! strcmp ( funcname, "<stat.atime>" ) )
		value	=  ( long ) st. st_atime ;
	else if  ( ! strcmp ( funcname, "<stat.size>" ) )
		value	=  ( long ) st. st_size ;
	else if  ( ! strcmp ( funcname, "<stat.uid>" ) )
		value	=  ( long ) st. st_uid ;
	else if  ( ! strcmp ( funcname, "<stat.gid>" ) )
		value	=  ( long ) st. st_gid ;

	length		=  sprintf ( buffer, "%ld", value ) ;
	output		=  variable_buffer_output ( output, buffer, length ) ;

	return ( output ) ;
    }

# endif		/*  __WAKE_EXTENSIONS  */