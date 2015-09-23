/**************************************************************************************************************

    NAME
        reader.c

    DESCRIPTION
        Makefile reading functions for the Wake extensions.

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

        Support functions.

  ==============================================================================================================*/


// adjust_eol_after_backslash -
//	Since wake accepts spaces after a final backslash (continuation line character), this function ensures
//	that extra spaces will be removed.
static char *  adjust_eol_after_backslash ( char *  bp )
   {
	* ( bp + 1 )	=  '\n' ;
	* ( bp + 2 )	=  '\0' ;

	return(  bp + 2 ) ;
    }


// find_continuation_backslash -
//	Returns the position of the last backslash in the line, or null if none exists.
//	Spaces are allowed after a continuation backslash, because I'm fed up by all the time I've spent in my career
//	trying to find why my continuation line was not taken as a continuation line.
static char *	find_continuation_backslash ( char *  start, char *  endp )
   {
	int		backslash_count		=  0 ;
	char *		p ;

	// Ignore trailing spaces
	while  ( endp  >=  start  &&  isspace ( ( int ) * endp ) )
		endp -- ;

	// Then count the number of backslashes at the end of the line
	p	=  endp ;

	while  ( p  >=  start  &&  * p  ==  '\\' )
		backslash_count ++, p -- ;

	// If an odd number of backslashes has been found, then this means that the last one is a continuation backslash
	if  ( backslash_count & 1 )
		return ( p + 1 ) ;
	// Otherwise, we met (or not) a series of "\\" 
	else
		return ( NULL ) ;
    }


// is_heredoc_start -
//	Checks if the specified line contains a here document.
static int	is_heredoc_start ( char *  startp, char *  endp, char **  keyword, int *  heredoc_startp, int * heredoc_contents_start )
   {
	char *		kws		=  NULL,
	     *		kwe		=  NULL ;
	char *		p ;
	char *		cmtp ;
	int		index		=  endp - startp ;


	// A line starting with tab/space or having a comment cannot have a heredoc construct
	if  ( isspace ( ( int ) * startp ) ) 
		return ( 0 ) ;

	// We will have to check if there is a comment before the heredoc start construct ("<<")
	cmtp	=  strchr ( startp, '#' ) ;

	/***
		Normally, at that point, we should have retained only two kind of items :
		- A variable definition
		- A rule header
		The distinction will be made in the eval() function.
	 ***/

	// Ignore spaces at the end of line
	while  ( endp  >=  startp  &&  isspace ( ( int ) * endp ) )
		endp --, index -- ;

	kwe	=  endp ;

	// Loop backward heredoc keyword characters
	while  ( IS_HEREDOC_KEYWORD_CHAR ( * endp ) )
	   {
		kws	=  endp -- ;
		index -- ;
	    }

	p	=  endp ;

	// Ignore spaces between the "<<" string and the keyword following
	while  ( p  >=  startp  &&  isspace ( ( int ) * p ) )
		p --, index -- ;

	// Then loop backward for heredoc characters
	while  ( p  >=  startp  &&  IS_HEREDOC_CHAR ( * p ) )
	   {
		p --, index -- ;
	    }

	// Did we find the heredoc construct ?
	p ++ ;

	if  ( strncmp ( HEREDOC_PREFIX, p, sizeof ( HEREDOC_PREFIX ) - 1 ) )
		return ( 0 ) ;

	// Check if the heredoc construct is not AFTER a comment
	if  ( cmtp  !=  NULL  &&  cmtp  <  p )
		return ( 0 ) ;

	p ++ ;
	* heredoc_startp		=  index ;

	// Save the keyword
	if  ( kws  ==  kwe )
	   {
		* keyword			=  xstrdup ( "END" ) ;
		* heredoc_contents_start	=  kwe - startp + 1 ;

		return ( 0 ) ;
	    }
	else
	   {
		int	length			=  kwe - kws + 1 ;

		* keyword			=  xmalloc ( length + 1 ) ;
		strncpy ( * keyword, kws, length ) ;
		( * keyword ) [ length ]	=  '\0' ;
		* heredoc_contents_start	=  kwe - startp + 1 ;

		return ( 1 ) ;
	    }
    }


/*==============================================================================================================

    readline -
        Readline is responsible for reading a "logical line" from a makefile. A logical line is either :
	. A single line
	. A sequence of lines terminated by a backslash character
	. (Wake extensions) A here-document, like in the unix shell :

		rule	:  <<END
			...
		END
	. (Wake extensions) Additionnally, multiline comments are supported using the '#/ *' and '* /#' delimiters.

  ==============================================================================================================*/
long	readline  ( readbuffer *  ebuf, heredocdata **  ehdata )
   {
	static heredocdata	heredoc			=  { 0, 0, 0, NULL, NULL } ;
	char *			p,
	     *			ep,
	     *			ip ;
	char *			end ;
	char *			start ;
	long			nlines			=  0 ;
	int			multiline_type		=  MULTILINE_NONE ;
	int			is_heredoc		=  0 ;
	int			keyword_length ;


	/* The behaviors between string and stream buffers are different enough to warrant different functions. Do the Right Thing. */
	if  ( ! ebuf -> fp )
		return ( readstring ( ebuf ) ) ;

	/* Initialization dedicated to here-documents */
	if  ( heredoc. keyword  !=  NULL )
		free ( heredoc. keyword ) ;

	if  ( heredoc. line  !=  NULL )
		free ( heredoc. line ) ;

	memset ( & heredoc, 0, sizeof ( heredoc ) ) ;

	if  ( ehdata  !=  NULL )
		* ehdata	=  NULL ;

	/* When reading from a file, we always start over at the beginning of the buffer for each new line.  */
	p	=  
	start	=  ebuf -> bufstart ;
	end	=  p + ebuf -> size ;
	* p	=  '\0' ;

	while ( fgets ( p, end - p, ebuf -> fp )  !=  0 )
	   {
		unsigned long		len	=  strlen ( p ) ;

		/* We need the start of the current line for multiline constructs */
		ip	=  p ;

		/*** 
			This only happens when the first thing on the line is a '\0'.
			It is a pretty hopeless case, but ( wonder of wonders ) Athena lossage strikes again!  
			( xmkmf puts NULs in its makefiles. )
			There is nothing really to be done ; we synthesize a newline so	the following line doesn't appear 
			to be part of this line.  
		 ***/
		if ( len  ==  0 )
		   {
			O ( error, & ebuf -> floc, _( "warning: NUL character seen ; rest of line ignored" ) ) ;
			p [0]	=  '\n' ;
			len	=  1 ;
		    }

		/* Jump past the text we just read.  */
		p	+=  len ;

		/*** 
			If the last char isn't a newline, the whole line didn't fit into the buffer.  
			Get some more buffer and try again.  
		 ***/
		if  ( p [-1]  !=  '\n'  &&  ! feof ( ebuf -> fp ) )
		   {
			unsigned int		offset	=  p - start ;

			readbuffer_grow ( ebuf, READBUFFER_INCREMENT ) ;

			start		=  ebuf -> buffer ;
			p		=  ebuf -> buffer + offset ;
			end		=  start + ebuf -> size ;
			* p		=  '\0' ;

			continue ;
		    }

		/* We got a newline, so add one to the count of lines.  */
		++ nlines ;

# if ! defined ( WINDOWS32 )  &&  ! defined ( __MSDOS__ )  &&  ! defined ( __EMX__ )
		/* Check to see if the line was really ended with CRLF ; if so ignore the CR.  */
		if  ( ( p - start )  >  1  &&  p [-1]  ==  '\n'  &&  p [-2]  ==  '\r' )
		   {
			-- p ;
			memmove ( p - 1, p, strlen ( p ) + 1 ) ;
		    }
# endif

		/* Check for multiline constructs */
		if  ( multiline_type  ==  MULTILINE_NONE )
		   {
			/* Continuation line ending with backslash */
			ep	=  find_continuation_backslash ( start, p - 1 ) ;

			if  ( ep  !=  NULL )
			   {
				multiline_type	=  MULTILINE_BACKSLASH ;
				p		=  adjust_eol_after_backslash ( ep ) ;
				continue ;
			    }

			// Multiline comment start 
			ep	=  strchr ( start, * MULTILINE_COMMENT_START ) ;

			if  ( ep  !=  NULL  &&  ! strncmp ( ep, MULTILINE_COMMENT_START, sizeof ( MULTILINE_COMMENT_START ) - 1 ) )
			   {
				// Take care : multiline comments can be placed on only one line
				char *		ep2		=  strstr ( start, MULTILINE_COMMENT_END ) ;

				if  ( ep2  ==  NULL )
				   {
					   multiline_type	=  MULTILINE_COMMENT ;
					   continue ;
				    }
			    }

			// Here document 
			if  ( is_heredoc_start ( start, p - 1, & heredoc. keyword, & heredoc. heredoc_start, & heredoc. start ) )
			   {
				is_heredoc		=  1 ;
				multiline_type		=  MULTILINE_HEREDOC ;
				keyword_length		=  strlen ( heredoc. keyword ) ;
			    }
		    }
		/* Continuation line after a backslash : check if it also ends with a backslash */
		else if  ( multiline_type  ==  MULTILINE_BACKSLASH )
		   {
			ep	=  find_continuation_backslash ( start, p - 1 ) ;

			if  ( ep  ==  NULL )
				multiline_type	=  MULTILINE_NONE ;
			else
				p		=  adjust_eol_after_backslash ( ep ) ;
		    }
		/* End of multiline comment ? */
		else if  ( multiline_type  ==  MULTILINE_COMMENT )
		   {
			ep		=  strstr ( ip, MULTILINE_COMMENT_END ) ;

			if  ( ep  !=  NULL )
				multiline_type	=  MULTILINE_NONE ;
		    }
		/* End of heredoc ? */
		else if  ( multiline_type  ==  MULTILINE_HEREDOC )
		   {
			if  ( ! strncmp ( ip, heredoc. keyword, keyword_length ) )
			   {
				char *	after	=  ip + keyword_length ;

				if  ( after  >=  p  ||  * after  ==  '\r'  ||  * after  ==  '\n' )
				   {
					multiline_type		=  MULTILINE_NONE ;

					if  ( ehdata  !=  NULL )
					   {
						heredoc. end		=  ip - start ;
						heredoc. line		=  xstrdup ( start ) ;
						* ehdata	=  & heredoc ;
					    }
				    }
			    }
		    }

		/* We're over when there is no construct that stops a continuation line construct */
		if  ( multiline_type  ==  MULTILINE_NONE  &&  p [-1]  ==  '\n' )
		   {
			p [-1]	=  '\0' ;
			break ;
		    }
	    }

	/* Error case : input error */
	if ( ferror ( ebuf -> fp ) )
		pfatal_with_name ( ebuf -> floc. filenm ) ;

	/* Error case : unterminated multiline comment */
	if  ( multiline_type  ==  MULTILINE_COMMENT )
		O ( fatal, & ebuf -> floc, _( "error: unterminated multiline comment" ) ) ;
	/* Other case : unterminated heredoc */
	else if  ( multiline_type  ==  MULTILINE_HEREDOC )
		O ( fatal, & ebuf -> floc, _( "error: unterminated here document" ) ) ;

	/* If a heredoc has been found, duplicate the line */
	if  ( is_heredoc )
		heredoc. line	=  xstrdup ( ebuf -> buffer ) ;

	/*** 
		If we found some lines, return how many.
		If we didn't, but we did find _something_, that indicates we read the last line of a file with no final newline ; return 1.
		If we read nothing, we're at EOF ; return -1.  
	 ***/
	return ( ( nlines ) ?  nlines : ( ( p  ==  ebuf -> bufstart ) ?  -1 : 1 ) );
    }


/*==============================================================================================================

    readstring -
        Find the next line of text in an eval buffer, combining continuation lines into one line.
	Return the number of actual lines read ( > 1 if continuation lines ).
	Returns -1 if there's nothing left in the buffer.

	After this function, ebuf -> buffer points to the first character of the line we just found.

	Read a line of text from a STRING. Since we aren't really reading from a file, don't bother with 
	line numbers.

  ==============================================================================================================*/
unsigned long	readstring  ( readbuffer *  ebuf )
   {
	char *		eol ;

	/* If there is nothing left in this buffer, return 0.  */
	if  ( ebuf -> bufnext  >=  ebuf -> bufstart + ebuf -> size )
		return ( -1 ) ;

	/*** 
		Set up a new starting point for the buffer, and find the end of the next logical line 
		( taking into account backslash/newline pairs ).  
	 ***/
	eol		= 
	ebuf -> buffer	=  ebuf -> bufnext ;

	while ( 1 )
	   {
		int		backslash	=  0 ;
		const char *	bol		=  eol ;
		const char *	p ;

		/* Find the next newline.  At EOS, stop.  */
		p = eol = strchr ( eol , '\n' ) ;

		if ( ! eol )
		   {
			ebuf -> bufnext = ebuf -> bufstart + ebuf -> size + 1 ;
			return ( 0 ) ;
		    }

		/* Found a newline ; if it's escaped continue ; else we're done.  */
		while  ( p  >  bol  &&  * ( -- p )  ==  '\\' )
			backslash	=  ! backslash ;

		if  ( ! backslash )
			break ;

		++ eol ;
	    }

	/* Overwrite the newline char.  */
	* eol		=  '\0' ;
	ebuf -> bufnext =  eol + 1 ;

	return ( 0 ) ;
    }


/*==============================================================================================================

    readbuffer_alloc, readbuffer_grow  -
        Allocates data in a readbuffer.

  ==============================================================================================================*/
void	readbuffer_alloc ( readbuffer *  ebuf, int  size )
   {
	ebuf -> buffer		=
	ebuf -> bufstart	= 
	ebuf -> bufnext		=  xmalloc ( size ) ;
	ebuf -> size		=  size ;
    }


void	readbuffer_grow ( readbuffer *  ebuf, int  increment )
   {
	int	size		=  ebuf -> size + increment ;

	ebuf -> buffer		= 
	ebuf -> bufstart	=  xrealloc ( ebuf -> buffer, size ) ;
	ebuf -> size		=  size ;
    }



/*==============================================================================================================

    transform_heredoc -
        Replaces a heredoc with a set of continuation lines terminated by a backslash.

  ==============================================================================================================*/
char *	transform_heredoc ( char *  line, heredocdata *  ehdata, int  include_backslash ) 
   {
	int		line_length,
			line_count,
			final_length ;
	char *		p ;
	char *		q ;
	char *		output ;
	int		index		=  0 ;


	// Do nothing if no heredoc information is available
	if  ( ehdata  ==  NULL )
		return ( line ) ;

	// Compute the total length needed, counting current line length minus the number of characters needed
	// for the heredoc syntax, plus the number of backslashes to be added per line
	line_length	=  strlen ( line )  ;
	final_length	=  line_length ;

	p		=  line ;
	line_count	=  0 ;

	for  ( p = strchr ( p, '\n' ) ; p  !=  NULL ; p = strchr ( p + 1, '\n' ) )
		final_length ++, line_count ++ ;

	final_length ++ ;

	if  ( include_backslash )
		final_length	+=  line_count ;

	// Reallocate the string if computed size is greater than the original size 
	if  ( final_length  >  line_length )
		output	=  xrealloc ( line, final_length + 1 ) ;
	else
		output	=  line ;

	// Process characters from the input line
	p		=  line ;
	q		=  output ;

	while  ( * p  &&  index <  ehdata -> end )
	   {
		// Copy all characters before the start of the heredoc + characters between the end of the heredoc
		// and the start of the ending keyword
		if  ( index  <  ehdata -> heredoc_start  || ( index  >=  ehdata -> start  &&  index  <  ehdata -> end ) )
		   {
			if  ( * p  ==  '\n'  &&  include_backslash )
			   {
				* q ++	=  '\\' ;
				* q ++	=  '\n' ;
			    }
			else
				* q ++	=  * p ;
		    }
		// Special case for the start of the heredoc ; substitute it with a newline preceded by a backslash, if needed
		else if  ( index  ==  ehdata -> start ) 
		   {
			if  ( include_backslash )
				* q ++	=  '\\' ;

			* q ++	=  '\n' ;
		    }

		p ++, index ++ ;
	    }

	* q	=  '\0' ;

	return ( output ) ;
    }


/*==============================================================================================================

    replace_heredoc -
        Replaces a character by another within the contents of a heredoc.
	This function must be called before collapse_continuations() to replace comment characters (#) by an
	unused one, then called again after collapse_continuations() to restore it.

  ==============================================================================================================*/
void	replace_heredoc ( char *  text, heredocdata *  heredoc, char  oldch, char  newch )
   {
	char *		p	=  text ;

	while  ( * p )
	   {
		if  ( * p  ==  oldch )
			* p	=  newch ;

		p ++ ;
	    }
    }


/*==============================================================================================================

    format_commands -
        Removes the first tab from a group of commands.

  ==============================================================================================================*/
void  format_commands ( char *  text ) 
   {
	char *	p		=  text,
	     *  q		=  text ;


	// Keep the first tab, since other make parts cannot cope with commands that do not start with it
	if  ( * p  == '\t' )
		p ++, q ++ ;

	while  ( * p )
	   {
		if  ( * ( p - 1 )  ==  '\n'  &&  * p  ==  cmd_prefix )
			p ++ ;
		else
			* q ++	=  * p ++ ;
	    }

	* q = '\0' ;
    }

# endif		/*  __WAKE_EXTENSIONS  */