/**************************************************************************************************************

    NAME
        eval.c

    DESCRIPTION
        Makefile evaluator function.

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

    Macros.

  ==============================================================================================================*/
#define record_waiting_files(  )									\
		do											\
		   {											\
			if ( filenames != 0 )								\
			   {										\
				fi. lineno = tgts_started ;						\
				record_files ( filenames, pattern, pattern_percent, depstr,		\
						cmds_started, commands, commands_idx, two_colon,	\
						prefix, &fi, is_heredoc_command ) ;                     \
				filenames	=  0 ;							\
			    }										\
													\
			commands_idx	=  0 ;								\
			no_targets	=  0 ;								\
			pattern		=  0 ;								\
		    } while ( 0 )



/*==============================================================================================================

    eval -
        Read file FILENAME as a makefile and add its contents to the data base.

	SET_DEFAULT is true if we are allowed to set the default goal.

  ==============================================================================================================*/
void	eval ( readbuffer *  ebuf, int  set_default )
{
	char *			collapsed		=  0 ;
	unsigned int		collapsed_length	=  0 ;
	unsigned int		commands_len		=  200 ;
	char *			commands ;
	unsigned int		commands_idx		=  0 ;
	unsigned int		cmds_started, 
				tgts_started ;
	int			ignoring		=  0, 
				in_ignored_define	=  0 ;
	int			no_targets		=  0 ;		/*  Set when reading a rule without targets. */
	struct nameseq *	filenames		=  0 ;
	char *			depstr			=  0 ;
	long			nlines			=  0 ;
	int			two_colon		=  0 ;
	char			prefix			=  cmd_prefix ;
	const char *		pattern			=  0 ;
	const char *		pattern_percent ;
	gmk_floc *		fstart ;
	gmk_floc		fi ;
	int			is_heredoc_command	=  0 ;
	stringbuffer		sb ;					/* String buffer for here documents */


	pattern_percent		=  0 ;
	cmds_started		=  
	tgts_started		=  1 ;
	fstart			=  & ebuf -> floc ;
	fi. filenm		=  ebuf -> floc. filenm ;

	/***  
		Loop over lines in the file. 
		The strategy is to accumulate target names in FILENAMES, dependencies in DEPS and commands in COMMANDS.   
		These are used to define a rule when the start of the next rule ( or eof ) is encountered. 

		When you see a "continue" in the loop below, that means we are moving on to the next line.   
		If you see record_waiting_files(  ), then the statement	we are parsing also finishes the previous rule.   
	 ***/
	commands	=  xmalloc ( 200 ) ;
	stringbuffer_init ( & sb, NULL, '\n' ) ;

	while ( 1 )
	   {
		unsigned int		linelen ;
		char *			line ;
		unsigned int		wlen ;
		char *			p ;
		char *			p2 ;
		variable_modifiers	vmod ;
		heredocdata *		heredoc ;
		

		/*  At the top of this loop, we are starting a brand new line.  */
		/*  Grab the next line to be evaluated				*/
		/*  Handle potentially buffered lines for heredocs		*/
		ebuf -> floc. lineno	+=  nlines ;

		if  ( stringbuffer_empty ( & sb ) )
		   {
			nlines			 = readline ( ebuf, & heredoc ) ;

			/*  If there is nothing left to eval, we're done.   */
			if ( nlines < 0 )
				break ;

			line			=  ebuf -> buffer ;
			is_heredoc_command	=  0 ;
		    }
		else
		   {
			nlines			=  stringbuffer_lines ( & sb ) ;

			if  ( ! nlines )
				break ;

			line			=  xstrdup ( sb. next ) ;

			stringbuffer_reset ( & sb ) ;
		    }

		/*  If this is the first line, check for a UTF-8 BOM and skip it.   */
		if ( ebuf -> floc. lineno  ==  1  &&  line [0]  ==  ( char ) 0xEF  &&
				line [1]  ==  ( char ) 0xBB   &&  line [2]  ==  ( char ) 0xBF )
		   {
			line	+= 3 ;

			if ( ISDB( DB_BASIC ) )
			   {
				if ( ebuf -> floc. filenm )
					printf ( _( "Skipping UTF-8 BOM in makefile '%s'\n" ),	ebuf -> floc. filenm ) ;
				else
					printf ( _( "Skipping UTF-8 BOM in makefile buffer\n" ) ) ;
			    }
		    }

		/*  If this line is empty, skip it.   */
		if ( line [0] == '\0' )
			continue ;

		linelen = strlen ( line ) ;

		/*  Check for a shell command line first. If it is not one, we can stop treating cmd_prefix specially.   */
		if  ( line [0]  ==  cmd_prefix )
		   {
			/*  Ignore the commands in a rule with no targets.   */
			if  ( no_targets )
				continue ;

			/*  If there is no preceding rule line, don't treat this line as a command, even though it	*/
			/* begins with a recipe prefix. SunOS 4 make appears to behave this way.			*/
			if  ( filenames  !=  0 )
			   {
				/*  Yep, this is a shell command, and we don't care.   */
				if  ( ignoring )
					continue ;

				if  ( commands_idx  ==  0 )
					cmds_started	= ebuf -> floc. lineno ;

				/*  Append this command line to the line being accumulated. Skip the initial command prefix character. */
				if  ( linelen + commands_idx > commands_len )
				   {
					commands_len	=  ( linelen + commands_idx ) *  2 ;
					commands	=  xrealloc ( commands, commands_len ) ;
				    }

				memcpy ( & commands [ commands_idx ], line + 1, linelen - 1 ) ;
				commands_idx		+=  linelen - 1 ;
				commands[commands_idx++] =  '\n' ;

				continue ;
			    }
		    }

		/* This line is not a shell command line.   Don't worry about whitespace.			*/
		/* Get more space if  we need it ; we don't need to preserve the current contents of the buffer.*/
		if  ( collapsed_length  <  linelen + 1 )
		   {
			collapsed_length	=  linelen + 1 ;
			free ( collapsed ) ;

			/*  Don't need xrealloc: we don't need to preserve the content.   */
			collapsed		=  xmalloc ( collapsed_length ) ;
		    }

		strcpy ( collapsed, line ) ;

		/*  Collapse continuation lines and remove comments */
		if  ( heredoc  !=  NULL )
		   {
			   replace_heredoc ( collapsed, heredoc, '#', '\1' ) ;
			   remove_comments ( collapsed ) ;
			   replace_heredoc ( collapsed, heredoc, '\1', '#' ) ;
		    }
		else
			remove_comments ( collapsed ) ;

		/*  Get rid if  starting space ( including formfeed, vtab, etc.  )  */
		p	=  collapsed ;

		while  ( isspace ( ( unsigned char )* p ) )
			++ p ;

		/*  See if  this is a variable assignment.   We need to do this early, to allow variables with names like	*/
		/* 'ifdef', 'export', 'private', etc.										*/
		p	=  parse_var_assignment ( p, & vmod ) ;

		if  ( vmod. vm_assign )
		   {
			/***
				Lines containing variables with heredocs, such as :

					VAR = <<
						line 1
						...
						line n
					END

				are returned as is. 
				Although readline() returns the whole construct, it is still recognized as a variable assignment.
				We then need to process the real heredoc data (from "VAR" to "END") which is stored in the
				heredoc structure returned by readline().
				The tranform_heredoc() function will transform the initial definition into :

					VAR = \
						line 1\
						...\
						line n

				At that point, we must call collapse_continuations() a second time to join the continuation
				lines and obtain the following :

					VAR = line 1 ... line n

				Yes, that's acrobatic !
			 ***/
			if  ( heredoc  !=  NULL )
			   {
				collapsed	=  transform_heredoc ( collapsed, heredoc, 1 ) ;
				collapse_continuations ( collapsed ) ;
			    }

			struct variable *	v ;
			enum variable_origin	origin	=  ( vmod. vm_override ) ?  o_override : o_file ;

			/*  Variable assignment ends the previous rule.   */
			record_waiting_files (  ) ;

			/*  if  we're ignoring then we're done now.   */
			if  ( ignoring )
			   {
				if  ( vmod. vm_define )
					in_ignored_define	= 1 ;

				continue ;
			    }

			if  ( vmod. vm_undefine )
			   {
				do_undefine ( p, origin, ebuf ) ;
				continue ;
			    }
			else if  ( vmod. vm_define )
				v = do_define ( p, origin, ebuf ) ;
			else
				v = try_variable_definition ( fstart, p, origin, 0 ) ;

			assert ( v  !=  NULL ) ;

			if  ( vmod. vm_export )
				v -> export	=  v_export ;

			if  ( vmod. vm_private )
				v -> private_var = 1 ;

			/*  This line has been dealt with.   */
			continue ;
		    }

		/*  if  this line is completely empty, ignore it.   */
		if  ( * p  ==  '\0' )
			continue ;

		/***
			Process here documents for rules.
			Since the readline() function is not aware of the context, it returned the whole rule
			together with the heredoc construct ; we now have to process this bunch of data line per
			line, after removing the heredoc construct.
		 ***/
		if  ( heredoc  !=  NULL )
		   {
			char *		transformed	=  transform_heredoc ( collapsed, heredoc, 0 ) ;

			stringbuffer_init ( & sb, transformed, '\n' ) ;
			free ( collapsed ) ;

			p		= 
			line		= 
			collapsed	=  stringbuffer_next ( & sb ) ;
		    }

		p2	=  end_of_token ( p ) ;
		wlen	=  p2 - p ;
		p2	=  next_token ( p2 ) ;

		/*  if  we're in an ignored define, skip this line ( but maybe get out ).   */
		if  ( in_ignored_define )
		   {
			/*  See if  this is an endef line ( plus optional comment ).   */
			if  ( word1eq ( "endef", p, wlen )  &&  STOP_SET ( * p2, MAP_COMMENT | MAP_NUL ) )
				in_ignored_define	=  0 ;

			continue ;
		    }

		/*  Check for conditional state changes.   */
		   {
			int i = conditional_line ( p, wlen, fstart ) ;

			if  ( i  !=  -2 )
			   {
				if  ( i  ==  -1 )
					O ( fatal, fstart, _( "invalid syntax in conditional" ) ) ;

				ignoring	=  i ;
				continue ;
			    }
		    }

		/*  Nothing to see here. . .  move along.   */
		if  ( ignoring )
			continue ;

		/*  Manage the "export" keyword used outside of variable assignment as well as "unexport".   */
		if  ( word1eq ( "export", p, wlen )  ||  word1eq ( "unexport", p, wlen ) )
		   {
			int	exporting	=  ( * p  ==  'u' ) ?  0 : 1 ;

			/*  Export/unexport ends the previous rule.   */
			record_waiting_files (  ) ;

			/*  ( un )export by itself causes everything to be ( un )exported.  */
			if  ( * p2  ==  '\0' )
				export_all_variables	= exporting ;
			else
			   {
				unsigned int	l ;
				const char *	cp ;
				char *		ap ;

				/*  Expand the line so we can use indirect and constructed
				variable names in an ( un )export command.   */
				cp = ap = allocated_variable_expand ( p2 ) ;

				for ( p = find_next_token ( &cp, &l ) ; p != 0 ; p = find_next_token ( &cp, &l ) )
				   {
					struct variable *	v	=  lookup_variable ( p, l ) ;

					if  ( v  ==  0 )
						v = define_variable_global ( p, l, "", o_file, 0, fstart ) ;

					v -> export	= ( exporting ) ?  v_export : v_noexport ;
				    }

				free ( ap ) ;
			    }
			continue ;
		    }

		/*  Handle the special syntax for vpath.   */
		if  ( word1eq ( "vpath", p, wlen ) )
		   {
			const char *	cp ;
			char *		vpat ;
			unsigned int	l ;

			/*  vpath ends the previous rule.   */
			record_waiting_files (  ) ;

			cp	=  variable_expand ( p2 ) ;
			p	=  find_next_token ( & cp, & l ) ;

			/*  No searchpath means remove all previous selective VPATH's with the same pattern.   */
			if  ( p != 0 )
			   {
				vpat	=  xstrndup ( p, l ) ;
				p	=  find_next_token ( & cp, & l ) ;
			    }
			/*  No pattern means remove all previous selective VPATH's.   */
			else
				vpat	=  0 ;

			construct_vpath_list ( vpat, p ) ;
			free ( vpat ) ;

			continue ;
		    }

		/*  Handle include and variants.   */
		if  ( word1eq ( "include", p, wlen )  ||  word1eq ( "-include", p, wlen )  ||  word1eq ( "sinclude", p, wlen ) )
		   {
			/*  We have found an 'include' line specif ying a nested makefile to be read at this point.   */
			conditional_directives *	save ;
			conditional_directives		new_conditionals ;
			struct nameseq *		files ;
			/*  "-include" ( vs "include" ) says no error if  the file does not exist. "sinclude" is an alias for this from SGI. */
			int				noerror = ( p [0]  !=  'i' ) ;

			/*  Include ends the previous rule.   */
			record_waiting_files (  ) ;

			p	=  allocated_variable_expand ( p2 ) ;

			/*  if  no filenames, it's a no-op.   */
			if  ( * p  ==  '\0' )
			   {
				free ( p ) ;
				continue ;
			    }

			/*  Parse the list of file names.   Don't expand archive references!  */
			p2	=  p ;
			files	=  PARSE_FILE_SEQ ( &p2, struct nameseq, MAP_NUL, NULL,	PARSEFS_NOAR ) ;
			free ( p ) ;

			/*  Save the state of conditionals and start
			the included makefile with a clean slate.   */
			save	=  install_conditionals ( & new_conditionals ) ;

			/*  Record the rules that are waiting so they will determine the default goal before those in the included makefile. */
			record_waiting_files (  ) ;

			/*  Read each included makefile.   */
			while  ( files  !=  0 )
			   {
				struct nameseq *	next	=  files -> next ;
				const char *		name	=  files -> name ;
				int			r ;

				free_ns ( files ) ;

				files	=  next ;
				r	=  eval_makefile ( name, 
						( RM_INCLUDED | RM_NO_TILDE |
							( ( noerror     ) ?  RM_DONTCARE : 0 )  |
							( ( set_default ) ?  0 : RM_NO_DEFAULT_GOAL ) ) ) ;

				if  ( ! r  &&  ! noerror )
				   {
					const char *	err	=  strerror ( errno ) ;

					OSS ( error, fstart, "%s: %s", name, err ) ;
				    }
			    }

			/*  Restore conditional state.   */
			restore_conditionals ( save ) ;

			continue ;
		    }

		/*  Handle the load operations.   */
		if  ( word1eq ( "load", p, wlen )  ||  word1eq ( "-load", p, wlen ) )
		   {
			/*  A 'load' line specif ies a dynamic object to load.   */
			struct nameseq *	files ;
			int			noerror		=  ( p[0]  ==  '-' ) ;

			/*  Load ends the previous rule.   */
			record_waiting_files (  ) ;

			p	=  allocated_variable_expand ( p2 ) ;

			/*  if  no filenames, it's a no-op.   */
			if  ( * p  ==  '\0' )
			   {
				free ( p ) ;
				continue ;
			    }

			/*  Parse the list of file names. Don't expand archive references or strip ". /"  */
			p2	=  p ;
			files	=  PARSE_FILE_SEQ ( &p2, struct nameseq, MAP_NUL, NULL,	PARSEFS_NOAR ) ;

			free ( p ) ;

			/*  Load each file.   */
			while  ( files  !=  0 )
			   {
				struct nameseq *	next	=  files -> next ;
				const char *		name	= files -> name ;
				struct dep *		deps ;
				int			r ;

				/*  Load the file.   0 means failure.   */
				r	=  load_file ( & ebuf -> floc, & name, noerror ) ;

				if  ( ! r  &&  ! noerror )
					OS ( fatal, & ebuf -> floc, _( "%s: failed to load" ), name ) ;

				free_ns ( files ) ;
				files	=  next ;

				/*  Return of -1 means a special load: don't rebuild it.   */
				if  ( r  ==  -1 )
					continue ;

				/*  It succeeded, so add it to the list "to be rebuilt".   */
				deps		=  alloc_dep (  ) ;
				deps -> next	=  read_files ;
				read_files	=  deps ;
				deps -> file	=  lookup_file ( name ) ;

				if  ( deps -> file  ==  0 )
					deps -> file	=  enter_file ( name ) ;

				deps -> file -> loaded	=  1 ;
			    }

			continue ;
		    }

		/***  
			This line starts with a tab but was not caught above because there was no preceding target, 
			and the line might have been usable as a variable definition. But now we know it is definitely lossage.   
		 ***/
		if  ( line[0]  ==  cmd_prefix )
			O ( fatal, fstart, _( "recipe commences before first target" ) ) ;

		/***  
			This line describes some target files.   This is complicated by	the existence of target-specif ic variables, 
			because we can't expand the entire line until we know if  we have one or not.   
			So we expand the line word by word until we find the first ':', then check to see if  it's a target-specif 
			ic variable. 

			In this algorithm, 'lb_next' will point to the beginning of the	unexpanded parts of the input buffer, 
			while  'p2' points to the parts of the expanded buffer we haven't searched yet.  
		 ***/
		   {
			makewordtype		wtype ;
			char *			cmdleft, 
			     *			semip, 
			     *			lb_next ;
			unsigned int		plen		=  0 ;
			char *			colonp ;
			const char *		end, 
				   *		beg ;		/*  Helpers for whitespace stripping.  */

			/*  Record the previous rule.   */
			record_waiting_files (  ) ;

			tgts_started	=  fstart -> lineno ;

			/*  Search the line for an unquoted  ; that is not after an unquoted #.   */
			cmdleft = find_char_unquote ( line, MAP_SEMI | MAP_COMMENT| MAP_VARIABLE ) ;

			if  ( cmdleft  !=  0  &&  * cmdleft  ==  '#' )
			   {
				/*  COMMENT HERE */
				/*  We found a comment before a semicolon.   */
				* cmdleft	=  '\0' ;
				cmdleft		=  0 ;
			    }
			/*  Found one.   Cut the line short there before expanding it.   */
			else if  ( cmdleft  !=  0 )
				* ( cmdleft ++ )	= '\0' ;

			semip	=  cmdleft ;

			collapse_continuations ( line ) ;

			/***  
				We can't expand the entire line, since if  it's a per-target variable we don't want to expand it.   
				So, walk from the beginning, expanding as we go, and looking for "interesting"	chars.   
				The first word is always expandable.   
			 ***/
			wtype	=  get_next_makeword ( line, NULL, & lb_next, & wlen ) ;

			switch ( wtype )
			   {
				/*  This line contained something but turned out to be nothing but whitespace ( a comment? ).   */
				case	wt_eol :
					if  ( cmdleft  !=  0 )
						O ( fatal, fstart, _( "missing rule before recipe" ) ) ;

					continue ;

				/*  We accept and ignore rules without targets for compatibility with SunOS 4 make.   */
				case	wt_colon :
				case	wt_dcolon :
					no_targets	= 1 ;
					continue ;

				default:
					break ;
			    }

			p2 = variable_expand_string ( NULL, lb_next, wlen ) ;

			while  ( 1 )
			   {
				lb_next		+=  wlen ;

				if  ( cmdleft  ==  0 )
				   {
					/*  Look for a semicolon in the expanded line.   */
					cmdleft		=  find_char_unquote ( p2, MAP_SEMI ) ;

					if  ( cmdleft  !=  0 )
					   {
						unsigned long	p2_off		=  p2 - variable_buffer ;
						unsigned long	cmd_off		=  cmdleft - variable_buffer ;
						char *		pend		=  p2 + strlen ( p2 ) ;

						/*  Append any remnants of lb, then cut the line short at the semicolon.   */
						* cmdleft = '\0' ;

						/***  
							One school of thought says that you shouldn't expand here, but merely copy, 
							since now you're beyond a ";" and into a command script.   
							However, the old parser	expanded the whole line, so we continue that for
							backwards-compatibility.   
							Also, it wouldn't be entirely consistent, since we do an unconditional
							expand below once we know we don't have a target-specif ic variable.  
						 ***/
						( void ) variable_expand_string ( pend, lb_next, ( long )-1 ) ;

						lb_next		+=  strlen ( lb_next ) ;
						p2		 =  variable_buffer + p2_off ;
						cmdleft		 =  variable_buffer + cmd_off + 1 ;
					    }
				    }

				colonp = find_char_unquote ( p2, MAP_COLON ) ;

#ifdef HAVE_DOS_PATHS
				/***  
					The drive spec brain-damage strikes again. . .
					Note that the only separators of targets in this context are whitespace and a left paren.   
					If  others are possible, they should be added to the string in the call to index.   
				 ***/
				while  ( colonp  &&  ( colonp[1]  ==  '/'  ||  colonp[1]  ==  '\\' )  && 
						colonp > p2  &&  isalpha ( ( unsigned char ) colonp [-1] )  && 
						( colonp  ==  p2 + 1  ||  strchr ( " \t( ", colonp[-2] ) != 0 ) )
					colonp	=  find_char_unquote ( colonp + 1, MAP_COLON ) ;
#endif 

				if  ( colonp != 0 )
					break ;

				wtype	=  get_next_makeword ( lb_next, NULL, & lb_next, &wlen ) ;

				if  ( wtype  ==  wt_eol )
					break ;

				p2		+=  strlen ( p2 ) ;
				* ( p2++ )	 =  ' ' ;
				p2		 =  variable_expand_string ( p2, lb_next, wlen ) ;

				/***  
					We don't need to worry about cmdleft here, because if  it was
					found in the variable_buffer the entire buffer has already
					been expanded. . .  we'll never get here.   
				 ***/
			    }

			p2 = next_token ( variable_buffer ) ;

			/***  
				if  the word we're looking at is EOL, see if  there's _anything_ on the line.   
				if  not, a variable expanded to nothing, so ignore it.   
				if  so, we can't parse this line so punt.   
			 ***/
			if  ( wtype  ==  wt_eol )
			   {
				if  ( * p2  ==  '\0' )
					continue ;

				/*  There's no need to be ivory-tower about this: check for
				one of the most common bugs found in makefiles. . .   */
				if  ( cmd_prefix  ==  '\t'  &&  strneq ( line, "        ", 8 ) )
					O ( fatal, fstart, _( "missing separator ( did you mean TAB instead of 8 spaces? )" ) ) ;
				else
					O ( fatal, fstart, _( "missing separator" ) ) ;
			    }

			/***  
				Make the colon the end-of-string so we know where to stop looking for targets.   
				Start there again once we're done.   
			 ***/
			* colonp	=  '\0' ;
			filenames	=  PARSE_SIMPLE_SEQ ( &p2, struct nameseq ) ;
			* colonp	=  ':' ;
			p2		=  colonp ;

			if  ( ! filenames )
			   {
				/*  We accept and ignore rules without targets for compatibility with SunOS 4 make.   */
				no_targets	=  1 ;
				continue ;
			    }

			/*  This should never be possible ; we handled it above.   */
			assert ( * p2  !=  '\0' ) ;
			++ p2 ;

			/*  Is this a one-colon or two-colon entry?  */
			two_colon = * p2  ==  ':' ;
			if  ( two_colon )
				p2++ ;

			/*** 
				Test to see if  it's a target-specif ic variable.   Copy the rest of the buffer over, 
				possibly temporarily (we'll expand it later if  it's not a target-specif ic variable ).   
				PLEN saves the length of the unparsed section of p2, for later.   
			 ***/
			if  ( * lb_next  !=  '\0' )
			   {
				unsigned int	l	=  p2 - variable_buffer ;

				plen	=  strlen ( p2 ) ;
				variable_buffer_output ( p2+plen, lb_next, strlen ( lb_next )+1 ) ;
				p2	=  variable_buffer + l ;
			    }

			p2	=  parse_var_assignment ( p2, & vmod ) ;

			if  ( vmod. vm_assign )
			   {
				/*  if  there was a semicolon found, add it back, plus anything after it.   */
				if  ( semip )
				   {
					unsigned int l		=  p2 - variable_buffer ;

					* ( -- semip )	=  ';' ;
					collapse_continuations ( semip ) ;
					variable_buffer_output ( p2 + strlen ( p2 ), semip, strlen ( semip ) + 1 ) ;
					p2		=  variable_buffer + l ;
				    }

				record_target_var ( filenames, p2, ( vmod. vm_override ) ?  o_override : o_file, & vmod, fstart ) ;
				filenames	=  0 ;

				continue ;
			    }

			/*  This is a normal target, _not_ a target-specif ic variable. Unquote any = in the dependency list.   */
			find_char_unquote ( lb_next, MAP_EQUALS ) ;

			/*  Remember the command prefix for this target.   */
			prefix		=  cmd_prefix ;

			/*  We have some targets, so don't ignore the following commands.   */
			no_targets	=  0 ;

			/*  Expand the dependencies, etc.   */
			if  ( * lb_next  !=  '\0' )
			   {
				unsigned int		l	=  p2 - variable_buffer ;

				( void ) variable_expand_string ( p2 + plen, lb_next, ( long ) -1 ) ;

				p2	=  variable_buffer + l ;

				/*  Look for a semicolon in the expanded line.   */
				if  ( cmdleft  ==  0 )
				   {
					cmdleft		=  find_char_unquote ( p2, MAP_SEMI ) ;

					if  ( cmdleft  !=  0 )
						* ( cmdleft++ ) = '\0' ;
				    }
			    }

			/*  Is this a static pattern rule: 'target: %targ: %dep ; . . . '?  */
			p	=  strchr ( p2, ':' ) ;

			while  ( p  !=  0  &&  p [-1]  ==  '\\' )
			   {
				char *		q		=  & p [-1] ;
				int		backslash	=  0 ;

				while  ( * q --  ==  '\\' )
					backslash	=  ! backslash ;

				if  ( backslash )
					p = strchr ( p + 1, ':' ) ;
				else
					break ;
			    }

#ifdef _AMIGA
			/***  
				Here, the situation is quite complicated.  Let's have a look at a couple of targets:

				install: dev:make

				dev:make: make

				dev:make:: xyz

				The rule is that it's only a target, if  there are TWO :'s OR a space around the :. 
			 ***/
			if  ( p  &&  !( isspace ( ( unsigned char ) p [1] )  ||  ! p [1]  ||
					isspace ( ( unsigned char ) p [-1] ) ) )
				p = 0 ;
#endif 

#ifdef HAVE_DOS_PATHS
			   {
				int	check_again ;

				do    
				   {
					check_again	=  0 ;

					/*  For DOS-style paths, skip a "C:\. . . " or a "C:/. . . " */
					if  ( p != 0  &&  ( p[1]  ==  '\\'  ||  p[1]  ==  '/' )  && 
							isalpha ( ( unsigned char ) p[-1] )  && 
							( p  ==  p2 + 1  ||  strchr ( " \t:( ", p [-2] ) != 0 ) )    
					   {
							p		=  strchr ( p + 1, ':' ) ;
							check_again	= 1 ;
					    }
				     } while  ( check_again ) ;
			    }
#endif 

			if  ( p  !=  0 )
			   {
				struct nameseq *	target ;


				target = PARSE_FILE_SEQ ( &p2, struct nameseq, MAP_COLON, NULL,	PARSEFS_NOGLOB ) ;
				++ p2 ;

				if  ( target  ==  0 )
					O ( fatal, fstart, _( "missing target pattern" ) ) ;
				else if  ( target -> next  !=  0 )
					O ( fatal, fstart, _( "multiple target patterns" ) ) ;

				pattern_percent		=  find_percent_cached ( & target -> name ) ;
				pattern			=  target -> name ;

				if  ( pattern_percent  ==  0 )
					O ( fatal, fstart, _( "target pattern contains no '%%'" ) ) ;

				free_ns ( target ) ;
			    }
			else
				pattern		=  0 ;

			/*  Strip leading and trailing whitespaces.  */
			beg	=  p2 ;
			end	=  beg + strlen ( beg ) - 1 ;

			strip_whitespace ( & beg, & end ) ;

			/*  Put all the prerequisites here ; they'll be parsed later.   */
			if  ( beg  <=  end  &&  * beg  !=  '\0' )
				depstr	=  xstrndup ( beg, end - beg + 1 ) ;
			else
				depstr  =  0 ;

			commands_idx	=  0 ;

			if  ( cmdleft  !=  0 )
			   {
				/*  Semicolon means rest of line is a command.   */
				unsigned int	l	=  strlen ( cmdleft ) ;

				cmds_started	=  fstart -> lineno ;

				/*  Add this command line to the buffer.   */
				if  ( l + 2 > commands_len )
				   {
					commands_len	=  ( l + 2 ) *  2 ;
					commands	=  xrealloc ( commands, commands_len ) ;
				    }
				 
				memcpy ( commands, cmdleft, l ) ;
				commands_idx			+=  l ;
				commands [ commands_idx ++ ]	 =  '\n' ;
			    }

			/***  
				Determine if  this target should be made default.  We used to do this in record_files() but 
				because of the delayed target recording	and because preprocessor directives are legal in 
				target's commands it is too late.  Consider this fragment for example:

				foo:

				if eq ( $( . DEFAULT_GOAL ),foo )
				. . . 
				endif 

				Because the target is not recorded until after if eq directive is evaluated the .DEFAULT_GOAL 
				does not contain foo yet as one	would expect.  Because of this we have to move the logic here.   
			 ***/
			if  ( set_default  &&  default_goal_var -> value [0]  ==  '\0' )
			   {
				struct dep *		d ;
				struct nameseq *	t	=  filenames ;

				for (  ; t  !=  0 ; t = t -> next )
				   {
					int		reject	=  0 ;
					const char *	name	=  t -> name ;

					/*  We have nothing to do if  this is an implicit rule.  */
					if  ( strchr ( name, '%' )  !=  0 )
						break ;

					/*  See if  this target's name does not start with a '.', unless it contains a slash.   */
					if  ( * name  ==  '.'  &&  strchr ( name, '/' )  ==  0
#ifdef HAVE_DOS_PATHS
						 &&  strchr ( name, '\\' )  ==  0
#endif 
						 )
						continue ;

					/*  if  this file is a suffix, don't let it be the default goal file.   */
					for ( d = suffix_file -> deps ; d  !=  0 ; d = d -> next )
					   {
						register struct dep *	d2 ;

						if  ( * dep_name ( d ) != '.'  &&  streq ( name, dep_name ( d ) ) )
						   {
							reject	=  1 ;
							break ;
						    }

						for ( d2 = suffix_file -> deps ; d2  !=  0 ; d2 = d2 -> next )
						   {
							unsigned int	l	=  strlen ( dep_name ( d2 ) ) ;

							if  ( ! strneq ( name, dep_name ( d2 ), l ) )
								continue ;
							if  ( streq ( name + l, dep_name ( d ) ) )
							   {
								reject	=  1 ;
								break ;
							    }
						    }

						if  ( reject )
							break ;
					    }

					if  ( ! reject )
					   {
						define_variable_global ( ".DEFAULT_GOAL", 13, t -> name, o_file, 0, NILF ) ;
						break ;
					    }
				    }
			    }

			continue ;
		    }

		/***  
			We get here except in the case that we just read a rule line. 
			Record now the last rule we read, so following spurious	commands are properly diagnosed.   
		 ***/
		record_waiting_files (  ) ;
	    }

	if  ( conditionals -> if_cmds )
		O ( fatal, fstart, _( "missing 'endif '" ) ) ;

	/*  At eof, record the last rule.   */
	record_waiting_files (  ) ;

	free ( collapsed ) ;
	free ( commands ) ;
    }


/*==============================================================================================================

    eval_makefile -
        Evaluates the contents of a makefile.

  ==============================================================================================================*/
int	eval_makefile ( const char * filename, int flags )
   {
	struct dep *		deps ;
	readbuffer		ebuf ;
	const gmk_floc *	curfile ;
	char *			expanded	=  0 ;
	int			makefile_errno ;

	ebuf. floc. filenm = filename ;		/*  Use the original file name.   */
	ebuf. floc. lineno = 1 ;

	if  ( ISDB ( DB_VERBOSE ) )
	   {
		printf ( _( "Reading makefile '%s'" ), filename ) ;

		if  ( flags & RM_NO_DEFAULT_GOAL )
			printf ( _( " ( no default goal )" ) ) ;

		if  ( flags & RM_INCLUDED )
			printf ( _( " ( search path )" ) ) ;

		if  ( flags & RM_DONTCARE )
			printf ( _( " ( don't care )" ) ) ;

		if  ( flags & RM_NO_TILDE )
			printf ( _( " ( no ~ expansion )" ) ) ;

		puts ( ". . . " ) ;
	    }

	/*  First, get a stream to read.   */

	/*  Expand ~ in FILENAME unless it came from 'include', in which case it was already done.   */
	if  ( ! ( flags & RM_NO_TILDE )  &&  filename[0]  ==  '~' )
	   {
		expanded = tilde_expand ( filename ) ;

		if  ( expanded != 0 )
			filename = expanded ;
	    }

	ENULLLOOP ( ebuf. fp, fopen ( filename, "r" ) ) ;

	/*  Save the error code so we print the right message later.   */
	makefile_errno	=  errno ;

	/*  Check for unrecoverable errors: out of mem or FILE slots.   */
	switch ( makefile_errno )
	   {
#ifdef EMFILE
		case EMFILE :
#endif 
#ifdef ENFILE
		case ENFILE :
#endif 
		case ENOMEM :
			OS ( fatal, reading_file, "%s", strerror ( makefile_errno ) ) ;
			break ;
	    }

	/*  if  the makefile wasn't found and it's either a makefile from
	the 'MAKEFILES' variable or an included makefile,
	search the included makefile search path for this makefile.   */
	if  ( ebuf. fp  ==  0  &&  ( flags & RM_INCLUDED )  &&  * filename  !=  '/' )
	   {
		unsigned int	i ;

		for ( i = 0 ; include_directories [i]  !=  0 ; ++ i )
		   {
			const char *	included	= concat ( 3, include_directories [i], "/", filename ) ;

			ebuf. fp	=  fopen ( included, "r" ) ;

			if  ( ebuf. fp )
			   {
				filename	= included ;
				break ;
			    }
		    }
	    }

	/*  Now we have the final name for this makefile.  Enter it into the cache.   */
	filename	=  strcache_add ( filename ) ;

	/*  Add FILENAME to the chain of read makefiles.   */
	deps		=  alloc_dep (  ) ;
	deps -> next	=  read_files ;
	read_files	=  deps ;
	deps -> file	=  lookup_file ( filename ) ;

	if  ( deps -> file  ==  0 )
		deps -> file = enter_file ( filename ) ;

	filename	=  deps -> file -> name ;
	deps -> changed =  flags ;

	if  ( flags & RM_DONTCARE )
		deps -> dontcare = 1 ;

	free ( expanded ) ;

	/*  if  the makefile can't be found at all, give up entirely.   */
	if  ( ebuf. fp  ==  0 )
	   {
		/***  
			if  we did some searching, errno has the error from the last attempt, rather from FILENAME itself.   
			Restore it in case the caller wants to use it in a message.   
		 ***/
		errno	= makefile_errno ;
		return ( 0 ) ;
	    }

	/*  Set close-on-exec to avoid leaking the makefile to children, such as $( shell . . .  ).   */
#ifdef HAVE_FILENO
	CLOSE_ON_EXEC ( fileno ( ebuf. fp ) ) ;
#endif 

	/*  Add this makefile to the list.  */
	do_variable_definition ( &ebuf. floc, "MAKEFILE_LIST", filename, o_file, f_append, 0 ) ;

	/*  Evaluate the makefile */
	readbuffer_alloc ( & ebuf, READBUFFER_SIZE ) ;

	curfile		=  reading_file ;
	reading_file	=  & ebuf. floc ;

	eval ( & ebuf, ! ( flags & RM_NO_DEFAULT_GOAL ) ) ;

	reading_file	=  curfile ;

	fclose ( ebuf. fp ) ;

	free ( ebuf. bufstart ) ;
	alloca ( 0 ) ;

	return ( 1 ) ;
    }


/*==============================================================================================================

    eval_buffer -
        Evaluates a buffer.

  ==============================================================================================================*/
void	eval_buffer ( char *  buffer, const gmk_floc *  floc )
   {
	struct readbuffer		ebuf ;
	conditional_directives *	saved ;
	conditional_directives		new ;
	const gmk_floc *		curfile ;


	/*  Evaluate the buffer */
	ebuf. size	=  strlen ( buffer ) ;
	ebuf. buffer	=  
	ebuf. bufnext	= 
	ebuf. bufstart	= buffer ;
	ebuf. fp	=  NULL ;

	if  ( floc )
		ebuf. floc	=  * floc ;
	else if  ( reading_file )
		ebuf. floc	=  * reading_file ;
	else
	   {
		ebuf. floc. filenm	=  NULL ;
		ebuf. floc. lineno	=  1 ;
	    }

	curfile		=  reading_file ;
	reading_file	=  & ebuf. floc ;

	saved		=  install_conditionals ( & new ) ;

	eval ( & ebuf, 1 ) ;

	restore_conditionals ( saved ) ;

	reading_file = curfile ;

	alloca ( 0 ) ;
    }


# endif 		/*  WAKE_EXTENSIONS */