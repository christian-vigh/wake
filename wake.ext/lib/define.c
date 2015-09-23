/**************************************************************************************************************

    NAME
        define.c

    DESCRIPTION
        Define/undefine helper functions during parsing.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/07]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include		"wake.h"
# include		"parse.h"


/*==============================================================================================================

    do_undefine -
        Execute a 'undefine' directive.
	The undefine line has already been read, and NAME is the name of the variable to be undefined.

  ==============================================================================================================*/
void	do_undefine  ( char *  name, enum variable_origin  origin, readbuffer *  ebuf )
   {
	char * p, * var ;

	/* Expand the variable name and find the beginning (NAME) and end. */
	var	=	allocated_variable_expand ( name ) ;
	name	=	next_token ( var ) ;

	if  ( * name  ==  '\0' )
		O ( fatal, & ebuf -> floc, _( "empty variable name" ) ) ;

	p	=  name + strlen ( name ) - 1 ;

	while  ( p  >  name  &&  isblank ( ( unsigned char ) * p ) )
		p -- ;

	p [1]	=  '\0' ;

	undefine_variable_global ( name, p - name + 1, origin ) ;
	free ( var ) ;
    }  


/*==============================================================================================================

    do_define -
        Execute a 'define' directive.
	The first line has already been read, and NAME is the name of the variable to be defined.  
	The following lines remain to be read.

  ==============================================================================================================*/
struct variable *  do_define  ( char *  name, enum variable_origin  origin, readbuffer *  ebuf )
   {
	struct variable *	v ;
	struct variable		var ;
	gmk_floc		defstart ;
	int			nlevels			=  1 ;
	unsigned int		length			=  100 ;
	char *			definition		=  xmalloc ( length ) ;
	unsigned int		idx			=  0 ;
	char			* p, 
				* n ;


	defstart	=  ebuf -> floc ;
	p		=  parse_variable_definition ( name, & var ) ;

	/* No assignment token, so assume recursive.  */
	if  ( p  ==  NULL )
		var. flavor	=  f_recursive ;
	else
	   {
		if  ( var. value [0]  !=  '\0' )
			O (error, & defstart, _( "extraneous text after 'define' directive" ) ) ;

		/* Chop the string before the assignment token to get the name. */
		var. name [ var. length ]	=  '\0' ;
	    }

	/* Expand the variable name and find the beginning (NAME) and end.  */
	n	=  allocated_variable_expand ( name ) ;
	name	=  next_token ( n ) ;

	if  ( name [0]  ==  '\0' )
		O ( fatal, & defstart, _( "empty variable name" ) ) ;

	p	=  name + strlen ( name ) - 1 ;

	while  ( p  >  name  &&  isblank ( ( unsigned char ) * p ) )
		p -- ;

	p[1] = '\0';

	/* Now read the value of the variable.  */
	while ( 1 )
	   {
		unsigned int	len ;
		char *		line ;
		long		nlines	=  readline ( ebuf, NULL ) ;

		/* If there is nothing left to be eval'd, there's no 'endef'!! */
		if  ( nlines  <  0 )
			O ( fatal, & defstart, _( "missing 'endef', unterminated 'define'" ) ) ;

		ebuf -> floc. lineno	+=  nlines ;
		line			 =  ebuf -> buffer ;

		collapse_continuations ( line ) ;

		/* If the line doesn't begin with a tab, test to see if it introduces
		another define, or ends one.  Stop if we find an 'endef' */
		if  ( line [0]  !=  cmd_prefix )
		   {
			p	=  next_token ( line ) ;
			len	=  strlen ( p ) ;

			/* If this is another 'define', increment the level count.  */
			if  ( ( len  ==  6  ||  ( len  >  6  &&  isblank ( ( unsigned char ) p [6] ) ) )  &&
					strneq  ( p, "define", 6 ) )
				nlevels ++ ;

			/* If this is an 'endef', decrement the count.  If it's now 0, we've found the last one.  */
			else if  ( ( len  ==  5  ||  ( len  >  5  &&  isblank ( ( unsigned char ) p [5] ) ) )  &&
					strneq ( p, "endef", 5 ) )
			   {
				p	+=  5 ;

				remove_comments ( p ) ;

				if  ( * ( next_token ( p ) )  !=  '\0' )
					O ( error, & ebuf -> floc, _( "extraneous text after 'endef' directive" ) ) ;

				if  ( -- nlevels  ==  0 )
					break ;
			    }
		    }

		/* Add this line to the variable definition.  */
		len	=  strlen ( line ) ;

		if  ( idx + len + 1  >  length )
		   {
			length		=  ( idx + len ) * 2 ;
			definition	=  xrealloc ( definition, length + 1 ) ;
		    }

		memcpy ( & definition [idx], line, len ) ;
		idx	+=  len ;

		/* Separate lines with a newline.  */
		definition [ idx ++ ]	=  '\n' ;
	}

	/* We've got what we need; define the variable.  */
	if  ( idx  ==  0 ) 
		definition [0]		=  '\0' ;
	else
		definition [ idx - 1 ]	=  '\0' ;

	v	=  do_variable_definition ( & defstart, name, definition, origin, var. flavor, 0 ) ;

	free ( definition ) ;
	free ( n ) ;

	return ( v ) ;
    }

# endif		/*  WAKE_EXTENSIONS  */