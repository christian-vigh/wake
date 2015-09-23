/**************************************************************************************************************

    NAME
        conditionals.c

    DESCRIPTION
        Handles conditionals parsing.

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


/* Conditionals (ifdef, etc.) */
conditional_directives		toplevel_conditionals ;
conditional_directives *	conditionals		= & toplevel_conditionals ;



/*==============================================================================================================

    install_conditionals -
        Install a new conditional and return the previous one.

  ==============================================================================================================*/
conditional_directives *	install_conditionals ( conditional_directives *  new )
   {
	conditional_directives *	save	=  conditionals ;

	memset ( new, '\0', sizeof ( * new ) ) ;
	conditionals	=  new ; 

	return ( save ) ;
    }


/*==============================================================================================================

    restore_conditionals -
        Free the current conditionals and reinstate a saved one.

  ==============================================================================================================*/
void  restore_conditionals  ( conditional_directives *  saved )
   {
	/* Free any space allocated by conditional_line.  */
	free ( conditionals -> ignoring ) ;
	free ( conditionals -> seen_else ) ;

	/* Restore state.  */
	conditionals	= saved ;
}


/*==============================================================================================================

    conditional_line -
        Interpret conditional commands "ifdef", "ifndef", "ifeq", "ifneq", "else" and "endif".
	LINE is the input line, with the command as its first word.

	FILENAME and LINENO are the filename and line number in the current makefile.  They are used for error 
	messages.

	Value is -2 if the line is not a conditional at all, -1 if the line is an invalid conditional, 0 if 
	following text should be interpreted, 1 if following text should be ignored.

  ==============================================================================================================*/
# define	chkword(s, t)			if  ( word1eq  ( s, line, len ) ) { cmdtype = (t) ; cmdname = (s) ; }
# define	EXTRATEXT()			OS ( error, flocp, _("extraneous text after '%s' directive"), cmdname )
# define	EXTRACMD()			OS ( fatal, flocp, _("extraneous '%s'"), cmdname )

int	conditional_line ( char *  line, int  len, const gmk_floc *  flocp )
   {
	const char *		cmdname ;
	conditional_type	cmdtype ;
	unsigned int		i ;
	unsigned int		o ;


	/* Make sure this line is a conditional.  */
	chkword ( "ifdef", ct_ifdef )
	else chkword ( "ifndef", ct_ifndef )
	else chkword ( "ifeq", ct_ifeq )
	else chkword ( "ifneq", ct_ifneq )
	else chkword ( "else", ct_else )
	else chkword ( "endif", ct_endif )
	else
		return (-2) ;

	/* Found one: skip past it and any whitespace after it.  */
	line	=  next_token ( line + len ) ;


	/* An 'endif' cannot contain extra text, and reduces the if-depth by 1  */
	if (cmdtype == ct_endif)
	{
		if (*line != '\0')
			EXTRATEXT ();

		if (!conditionals->if_cmds)
			EXTRACMD ();

		--conditionals->if_cmds;

		goto DONE;
	}

	/* An 'else' statement can either be simple, or it can have another conditional after it.  */
	if  ( cmdtype  ==  ct_else )
	   {
		const char *	p ;

		if   ( ! conditionals -> if_cmds )
			EXTRACMD ( ) ;

		o	=  conditionals -> if_cmds - 1 ;

		if  ( conditionals -> seen_else [o] )
			O ( fatal, flocp, _( "only one 'else' per conditional" ) ) ;

		/* Change the state of ignorance.  */
		switch ( conditionals -> ignoring [o] )
		   {
			/* We've just been interpreting.  Never do it again.  */
			case  0 :
				conditionals -> ignoring [o]	= 2 ;
				break ;

			/* We've never interpreted yet.  Maybe this time!  */
			case  1 :
				conditionals -> ignoring [o]	=  0 ;
				break ;
		    }

		/* It's a simple 'else'.  */
		if  ( * line  ==  '\0' )
		   {
			conditionals -> seen_else [o]	=  1 ;
			goto DONE ;
		    }

		/* The 'else' has extra text.  That text must be another conditional and cannot be an 'else' or 'endif'.  */

		/* Find the length of the next word.  */
		for  ( p = line + 1 ; ! STOP_SET ( *p, MAP_SPACE | MAP_NUL) ; ++ p )
			;

		len = p - line ;

		/* If it's 'else' or 'endif' or an illegal conditional, fail.  */
		if  ( word1eq ( "else", line, len ) || word1eq ( "endif", line, len )  ||
				conditional_line ( line, len, flocp )  <  0 )
			EXTRATEXT ( ) ;
		else
		  {
			/* conditional_line() created a new level of conditional. Raise it back to this level.  */
			if  ( conditionals -> ignoring [o]  < 2 )
				conditionals -> ignoring [o]	=  conditionals -> ignoring [o+1] ;

			conditionals -> if_cmds -- ;
		   }

		goto DONE ;
	    }

	if ( conditionals -> allocated  ==  0 )
	   {
		conditionals -> allocated	=  5 ;
		conditionals -> ignoring	=  xmalloc ( conditionals -> allocated ) ;
		conditionals -> seen_else	=  xmalloc ( conditionals -> allocated ) ;
	    }

	o	=  conditionals -> if_cmds ++ ;

	if  ( conditionals -> if_cmds  >  conditionals -> allocated )
	   {
		conditionals -> allocated	+=  5;
		conditionals -> ignoring	 =  xrealloc ( conditionals -> ignoring,
								conditionals -> allocated ) ;
		conditionals -> seen_else	 =  xrealloc ( conditionals -> seen_else,
								conditionals -> allocated ) ;
	    }

	/* Record that we have seen an 'if...' but no 'else' so far.  */
	conditionals -> seen_else [o]	=  0 ;

	/* Search through the stack to see if we're already ignoring.  */
	for  ( i = 0 ; i < o ; i++ )
	    {
		if  ( conditionals -> ignoring [i] )
		   {
			/* We are already ignoring, so just push a level to match the next "else" or "endif",	*/
			/* and keep ignoring.  We don't want to expand variables in the condition.		*/
			conditionals -> ignoring [o]	=  1 ;
			return (1) ;
		    }

		if  ( cmdtype  ==  ct_ifdef  ||  cmdtype  ==  ct_ifndef )
		   {
			char *			var ;
			struct variable *	v ;
			char *			p ;

			/* Expand the thing we're looking up, so we can use indirect and constructed variable names.  */
			var	=  allocated_variable_expand ( line ) ;

			/* Make sure there's only one variable name to test.  */
			p	=  end_of_token ( var ) ;
			i	=  p - var ;
			p	=  next_token ( p ) ;

			if  ( * p  !=  '\0' )
				return (-1) ;

			var [i]	=  '\0';
			v	=  lookup_variable ( var, i ) ;

			conditionals -> ignoring [o]	= ( ( ( v  != 0  &&  * v -> value  !=  '\0' )  ==  ( cmdtype  ==  ct_ifndef ) ) ) ;

			free ( var ) ;
		     }
		/* "ifeq" or "ifneq".  */
		else
		    {
			char		* s1, * s2 ;
			unsigned int	l ;
			char		termin		=  ( * line  ==  '(' ) ?  ',' : * line ;

			if  ( termin  !=  ','  &&  termin  !=  '"'  &&  termin  !=  '\'' )
				return ( -1 ) ;

			s1	=  ++ line ;

			/* Find the end of the first string.  */
			if  ( termin  ==  ',' )
			   {
				int	count	=  0 ;

				for  ( ; * line != '\0' ; ++ line )
				   {
					if  ( * line  ==  '(' )
						++ count ;
					else if  ( * line  ==  ')' )
						-- count ;
					else if  ( *line  ==  ','  &&  count  <=  0 )
						break ;
				    }
			    }
			else
			   {
				while  ( * line  !=  '\0'  &&  * line  !=  termin )
					++ line ;
			    }

			if  ( * line  ==  '\0' )
				return ( -1 ) ;

			if  ( termin  ==  ',' )
			   {
				/* Strip blanks after the first string.  */
				char *		p	=  line ++ ;

				while  ( isblank ( ( unsigned char ) p [-1] ) )
					-- p ;

				* p	=  '\0' ;
			    }
			else
				* line ++	=  '\0' ;

			s2	=  variable_expand ( s1 ) ;

			/* We must allocate a new copy of the expanded string because variable_expand re-uses the same buffer.  */
			l	=  strlen ( s2 );
			s1	=  alloca ( l + 1 ) ;

			memcpy ( s1, s2, l + 1 ) ;

			/* Find the start of the second string.  */
			if  ( termin  !=  ',' )
				line	=  next_token ( line ) ;

			termin		=   ( termin  ==  ',' ) ?  ')' : * line ;

			if  ( termin  !=  ')'  &&  termin  !=  '"'  &&  termin  !=  '\'' )
				return ( -1 ) ;

			/* Find the end of the second string.  */
			if  ( termin  ==  ')' )
			   {
				int	count	=  0 ;

				s2	=  next_token ( line ) ;

				for  ( line = s2; * line  !=  '\0'; ++ line )
				   {
					if  ( * line  ==  '(' )
						++ count ;
					else if  ( * line  ==  ')' )
					   {
						if  ( count  <=  0 )
							break ;
						else
							-- count ;
					    }
				     }
			    }
			else
			   {
				++ line ;
				s2	=  line ;

				while  ( * line  !=  '\0'  &&  * line  !=  termin )
					++ line ;
			    }

			if  ( * line  ==  '\0' )
				return ( -1 );

			* line	=  '\0' ;
			line	=  next_token ( ++ line ) ;

			if  ( * line  !=  '\0' )
				EXTRATEXT ( ) ;

			s2				=  variable_expand ( s2 ) ;
			conditionals -> ignoring [o]	=  ( streq ( s1, s2 )  ==  ( cmdtype  ==  ct_ifneq ) ) ;
		    }
	    }

DONE:
	/* Search through the stack to see if we're ignoring.  */
	for  ( i = 0 ; i  <  conditionals -> if_cmds ; ++ i )
	   {
		if  ( conditionals -> ignoring [i] )
			return ( 1 ) ;
	    }

	return ( 0 ) ;
    }


# endif		/*  WAKE_EXTENSIONS  */