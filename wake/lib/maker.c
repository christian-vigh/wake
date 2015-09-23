/**************************************************************************************************************

    NAME
        maker.c

    DESCRIPTION
        Makefile reading functions for the Wake extensions.
	Replaces the original read.c source.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/02]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include		"wake.h"
# include		"parse.h"


/*==============================================================================================================

	Internal variables.

  ==============================================================================================================*/

/* Default directories to search for include files in  */
const char *		default_include_directories [] =
   {
# if defined ( WINDOWS32 )  &&  ! defined  (INCLUDEDIR )
/* 
	This completely up to the user when they install MSVC or other packages.
	 This is defined as a placeholder.  
 */
#	define INCLUDEDIR "."
# endif
	INCLUDEDIR,
# ifndef	_AMIGA
	"/usr/gnu/include",
	"/usr/local/include",
	"/usr/include",
# endif
	0
    } ;

/* Default makefiles */
const char *		default_makefiles [] =
   {
# ifdef		VMS
	/* all lower case since readdir() (the vms version) 'lowercasifies' */
	WAKE_MAKEFILES, "makefile.vms", "gnumakefile.", "makefile."
# elifdef	_AMIGA
	WAKE_MAKEFILES, "GNUmakefile", "Makefile", "SMakefile"
# elifdef	WINDOWS32
	WAKE_MAKEFILES, "GNUmakefile", "makefile", "Makefile", "makefile.mak"
#else
	WAKE_MAKEFILES, "GNUmakefile", "gnumakefile", "makefile", "Makefile"
# endif
	, NULL
    } ;


/* List of directories to search for include files in  */
const char **		include_directories ;

/* The filename and pointer to line number of the makefile currently being read in.  */
const gmk_floc *	reading_file		=  0 ;

/* The chain of files read by read_all_makefiles.  */
struct dep *		read_files		=  0 ;



/*==============================================================================================================

    read_all_makefiles -
        Read in all the makefiles and return a chain of targets to rebuild.

  ==============================================================================================================*/
struct dep *  read_all_makefiles ( const char **  makefiles )
   {
	unsigned int	num_makefiles	=  0 ;


	/* Create *_LIST variables, to hold the makefiles, targets, and variables we will be reading. */
	define_variable_cname ( "MAKEFILE_LIST", "", o_file, 0 ) ;

	DB ( DB_BASIC, ( _( "Reading makefiles...\n" ) ) ) ;

	/* If there's a non-null variable MAKEFILES, its value is a list of files to read first.		  */
	/* But don't let it prevent reading the default makefiles and don't let the default goal come from there. */
	   {
		char *		value;
		char *		name, 
		     *		p ;
		unsigned int	length ;
		int		save	= warn_undefined_variables_flag ;


		/* Turn off --warn-undefined-variables while we expand MAKEFILES.  */
		warn_undefined_variables_flag = 0;

		value = allocated_variable_expand ("$(MAKEFILES)");

		warn_undefined_variables_flag = save;

		/* Set NAME to the start of next token and LENGTH to its length. MAKEFILES is updated for finding remaining tokens.  */
		p	=  value ;

		while ( ( name = find_next_token ( ( const char ** ) & p, & length ) )  !=  0 )
		   {
			if  ( * p  !=  '\0' )
				* p ++  =  '\0' ;

			eval_makefile ( name, RM_NO_DEFAULT_GOAL |RM_INCLUDED | RM_DONTCARE ) ;
		    }

		free ( value ) ;
	    }

	/* Read makefiles specified with -f switches.  */
	if  ( makefiles  !=  0 )
	   {
		while  ( * makefiles  !=  0 )
		   {
			struct dep *	tail	=  read_files ;
			struct dep *	d ;


			if  ( ! eval_makefile ( * makefiles, 0 ) )
				perror_with_name ( "", * makefiles ) ;

			/* Find the first element eval_makefile() added to read_files.  */
			d = read_files ;

			while  ( d -> next  !=  tail )
				d = d -> next ;

			/* Reuse the storage allocated for the read_file.  */
			* makefiles	=  dep_name (d) ;
			num_makefiles ++ ;
			makefiles ++ ;
		    }
	    }

	/* If there were no -f switches, try the default names.  */
	if  ( num_makefiles  ==  0 )
	   {
		const char **	p	=  default_makefiles ;

		while  ( * p  !=  0  &&  ! file_exists_p ( * p ) )
			p ++ ;

		if  ( * p  !=  0 )
		   {
			if  ( ! eval_makefile ( * p, 0 ) )
				perror_with_name ( "", * p ) ;
		    }
		else
		   {
			/* No default makefile was found.  Add the default makefiles to the 'read_files' chain  */
			/* so they will be updated if possible.							*/
			struct dep *	tail	= read_files ;

			/* Add them to the tail, after any MAKEFILES variable makefiles.  */
			while ( tail  !=  0  &&  tail -> next  !=  0 )
				tail = tail -> next ;

			for  ( p = default_makefiles ; * p  !=  0 ; p++ )
			   {
				struct dep *	d	= alloc_dep ( ) ;

				d -> file	=  enter_file ( strcache_add ( * p ) ) ;
				d -> dontcare	=  1 ;

				/* Tell update_goal_chain to bail out as soon as this file is made,	*/
				/* and main not to die if we can't make this file.			*/
				d -> changed	=  RM_DONTCARE ;

				if  ( tail  ==  0 )
					read_files	=  d ;
				else
					tail -> next	=  d ;

				tail	=  d ;
			    }

			if  ( tail  !=  0 )
				tail -> next	=  0 ;
		    }
	     }

	return ( read_files ) ;
    }


/*==============================================================================================================

    parse_var_assignment -
	Check LINE to see if it's a variable assignment or undefine.

	It might use one of the modifiers "export", "override", "private", or it might be one of the conditional 
	tokens like "ifdef", "include", etc.

	If it's not a variable assignment or undefine, VMOD.V_ASSIGN is 0. 
	Returns LINE.

	Returns a pointer to the first non-modifier character, and sets VMOD based on the modifiers found if any, 
	plus V_ASSIGN is 1.

  ==============================================================================================================*/
char *  parse_var_assignment ( const char *  line, variable_modifiers *  vmod )
{
	const char *		p ;

	memset ( vmod, '\0', sizeof ( * vmod ) ) ;

	/* Find the start of the next token.  If there isn't one we're done.  */
	line	=  next_token ( line ) ;

	if  ( * line  ==  '\0' )
		return ( ( char * ) line ) ;

	p	=  line ;

	while ( 1 )
	   {
		int		wlen ;
		const char *	p2 ;
		struct variable v ;

		p2	=  parse_variable_definition ( p, & v ) ;

		/* If this is a variable assignment, we're done.  */
		if  ( p2 )
			break ;

		/* It's not a variable; see if it's a modifier.  */
		p2	=  end_of_token ( p ) ;
		wlen	=  p2 - p ;

		if  ( word1eq ( "export", p, wlen ) )
			vmod -> vm_export	=  1 ;
		else if  ( word1eq ( "override", p, wlen ) )
			vmod -> vm_override	=  1 ;
		else if  ( word1eq ( "private", p, wlen ) )
			vmod -> vm_private	=  1 ;
		else if  ( word1eq ( "define", p, wlen ) )
		   {
			/* We can't have modifiers after 'define' */
			vmod -> vm_define	=  1 ;
			p			=  next_token ( p2 ) ;

			break ;
		    }
		else if  ( word1eq ( "undefine", p, wlen ) )
		   {
			/* We can't have modifiers after 'undefine' */
			vmod -> vm_undefine	=  1 ;
			p			=  next_token ( p2 ) ;

			break ;
		    }
		/* Not a variable or modifier: this is not a variable assignment.  */
		else
			return ( ( char * ) line ) ;

		/* It was a modifier.  Try the next word.  */
		p	=  next_token ( p2 ) ;

		if  ( * p  ==  '\0' )
			return ( ( char * ) line ) ;
	    }

	/* Found a variable assignment or undefine.  */
	vmod -> vm_assign	=  1 ;

	return ( ( char * ) p ) ;
    }


/*==============================================================================================================

    remove_comments -
        Remove comments from LINE.
	Added multiline comment support.

  ==============================================================================================================*/
void  remove_comments ( char *  line )
   {
	char *		comment		=  find_char_unquote ( line, MAP_COMMENT ) ;

	if  ( comment  !=  0 )
	   {
		if  ( strncmp ( comment, MULTILINE_COMMENT_START, sizeof ( MULTILINE_COMMENT_START ) - 1 ) )
			* comment	=  '\0' ;
		else
		   {
			char *		end	=  strstr ( comment, MULTILINE_COMMENT_END ) ;

			assert ( end != NULL ) ;
			strcpy ( comment, end + sizeof ( MULTILINE_COMMENT_END ) ) ;
		    }
	    }
    }


/*==============================================================================================================

    find_char_unquote -
        Search STRING for an unquoted STOPCHAR or blank (if BLANK is nonzero).
	Backslashes quote STOPCHAR, blanks if BLANK is nonzero, and backslash.
	Quoting backslashes are removed from STRING by compacting it into itself.  
	Returns a pointer to the first unquoted STOPCHAR if there is one, or nil if there are none.  
	STOPCHARs inside variable references are ignored if IGNOREVARS is true.

	STOPCHAR _cannot_ be '$' if IGNOREVARS is true.

  ==============================================================================================================*/
char *	find_char_unquote ( char * string, int map )
   {
	unsigned int		string_len	= 0 ;
	char *			p		= string ;

	/* Always stop on NUL.  */
	map	|=  MAP_NUL ;

	while ( 1 )
	   {
		while ( ! STOP_SET ( * p, map ) )
			++p ;

		if ( * p  ==  '\0' )
			break ;

		/* If we stopped due to a variable reference, skip over its contents.  */
		if ( STOP_SET ( * p, MAP_VARIABLE ) )
		   {
			char	openparen	=  p [1] ;

			p += 2 ;

			/* Skip the contents of a non-quoted, multi-char variable ref.  */
			if ( openparen  ==  '(' || openparen  ==  '{' )
			   {
				unsigned int pcount = 1 ;
				char closeparen = ( openparen  ==  '(' ? ')' : '}' ) ;

				while ( * p )
				   {
					if ( * p  ==  openparen )
						++ pcount ;
					else if ( * p  ==  closeparen )
						if ( -- pcount  ==  0 )
						   {
							++ p ;
							break ;
						    }
						++ p ;
				    }
			    }

			/* Skipped the variable reference: look for STOPCHARS again.  */
			continue ;
		    }

		if ( p  >  string  &&  p [-1]  ==  '\\' )
		   {
			/* Search for more backslashes.  */
			int	i	= -2 ;

			while ( & p [i]  >=  string  &&  p [i]  ==  '\\' )
				-- i ;
			++ i ;

			/* Only compute the length if really needed.  */
			if ( string_len  ==  0 )
				string_len = strlen ( string ) ;

			/* The number of backslashes is now -I.	Copy P over itself to swallow half of them.  */
			memmove ( & p [i], & p [i/2], ( string_len - ( p - string ) ) - ( i / 2 ) + 1 ) ;
			p	+=  i / 2 ;

			/* All the backslashes quoted each other ; the STOPCHAR was unquoted.  */
			if  ( i % 2  ==  0 )
				return ( p ) ;

			/* The STOPCHAR was quoted by a backslash.  Look for another.  */
		    }
		else
			/* No backslash in sight.  */
			return ( p ) ;
	    }

	/* Never hit a STOPCHAR or blank ( with BLANK nonzero ).  */
	return ( 0 ) ;
    }


/*==============================================================================================================

    unescape_char -
        Unescape a character in a string.  The string is compressed onto itself.

  ==============================================================================================================*/
char *  unescape_char ( char *  string, int  c )
   {
	char * p	=  string ;
	char * s	=  string ;

	while ( * s  !=  '\0' )
	   {
		if ( * s  ==  '\\' )
		   {
			char *	e  =  s ;
			int	l ;

			/* We found a backslash.  See if it's escaping our character.  */
			while ( * e  ==  '\\' )
				++ e ;

			l	= e - s ;

			/* It's not ; just take it all without unescaping.  */
			if  ( * e  !=  c  ||  l % 2  ==  0 )
			   {
				memmove ( p, s, l ) ;
				p += l ;
			    }
			/* It is, and there's >1 backslash.  Take half of them.  */
			else if ( l > 1 )
			   {
				l /= 2 ;
				memmove ( p, s, l ) ;
				p += l ;
			    }
			s = e ;
		    }

		* ( p ++ ) = * ( s ++ ) ;
	    }

	* p	=  '\0' ;

	return ( string ) ;
    }


/*==============================================================================================================

    find_percent -
        Search PATTERN for an unquoted % and handle quoting.

  ==============================================================================================================*/
char *	find_percent ( char *  pattern )
   {
	return ( find_char_unquote ( pattern, MAP_PERCENT ) ) ;
    }


/*==============================================================================================================

    find_percent_cached -
        Search STRING for an unquoted % and handle quoting.  Returns a pointer to the % or NULL if no % was found.
	This version is used with strings in the string cache: if there's a need to modify the string a new 
	version will be added to the string cache and *STRING will be set to that.

  ==============================================================================================================*/
const char *  find_percent_cached ( const char ** string )
   {
	const char *	p	=  * string ;
	char *		new	=  0 ;
	int		slen	=  0 ;

	/* If the first char is a % return now.  This lets us avoid extra tests	inside the loop.  */
	if ( * p  ==  '%' )
		return ( p ) ;

	while ( 1 )
	   {
		while ( ! STOP_SET ( * p, MAP_PERCENT | MAP_NUL ) )
			++ p ;

		if ( * p  ==  '\0' )
			break ;

		/* See if this % is escaped with a backslash ; if not we're done.  */
		if  ( p [-1] != '\\' )
			break ;

		/* Search for more backslashes.  */
		   {
			char *		pv ;
			int		i	=  -2 ;

			while  ( & p [i]  >=  * string  &&  p [i]  ==  '\\' )
				-- i ;

			++ i ;

			/* At this point we know we'll need to allocate a new string. Make a copy if we haven't yet done so.  */
			if  ( ! new )
			   {
				slen		=  strlen ( * string ) ;
				new		=  alloca ( slen + 1 ) ;

				memcpy ( new, *string, slen + 1 ) ;

				p		=  new + ( p - * string ) ;
				* string	=  new ;
			    }

			/* At this point *string, p, and new all point into the same string.	*/
			/* Get a non-const version of p so we can modify new.			*/
			pv	=  new + ( p - * string ) ;

			/* The number of backslashes is now -I. Copy P over itself to swallow half of them.  */
			memmove ( & pv [i], & pv [i/2], ( slen - ( pv - new ) ) - ( i / 2 ) + 1 ) ;
			p	+=  i / 2 ;

			/* If the backslashes quoted each other ; the % was unquoted.  */
			if  ( i % 2  ==  0 )
				break ;
		    }
	    }

	/* If we had to change STRING, add it to the strcache.  */
	if ( new )
	   {
		* string	=  strcache_add ( * string ) ;
		p		=  * string + ( p - new ) ;
	    }

	/* If we didn't find a %, return NULL.  Otherwise return a ptr to it.  */
	return ( ( * p  ==  '\0' ) ?  NULL : p ) ;
    }


/*==============================================================================================================

    get_next_makeword -
        Parse the next "makefile word" from the input buffer, and return info about it.

	A "makefile word" is one of:

	wt_bogus        Should never happen
	wt_eol          End of input
	wt_static       A static word; cannot be expanded
	wt_variable     A word containing one or more variables/functions
	wt_colon        A colon
	wt_dcolon       A double-colon
	wt_semicolon    A semicolon
	wt_varassign    A variable assignment operator ( =, :=, ::=, +=, ?=, or != )

	Note that this function is only used when reading certain parts of the makefile.  Don't use it where 
	special rules hold sway ( RHS of a variable, in a command list, etc. )

  ==============================================================================================================*/
makewordtype	get_next_makeword  ( char *  buffer, char *  delim, char **  startp, unsigned int *  length )
   {
	makewordtype		wtype	= wt_bogus ;
	char *			p	= buffer, 
	     *			beg  ;
	char c ;

	/* Skip any leading whitespace.  */
	while  ( isblank ( ( unsigned char ) * p ) )
		++ p ;

	beg	=  p ;
	c	=  * ( p ++ ) ;

	switch ( c )
	   {
		case	'\0' :
			wtype	=  wt_eol ;
			break ;

		case	';' :
			wtype	=  wt_semicolon ;
			break ;

		case	'=' :
			wtype	=  wt_varassign ;
			break ;

		case	':' :
			wtype	=  wt_colon ;

			switch ( * p )
			   {
				case	':' :
					++ p ;

					if  ( p [1]  !=  '=' )
						wtype	=  wt_dcolon ;
					else
					   {
						wtype	=  wt_varassign ;
						++ p ;
					    }
					break ;

				case	'=' :
					++ p ;
					wtype	=  wt_varassign ;
					break ;
			    }
			break ;

		case	'+' : 
		case	'?' :
		case	'!' :
			if  ( * p  ==  '=' )
			   {
				++ p ;
				wtype	=  wt_varassign ;
				break ;
			    }

		default :
			if  ( delim  &&  strchr ( delim, c ) )
				wtype	=  wt_static ;
			break ;
	    }

	/* Did we find something?  If so, return now.  */
	if  ( wtype  !=  wt_bogus )
		goto done ;

	/***
		This is some non-operator word.  A word consists of the longest string of characters that doesn't contain 
		whitespace, one of  [:=#], or  [?+!]=, or one of the chars in the DELIM string.  
	 ***/

	/* We start out assuming a static word ; if we see a variable we'll adjust our assumptions then.  */
	wtype	=  wt_static ;

	/* We already found the first value of "c", above.  */
	while ( 1 )
	   {
		char	closeparen ;
		int	count ;

		switch ( c )
		   {
			case	'\0'	:
			case	' '	:
			case	'\t'	:
			case	'='	:
				goto done_word ;

			case	':'	:
# ifdef		HAVE_DOS_PATHS
				/*** 
					A word CAN include a colon in its drive spec.  The drive spec is allowed either at the 
					beginning of a word, or as part	of the archive member name, like in "libfoo.a( d:/foo/bar.o )".  
				 ***/
				if  ( ! ( p - beg  >=  2
					&& ( * p  ==  '/'  ||  *p  ==  '\\' )  &&  isalpha ( ( unsigned char ) p [-2] )
					&& ( p - beg  ==  2  ||  p [-3]  ==  '(' ) ) )
# endif
					goto done_word ;

			case	'$'	:	
				c	= * ( p ++ ) ;

				if  ( c  ==  '$' )
					break ;

				/* This is a variable reference, so note that it's expandable. Then read it to the matching close paren. */
				wtype	=  wt_variable ;

				if  ( c  ==  '(' )
					closeparen	=  ')' ;
				else if ( c  ==  '{' )
					closeparen	=  '}' ;
				else
					/* This is a single-letter variable reference.  */
					break ;

				for  ( count = 0 ; * p  !=  '\0' ; ++ p )
				   {
					if  ( * p  ==  c )
						++ count ;
					else if  ( * p  ==  closeparen  &&  -- count  <  0 )
					   {
						++ p ;
						break ;
					    }
				    }
				break ;

			case	'?' :
			case	'+' :
				if  ( * p  ==  '=' )
					goto done_word ;

				break ;

			case	'\\' :
				switch ( *p )
				   {
					case	':'  :
					case	';'  :
					case	'='  :
					case	'\\' :
						++ p ;
						break ;
				    }
				break ;

			default :
				if  ( delim  &&  strchr ( delim, c ) )
					goto done_word ;

				break ;
		    }

		c	=  * ( p ++ ) ;
	    }

done_word:
	-- p ;

done:
	if  ( startp )
		* startp	=  beg ;

	if  ( length )
		* length	=  p - beg ;

	return ( wtype ) ;
    }



/*==============================================================================================================

    construct_include_path -
        Construct the list of include directories from the arguments and the default list.

  ==============================================================================================================*/
void	construct_include_path ( const char **  arg_dirs )
   {
# ifdef VAXC             /* just don't ask ... */
	stat_t		stbuf ;
# else
	struct stat	stbuf ;
# endif
	const char **	dirs ;
	const char **	cpp ;
	unsigned int	idx ;

	/* Compute the number of pointers we need in the table.  */
	idx	=  sizeof ( default_include_directories ) / sizeof ( const char * ) ;

	if ( arg_dirs )
	   {
		for  ( cpp = arg_dirs ; * cpp  !=  0 ; ++ cpp )
			++ idx ;
	    }

#ifdef  __MSDOS__
	/* Add one for $DJDIR.  */
	++ idx ;
#endif

	dirs	=  xmalloc ( idx * sizeof ( const char * ) ) ;
	idx	=  0 ;

	/* First consider any dirs specified with -I switches. Ignore any that don't exist.  Remember the maximum string length.  */
	if  ( arg_dirs )
	   {
		while  ( * arg_dirs  !=  0 )
		   {
			const char *	dir		=  * ( arg_dirs ++ ) ;
			char *		expanded	=  0 ;
			int		e ;

			if ( dir [0]  ==  '~' )
			   {
				expanded	=  tilde_expand ( dir ) ;

				if ( expanded  !=  0 )
					dir	=  expanded ;
			    }

			EINTRLOOP ( e, stat ( dir, & stbuf ) ) ;

			if  ( e  ==  0  &&  S_ISDIR ( stbuf. st_mode ) )
			   {
				unsigned int	len	=  strlen ( dir ) ;

				/* If dir name is written with trailing slashes, discard them.  */
				while  ( len  >  1  &&  dir [ len - 1 ]  ==  '/' )
					-- len ;

				dirs [idx ++ ]	=  strcache_add_len ( dir, len ) ;
			    }

			free ( expanded ) ;
		    }
	    }

	/* Now add the standard default dirs at the end.  */

# ifdef  __MSDOS__
	   {
		/* The environment variable $DJDIR holds the root of the DJGPP directory tree ; add ${DJDIR}/include.  */
		struct variable *	djdir	=  lookup_variable ( "DJDIR", 5 ) ;

		if ( djdir )
		   {
			unsigned int	len	=  strlen ( djdir -> value ) + 8 ;
			char *		defdir	=  alloca ( len + 1 ) ;

			strcat ( strcpy ( defdir, djdir -> value ), "/include" ) ;
			dirs [idx++]	=  strcache_add ( defdir ) ;
		    }
	    }
# endif

	for  ( cpp = default_include_directories ; * cpp  !=  0 ; ++ cpp )
	   {
		int	e ;

		EINTRLOOP ( e, stat ( * cpp, & stbuf ) ) ;

		if  ( e  ==  0  &&  S_ISDIR ( stbuf. st_mode ) )
		   {
			unsigned int	len	=  strlen ( *cpp ) ;

			/* If dir name is written with trailing slashes, discard them.  */
			while  ( len  >  1  &&  ( * cpp ) [len - 1]  ==  '/' )
				-- len ;

			dirs [idx++]	=  strcache_add_len ( * cpp, len ) ;
		    }
	    }

	dirs [idx] = 0 ;

	/* Now add each dir to the .INCLUDE_DIRS variable.  */
	for ( cpp = dirs ; *cpp != 0 ; ++cpp )
		do_variable_definition ( NILF, ".INCLUDE_DIRS", *cpp,
		o_default, f_append, 0 ) ;

	include_directories = dirs ;
    }

    
/*==============================================================================================================

    tilde_expand -
        Expand ~ or ~USER at the beginning of NAME.
	Return a newly malloc'd string or 0.

  ==============================================================================================================*/
char *	tilde_expand ( const char *  name )
   {
# ifndef	VMS
	if ( name [1]  ==  '/'  ||  name [1]  ==  '\0' )
	   {
		extern char *	getenv (  ) ;
		char *		home_dir ;
		int		is_variable ;
		int		save		=  warn_undefined_variables_flag ;

		/* Turn off --warn-undefined-variables while we expand HOME.  */
		warn_undefined_variables_flag	=  0 ;
		home_dir			=  allocated_variable_expand ( "$(HOME )" ) ;
		warn_undefined_variables_flag   =  save ;

		is_variable			=  ( home_dir [0]  !=  '\0' ) ;

		if  ( ! is_variable )
		   {
			free ( home_dir ) ;
			home_dir = getenv ( "HOME" ) ;
		    }

# if  ! defined ( _AMIGA )  &&  ! defined ( WINDOWS32 )
		if ( home_dir  ==  0  ||  home_dir [0]  ==  '\0' )
		   {
			extern char *		getlogin (  ) ;
			char *			logname		=  getlogin (  ) ;
			
			home_dir	=  0 ;

			if  ( logname  !=  0 )
			   {
				struct passwd *		p	=  getpwnam ( logname ) ;

				if  ( p != 0 )
					home_dir = p -> pw_dir ;
			    }
		    }
# endif /* !AMIGA && !WINDOWS32 */

		if  ( home_dir  !=  0 )
		   {
			char *		new	=  xstrdup ( concat ( 2, home_dir, name + 1 ) ) ;

			if  ( is_variable )
				free ( home_dir ) ;

			return new ;
		    }
	    }

# if  ! defined ( _AMIGA )  &&  ! defined ( WINDOWS32 )
	else
	   {
		struct passwd *		pwent ;
		char *			userend		=  strchr ( name + 1, '/' ) ;

		if  ( userend != 0 )
			* userend	=  '\0' ;

		pwent	=  getpwnam ( name + 1 ) ;

		if  ( pwent  !=  0 )
		   {
			if  ( userend  ==  0 )
				return ( xstrdup ( pwent -> pw_dir ) ) ;
			else
				return ( xstrdup ( concat ( 3, pwent -> pw_dir, "/", userend + 1 ) ) ) ;
		    }
		else if  ( userend  !=  0 )
			* userend	=  '/' ;
	    }
# endif /* !AMIGA && !WINDOWS32 */
# endif /* !VMS */

	return ( 0 ) ;
    }


/*==============================================================================================================

    parse_file_seq -
        Parse a string into a sequence of filenames represented as a chain of struct nameseq's and return that 
	chain.  Optionally expand the strings via glob().

	The string is passed as STRINGP, the address of a string pointer.
	The string pointer is updated to point at the first character not parsed, which either is a null char 
	or equals STOPCHAR.

	SIZE is how big to construct chain elements.
	This is useful if we want them actually to be other structures that have room for additional info.

	PREFIX, if non-null, is added to the beginning of each filename.

	FLAGS allows one or more of the following bitflags to be set:
	PARSEFS_NOSTRIP - Do no strip './'s off the beginning
	PARSEFS_NOAR    - Do not check filenames for archive references
	PARSEFS_NOGLOB  - Do not expand globbing characters
	PARSEFS_EXISTS  - Only return globbed files that actually exist	( cannot also set NOGLOB )
	PARSEFS_NOCACHE - Do not add filenames to the strcache ( caller frees )

  ==============================================================================================================*/
# define	NEWELT( _n )												\
			do												\
			   {												\
				const char *	__n	=  ( _n ) ;							\
															\
				* newp	=	xcalloc ( size ) ;							\
				( * newp ) -> name	= ( cachep ) ?  strcache_add ( __n ) : xstrdup ( __n ) ;	\
				newp	=  & ( * newp ) -> next ;							\
			   } while ( 0 )


void *	parse_file_seq ( char **  stringp, unsigned int  size, int  stopmap, const char *  prefix, int  flags )
   {
	extern void	dir_setup_glob ( glob_t *  glob ) ;

	/* tmp points to tmpbuf after the prefix, if any. tp is the end of the buffer. */
	static char *		tmpbuf		=  NULL ;
	int			cachep		=  NONE_SET ( flags, PARSEFS_NOCACHE ) ;
	struct nameseq *	new		=  0 ;
	struct nameseq **	newp		=  & new ;
	char *			p ;
	glob_t			gl ;
	char *			tp ;

	/* Always stop on NUL.  */
	stopmap		|=  MAP_NUL ;

	if  ( size < sizeof ( struct nameseq ) )
		size = sizeof ( struct nameseq ) ;

	if  ( NONE_SET ( flags, PARSEFS_NOGLOB ) )
		dir_setup_glob ( & gl ) ;

	/* Get enough temporary space to construct the largest possible target.  */
	   {
		static int	tmpbuf_len	=  0 ;
		int		l		=  strlen ( * stringp ) + 1 ;

		if  ( l  >  tmpbuf_len )
		   {
			tmpbuf		=  xrealloc ( tmpbuf, l ) ;
			tmpbuf_len	=  l ;
		    }
	    }

	tp	=  tmpbuf ;

	/* Parse STRING.  P will always point to the end of the parsed content.  */
	p	=  * stringp ;

	while ( 1 )
	   {
		const char *		name ;
		const char **		nlist	=  0 ;
		char *			tildep  =  0 ;
		int			globme  =  1 ;

# ifndef	NO_ARCHIVES
		char *			arname	=  0 ;
		char *			memname =  0 ;
# endif

		char *			s ;
		int			nlen ;
		int			i ;

		/* Skip whitespace ; at the end of the string or STOPCHAR we're done.  */
		p = next_token ( p ) ;

		if  ( STOP_SET ( * p, stopmap ) )
			break ;

		/* There are names left, so find the end of the next name. Throughout this iteration S points to the start.  */
		s	=  p ;
		p	=  find_char_unquote ( p, stopmap | MAP_VMSCOMMA | MAP_BLANK ) ;

# ifdef		VMS
		/* convert comma separated list to space separated */
		if  ( p  &&  * p  ==  ',' )
			* p	=  ' ' ;
# endif

# ifdef		_AMIGA
		if ( p  &&  STOP_SET ( * p, stopmap & MAP_COLON )  &&
				! ( isspace ( ( unsigned char ) p [1] ) || ! p [1]  ||
				isspace ( ( unsigned char ) p [-1] ) ) )
			p = find_char_unquote ( p + 1, stopmap | MAP_VMSCOMMA | MAP_BLANK ) ;
# endif

# ifdef		HAVE_DOS_PATHS
		/*** 
			For DOS paths, skip a "C:\..." or a "C:/..." until we find the first colon which isn't followed by 
			a slash or a backslash.
			Note that tokens separated by spaces should be treated as separate tokens since make doesn't allow 
			path names with spaces 
		 ***/
		if  ( stopmap | MAP_COLON )
		   {
			while  ( p  !=  0  &&  ! isspace ( ( unsigned char ) * p ) &&
				( p [1]  ==  '\\'  ||  p [1]  ==  '/' )  &&  isalpha ( ( unsigned char ) p [-1] ) )
				p = find_char_unquote ( p + 1, stopmap | MAP_VMSCOMMA | MAP_BLANK ) ;
		    }
# endif

		if ( p  ==  0 )
			p = s + strlen ( s ) ;

		/* Strip leading "this directory" references.  */
		if  ( NONE_SET ( flags, PARSEFS_NOSTRIP ) )
		   {
#ifdef VMS
			/* Skip leading ' []'s.  */
			while  ( p - s > 2  &&  s [0]  ==  ' ['  &&  s [1]  ==  ']' )
#else
			/* Skip leading './'s.  */
			while  ( p - s > 2  &&  s [0]  ==  '.'  &&  s [1]  ==  '/' )
#endif
			   {
				/* Skip "./" and all following slashes.  */
				s	+=  2 ;

				while  ( * s  ==  '/' )
					++ s ;
			    }
		    }

		/* Extract the filename just found, and skip it. Set NAME to the string, and NLEN to its length.  */
		if  ( s  ==  p )
		   {
			/* The name was stripped to empty ( "./" ). */
# if	defined ( VMS )
			continue ;
# elif	defined ( _AMIGA )
			/* PDS-- This cannot be right!! */
			tp [0]	=  '\0' ;
			nlen	=  0 ;
# else
			tp [0]	=  '.' ;
			tp [1]	=  '/' ;
			tp [2]	=  '\0' ;
			nlen	=  2 ;
# endif
		    }
		else
		   {
# ifdef VMS
			/*** 
				VMS filenames can have a ':' in them but they have to be '\'ed but we need
				to remove this '\' before we can use the filename.
				xstrdup called because S may be read-only string constant.
			 ***/
			char *	n  =  tp ;

			while ( s < p )
			   {
				if  ( s [0]  ==  '\\'  &&  s [1]  ==  ':' )
					++ s ;

				* ( n++ )	=  * ( s ++ ) ;
			    }

			n [0]	=  '\0' ;
			nlen	=  strlen ( tp ) ;
# else
			nlen		=  p - s ;
			memcpy ( tp, s, nlen ) ;
			tp [nlen]	=  '\0' ;
# endif
		    }

		/* At this point, TP points to the element and NLEN is its length.  */

# ifndef	NO_ARCHIVES
		/*** 
			If this is the start of an archive group that isn't complete, set up to add the archive prefix 
			for future files.  A file list like:

				"libf.a( x.o y.o z.o )" 
				
			needs to be expanded as:

				"libf.a( x.o ) libf.a( y.o ) libf.a( z.o )"

			TP  ==  TMP means we're not already in an archive group.  Ignore something starting with '(', 
			as that cannot actually be an archive-member reference ( and treating it as such results in an 
			empty file name, which causes much lossage ).  Also if it ends in " )" then it's a complete 
			reference so we don't need to treat it specially.

			Finally, note that archive groups must end with ' )' as the last character, so ensure there's 
			some word ending like that before considering this an archive group.  
		 ***/
		if  ( NONE_SET ( flags, PARSEFS_NOAR )  &&
				tp  ==  tmpbuf  &&  tp [0]  !=  '('  &&  tp [nlen-1]  !=  ')' )
		   {
			char *		n	=  strchr ( tp, '(' ) ;

			if ( n )
			   {
				/*** 
					This looks like the first element in an open archive group.
					A valid group MUST have ')' as the last character.  
				 ***/
				const char *	e	=  p ;

				do
				   {
					const char *	o	=  e ;

					e	=  next_token ( e ) ;

					/*** 
						Find the end of this word.  We don't want to unquote and
						we don't care about quoting since we're looking for the	last char in the word. 
					 ***/
					while  ( ! STOP_SET ( * e, stopmap | MAP_BLANK | MAP_VMSCOMMA ) )
						++ e ;

					/* If we didn't move, we're done now. */
					if  ( e  ==  o )
						break ;

					if  ( e [-1]  ==  ')' )
					   {
						/*** 
							Found the end, so this is the first element in an open archive group.  
							It looks like "lib( mem". Reset TP past the open paren.  
						 ***/
						nlen	-=  ( n + 1 ) - tp ;
						tp	 =  n + 1 ;

						/* We can stop looking now.  */
						break ;
					    }
				    } while ( * e  !=  '\0' ) ;

				/* If we have just "lib( ", part of something like "lib(  a b )", go to the next item.  */
				if  ( ! nlen )
					continue ;
			    }
		    }

		/* If we are inside an archive group, make sure it has an end.  */
		if  ( tp > tmpbuf )
		   {
			if ( tp [nlen-1]  ==  ')' )
			   {
				/* This is the natural end ; reset TP.  */
				tp	=  tmpbuf ;

				/* This is just " )", something like "lib( a b  )": skip it.  */
				if ( nlen  ==  1 )
					continue ;
			    }
			else
			   {
				/* Not the end, so add a "fake" end.  */
				tp [nlen++]	=  ')' ;
				tp [nlen]	=  '\0' ;
			    }
		   }
# endif

		/* If we're not globbing we're done: add it to the end of the chain. Go to the next item in the string.  */
		if  ( ANY_SET ( flags, PARSEFS_NOGLOB ) )
		   {
			NEWELT ( concat ( 2, prefix, tmpbuf ) ) ;
			continue ;
		    }

		/*** 
			If we get here we know we're doing glob expansion.
			TP is a string in tmpbuf.  NLEN is no longer used.
			We may need to do more work: after this NAME will be set.  
		 ***/
		name	=  tmpbuf ;

		/* Expand tilde if applicable.  */
		if  ( tmpbuf [0]  ==  '~' )
		   {
			tildep	=  tilde_expand ( tmpbuf ) ;

			if ( tildep != 0 )
				name = tildep ;
		    }

# ifndef	NO_ARCHIVES
		/*** 
			If NAME is an archive member reference replace it with the archive file name, and save the member name 
			in MEMNAME.  We will glob on the archive name and then reattach MEMNAME later.  
		 ***/
		if  ( NONE_SET ( flags, PARSEFS_NOAR )  &&  ar_name ( name ) )
		   {
			ar_parse_name ( name, & arname, & memname ) ;
			name	=  arname ;
		    }
# endif		/* !NO_ARCHIVES */

		/* glob(  ) is expensive: don't call it unless we need to.  */
		if  ( NONE_SET ( flags, PARSEFS_EXISTS )  &&  strpbrk ( name, "?*[" )  ==  NULL )
		   {
			globme	=  0 ;
			i	=  1 ;
			nlist	=  & name ;
		    }
		else
		   {
			switch ( glob ( name, GLOB_NOSORT | GLOB_ALTDIRFUNC, NULL, & gl ) )
			   {
				case	GLOB_NOSPACE :
					OUT_OF_MEM ( ) ;

				/* Success.  */
				case	0 :
					i	=  gl. gl_pathc ;
					nlist	=  ( const char ** ) gl. gl_pathv ;
					break ;

				/* If we want only existing items, skip this one.  */
				case	GLOB_NOMATCH :
					if ( ANY_SET ( flags, PARSEFS_EXISTS ) )
					   {
						i = 0 ;
						break ;
					    }
					/* FALLTHROUGH */

				/* By default keep this name.  */
				default :
					i	=  1 ;
					nlist	=  & name ;
					break ;
			    }
		    }

		/* For each matched element, add it to the list.  */
		while  ( i -- > 0 )
		   {
#ifndef NO_ARCHIVES
			if  ( memname  !=  0 )
			   {
				/* Try to glob on MEMNAME within the archive.  */
				struct nameseq *	found	=  ar_glob ( nlist [i], memname, size ) ;

				/* No matches.  Use MEMNAME as-is.  */
				if  ( ! found )
					NEWELT ( concat ( 5, prefix, nlist [i], "( ", memname, " )" ) ) ;
				/* We got a chain of items.  Attach them.  */
				else
				   {
					if  ( * newp )
						( *newp ) -> next	=  found ;
					else
						* newp			=  found ;

					/* Find and set the new end.  Massage names if necessary.  */
					while  ( 1 )
					   {
						if  ( ! cachep )
							found -> name	=  xstrdup ( concat ( 2, prefix, name ) ) ;
						else if ( prefix )
							found -> name	=  strcache_add ( concat ( 2, prefix, name ) ) ;

						if ( found -> next  ==  0 )
							break ;

						found	=  found -> next ;
					    }

					newp	=  & found -> next ;
				    }
			    }
			else
#endif /* !NO_ARCHIVES */
				NEWELT ( concat ( 2, prefix, nlist [i] ) ) ;
		    }

		if ( globme )
			globfree ( & gl ) ;

#ifndef NO_ARCHIVES
		free ( arname ) ;
#endif

		free ( tildep ) ;
	    }

	* stringp	=  p ;

	return ( new ) ;
    }

# endif		/*  WAKE_EXTENSIONS  */