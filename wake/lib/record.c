/**************************************************************************************************************

    NAME
        record.c

    DESCRIPTION
        description.

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

    record_target_var -
        Record target-specific variable values for files FILENAMES.
	TWO_COLON is nonzero if a double colon was used.

	The links of FILENAMES are freed, and so are any names in it that are not incorporated into other data 
	structures.

	If the target is a pattern, add the variable to the pattern-specific variable value list.

  ==============================================================================================================*/
void	record_target_var  ( struct nameseq *		filenames, 
			     char *			defn,
			     enum variable_origin	origin, 
			     variable_modifiers *	vmod,
			     const gmk_floc *		flocp )
    {
	struct nameseq *		nextf ;
	struct variable_set_list *	global ;


	global	=  current_variable_set_list ;

	/* If the variable is an append version, store that but treat it as a normal recursive variable.  */
	for  ( ; filenames  !=  0 ; filenames = nextf )
	   {
		struct variable *	v ;
		const char *		name	=  filenames -> name ;
		const char *		percent ;
		struct pattern_var *	p ;

		nextf	=  filenames -> next ;
		free_ns ( filenames ) ;

		/* If it's a pattern target, then add it to the pattern-specific variable list.  */
		percent		=  find_percent_cached ( & name ) ;

		if  ( percent )
		   {
			/* Get a reference for this pattern-specific variable struct.  */
			p			=  create_pattern_var ( name, percent ) ;
			p->variable.fileinfo	=  * flocp ;

			/* I don't think this can fail since we already determined it was a variable definition.  */
			v	=  assign_variable_definition ( & p -> variable, defn ) ;
			assert ( v != 0 ) ;

			v -> origin	=  origin ;

			if  ( v -> flavor  ==  f_simple )
				v -> value	=  allocated_variable_expand ( v -> value ) ;
			else
				v -> value	=  xstrdup ( v -> value ) ;
		    }
		else
		   {
			struct file *		f ;

			/* Get a file reference for this file, and initialize it.					*/
			/* We don't want to just call enter_file() because that allocates a new entry if the file	*/
			/* is a double-colon, which we don't want in this situation.					*/
			f	=  lookup_file ( name ) ;

			if  ( ! f )
				f	=  enter_file ( strcache_add ( name ) ) ;
			else if  ( f -> double_colon )
				f	=  f -> double_colon ;

			initialize_file_variables (f, 1) ;

			current_variable_set_list	=  f -> variables ;
			v				=  try_variable_definition ( flocp, defn, origin, 1 ) ;

			if  ( ! v )
				O ( fatal, flocp, _( "Malformed target-specific variable definition" ) ) ;

			current_variable_set_list	=  global ;
		    }

		/* Set up the variable to be *-specific.  */
		v->per_target = 1;
		v->private_var = vmod->vm_private;
		v->export = vmod->vm_export ? v_export : v_default;

		/* If it's not an override, check to see if there was a command-line setting.  If so, reset the value.  */
		if  ( v -> origin  !=  o_override )
		   {
			struct variable *	gv ;
			int			len	=  strlen ( v -> name ) ;

			gv	=  lookup_variable ( v -> name, len ) ;

			if  ( gv  &&  v  !=  gv  &&
				( gv -> origin  ==  o_env_override  ||  gv -> origin  ==  o_command ) )
			   {
				free  ( v -> value ) ;

				v -> value	=  xstrdup ( gv -> value ) ;
				v -> origin	=  gv -> origin ;
				v -> recursive	=  gv -> recursive ;
				v -> append	=  0 ;
			    }
		    }
	    }
    }


/*==============================================================================================================

    record_files -
        Record a description line for files FILENAMES, with dependencies DEPS, commands to execute described
	by COMMANDS and COMMANDS_IDX, coming from FILENAME:COMMANDS_STARTED.
	TWO_COLON is nonzero if a double colon was used.
	If not nil, PATTERN is the '%' pattern to make this a static pattern rule, and PATTERN_PERCENT is a pointer
	to the '%' within it.

	The links of FILENAMES are freed, and so are any names in it that are not incorporated into other data 
	structures.

  ==============================================================================================================*/
void	record_files ( struct nameseq *		filenames, 
		       const char *		pattern,
		       const char *		pattern_percent, 
		       char *			depstr,
		       unsigned int		cmds_started, 
		       char *			commands,
		       unsigned int		commands_idx, 
		       int			two_colon,
		       char			prefix, 
		       const gmk_floc *		flocp,
		       unsigned int		grouped )
    {
	struct commands *	cmds ;
	struct dep *		deps ;
	const char *		implicit_percent ;
	const char *		name ;


	/* If we've already snapped deps, that means we're in an eval being resolved after the makefiles have been read in.	*/
	/* We can't add more rules at this time, since they won't get snapped and we'll get core dumps.				*/
	/* See Savannah bug # 12124.												*/
	if  ( snapped_deps )
		O ( fatal, flocp, _( "prerequisites cannot be defined in recipes" ) ) ;

	/* Determine if this is a pattern rule or not.  */
	name			=  filenames -> name ;
	implicit_percent	=  find_percent_cached ( & name ) ;

	/* If there's a recipe, set up a struct for it.  */
	if  ( commands_idx  >  0 )
	   {
		format_commands ( commands ) ;

		cmds				=  xmalloc ( sizeof ( struct commands ) ) ;
		cmds -> fileinfo.filenm		=  flocp -> filenm ;
		cmds -> fileinfo.lineno		=  cmds_started ;
		cmds -> commands		=  xstrndup (commands, commands_idx ) ;
		cmds -> command_lines		=  0 ;
		cmds -> recipe_prefix		=  prefix ;
		cmds -> grouped_commands	=  grouped ;
	    }
	else
		cmds	=  0 ; 

	/* If there's a prereq string then parse it--unless it's eligible for 2nd expansion: if so, snap_deps() will do it.  */
	if  ( depstr  ==  0 )
		deps	=  0 ;
	else
	   {
		depstr	=  unescape_char (depstr, ':' ) ;

		if  ( second_expansion  &&  strchr ( depstr, '$' ) )
		   {
			deps				=  alloc_dep ( ) ;
			deps -> name			=  depstr ;
			deps -> need_2nd_expansion	=  1 ;
			deps -> staticpattern		=  ( pattern  !=  0 ) ;
		    }
		else
		   {
			deps	=  split_prereqs ( depstr ) ;
			free ( depstr ) ;

			/* We'll enter static pattern prereqs later when we have the stem.				*/
			/* We don't want to enter pattern rules at all so that we don't	think that they ought to exist	*/
			/* (make manual "Implicit Rule Search Algorithm", item 5c).					*/
			if  ( ! pattern  &&  ! implicit_percent )
				deps	=  enter_prereqs ( deps, NULL ) ;
		    }
	    }

	/* For implicit rules, _all_ the targets must have a pattern.  That means we can test the first one to see	*/
	/* if we're working with an implicit rule; if so we handle it specially.					*/
	if  ( implicit_percent )
	   {
		struct nameseq *	nextf ;
		const char **		targets, 
			   **		target_pats ;
		unsigned int		c ;

		if  ( pattern  !=  0 )
			O ( fatal, flocp, _( "mixed implicit and static pattern rules" ) ) ;

		/* Count the targets to create an array of target names. We already have the first one.  */
		nextf		=  filenames -> next ;
		free_ns ( filenames ) ;
		filenames	=  nextf ;

		for  ( c = 1 ; nextf ; ++c, nextf = nextf -> next )
			;

		targets		=  xmalloc ( c * sizeof ( const char * ) ) ;
		target_pats	=  xmalloc ( c * sizeof ( const char * ) ) ;

		targets [0]	=  name ;
		target_pats [0] =  implicit_percent ;

		c		=  1 ;

		while ( filenames )
		   {
			name			=  filenames -> name ;
			implicit_percent	=  find_percent_cached ( & name ) ;

			if  ( implicit_percent  ==  0 )
				O ( fatal, flocp, _( "mixed implicit and normal rules" ) ) ;

			targets [c]		=  name ;
			target_pats [c]		=  implicit_percent ;
			c++ ;

			nextf			=  filenames -> next ;
			free_ns ( filenames ) ;
			filenames		=  nextf ;
		    }

		create_pattern_rule (targets, target_pats, c, two_colon, deps, cmds, 1) ;

		return ;
	    }


	/* Walk through each target and create it in the database. We already set up the first target, above.  */
	while (1)
	   {
		struct nameseq *	nextf	=  filenames -> next ;
		struct file *		f ;
		struct dep *		this	=  0 ;

		free_ns ( filenames ) ;

		/* Check for special targets.  Do it here instead of, say, snap_deps() so that we can immediately use the value.  */
		if  ( streq ( name, ".POSIX" ) )
		   {
			posix_pedantic	=  1 ;

			define_variable_cname ( ".SHELLFLAGS"	, "-ec"		, o_default, 0 ) ;

			/* These default values are based on IEEE Std 1003.1-2008.  */
			define_variable_cname ("ARFLAGS"	, "-rv"		, o_default, 0 ) ;
			define_variable_cname ("CC"		, "c99"		, o_default, 0 ) ;
			define_variable_cname ("CFLAGS"		, "-O"		, o_default, 0 ) ;
			define_variable_cname ("FC"		, "fort77"	, o_default, 0 ) ;
			define_variable_cname ("FFLAGS"		, "-O 1"	, o_default, 0 ) ;
			define_variable_cname ("SCCSGETFLAGS"	, "-s"		, o_default, 0 ) ;
		    }
		else if  ( streq ( name, ".SECONDEXPANSION" ) )
			second_expansion	=  1 ;

# if	! defined ( __MSDOS__ )  &&  ! defined ( __EMX__ )
		else if  ( streq ( name, ".ONESHELL" ) )
			one_shell	= 1 ;
# endif
		else if  ( cmds  &&  cmds -> grouped_commands )
			one_shell	=  0 ;

		/* If this is a static pattern rule:							*/
		/*	'targets: target%pattern: prereq%pattern; recipe',				*/
		/* make sure the pattern matches this target name.					*/
		if  ( pattern  &&  ! pattern_matches ( pattern, pattern_percent, name ) )
			OS ( error, flocp, _( "target '%s' doesn't match the target pattern" ), name ) ;
		/* If there are multiple targets, copy the chain DEPS for all but the last one.		*/
		/* It is not safe for the same deps to go in more than one place in the database.	*/
		else if  ( deps )
			this	=  ( nextf  != 0 ) ?  copy_dep_chain ( deps ) : deps ;

		/* Find or create an entry in the file database for this target.  */
		if  ( ! two_colon )
		   {
			/* Single-colon.  Combine this rule with the file's existing record, if any.	*/
			f	=  enter_file ( strcache_add ( name ) ) ;

			if  ( f -> double_colon )
				OS ( fatal, flocp, _( "target file '%s' has both : and :: entries" ), f -> name ) ;

			/* If CMDS == F->CMDS, this target was listed in this rule more than once.	*/
			/* Just give a warning since this is harmless.					*/
			if  ( cmds  !=  0  &&  cmds  ==  f -> cmds )
				OS ( error, flocp, _( "target '%s' given more than once in the same rule" ), f->name ) ;

			/* Check for two single-colon entries both with commands.						*/
			/* Check is_target so that we don't lose on files such as .c.o whose commands were preinitialized.	*/
			else if  ( cmds  !=  0  &&  f -> cmds  !=  0  &&  f -> is_target )
			   {
				size_t		l	=  strlen ( f -> name ) ;

				error ( & cmds -> fileinfo, l, _( "warning: overriding recipe for target '%s'" ), f -> name ) ;
				error ( & f -> cmds -> fileinfo, l, _("warning: ignoring old recipe for target '%s'"), f->name ) ;
			    }

			/* Defining .DEFAULT with no deps or cmds clears it.  */
			if  ( f  ==  default_file  &&  this  ==  0  &&  cmds  ==  0 )
				f -> cmds	=  0 ;

			if  ( cmds  !=  0 )
				f -> cmds	=  cmds ;

			/* Defining .SUFFIXES with no dependencies clears out the list of suffixes.  */
			if  ( f  ==  suffix_file  &&  this  ==  0 )
			   {
				free_dep_chain ( f -> deps ) ;
				f -> deps = 0 ;
			    }
		    }
		else
		    {
			/* Double-colon.  Make a new record even if there already is one.  */
			f	=  lookup_file ( name ) ;

			/* Check for both : and :: rules.  Check is_target so we don't lose on default suffix rules or makefiles.  */
			if  ( f  !=  0  &&  f -> is_target  &&  ! f -> double_colon )
				OS ( fatal, flocp, _("target file '%s' has both : and :: entries" ), f -> name ) ;

			f	=  enter_file ( strcache_add ( name ) ) ;

			/* If there was an existing entry and it was a double-colon entry, enter_file will have returned a new one,	*/
			/* making it the prev pointer of the old one, and setting its double_colon pointer to the first one.		*/
			if  ( f -> double_colon  ==  0 ) 
				/* This is the first entry for this name, so we must set its double_colon pointer to itself.  */
				f -> double_colon	=  f ;

			f -> cmds	=  cmds ;
		    }

		f -> is_target	=  1 ;

		/* If this is a static pattern rule, set the stem to the part of its name that matched the '%' in the pattern,	*/
		/* so you can use $* in the commands.  If we didn't do it before, enter the prereqs now.			*/
		if  ( pattern )
		   {
			static const char *		percent		=  "%" ;
			char *				buffer		=  variable_expand ( "" ) ;
			char *				o		=  patsubst_expand_pat (buffer, name, pattern, percent,
										pattern_percent + 1, percent + 1 ) ;

			f -> stem	=  strcache_add_len ( buffer, o - buffer ) ;

			if  ( this )
			    {
				if  ( ! this -> need_2nd_expansion )
					this		=  enter_prereqs ( this, f -> stem ) ;
				else
					this -> stem	=  f -> stem;
			    }
		    }

		/* Add the dependencies to this file entry.  */
		if  ( this  !=  0 )
		   {
			/* Add the file's old deps and the new ones in THIS together.  */
			if  ( f -> deps  ==  0 )
				f -> deps	=  this ;
			else if  ( cmds  !=  0 )
			   {
				struct dep *	d	=  this ;

				/* If this rule has commands, put these deps first.  */
				while  ( d -> next  !=  0 )
					d	=  d -> next ;

				d -> next	=  f->deps ;
				f -> deps	=  this ;
			    }
			else
			   {
				struct dep *	d	=  f -> deps ;

				/* A rule without commands: put its prereqs at the end.  */
				while  ( d -> next  !=  0 )
					d	=  d -> next ;

				d -> next	=  this ;
			    }
		    }

		name	=  f -> name ;

		/* All done!  Set up for the next one.  */
		if  ( nextf == 0 )
			break ;

		filenames	=  nextf ;

		/* Reduce escaped percents.  If there are any unescaped it's an error  */
		name	=  filenames -> name ;

		if  ( find_percent_cached ( & name ) )
			O ( error, flocp, _( "*** mixed implicit and normal rules: deprecated syntax" ) ) ;
	    }
    }

# endif		/*  WAKE_EXTENSIONS  */