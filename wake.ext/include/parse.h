/**************************************************************************************************************

    NAME
        parse.h

    DESCRIPTION
        Parsing definitions.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/06]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifdef		WAKE_EXTENSIONS
# ifndef	__PARSE_H__
#	define	__PARSE_H__


# include		<assert.h>
# include		<glob.h>

# include		"makeint.h"
# include		"filedef.h"
# include		"dep.h"
# include		"job.h"
# include		"commands.h"
# include		"variable.h"
# include		"rule.h"
# include		"debug.h"
# include		"hash.h"


# ifdef WINDOWS32
#	include		<windows.h>
#	include		"sub_proc.h"
# else
#	ifdef  _AMIGA
		struct passwd *		getpwnam  ( char *  name ) ;
#	elifdef  VMS
		struct passwd *		getpwnam  ( char *  name ) ;
#	else
#		include		<pwd.h>
#	endif
# endif	


/*==============================================================================================================

	Macros & constants

  ==============================================================================================================*/

// Wake extensions : multiline comments
# define	MULTILINE_COMMENT_START		"#/*"
# define	MULTILINE_COMMENT_END		"*/#" 

// Wake extensions : here document
# define	HEREDOC_PREFIX			"<<"
# define	IS_HEREDOC_CHAR(c)		( c  ==  '<' )
# define	IS_HEREDOC_KEYWORD_CHAR(c)	( ! IS_HEREDOC_CHAR ( c )  &&  ! isspace ( ( int ) c ) )

// Multiline construct types
# define	MULTILINE_NONE			0
# define	MULTILINE_BACKSLASH		1
# define	MULTILINE_COMMENT		2
# define	MULTILINE_HEREDOC		3

// readbuffer related constants
# define	READBUFFER_SIZE			256
# define	READBUFFER_INCREMENT		256

/* Compares a word, both length and contents. P must point to the word to be tested, and LEN must be the length. */
# define	word1eq(keyword, p, len)	( len  ==  CSTRLEN ( keyword )  &&  strneq ( keyword, p, CSTRLEN ( keyword ) ) )


/*==============================================================================================================

	Data types

  ==============================================================================================================*/

/***
	readbuffer : Holds the current location within a makefile during evaluation.
 ***/
typedef struct readbuffer
  {
	char *		buffer ;		/* Start of the current line in the buffer.		*/
	char *		bufnext ;		/* Start of the next line in the buffer.		*/
	char *		bufstart ;		/* Start of the entire buffer.				*/
	unsigned int	size ;			/* Malloc'd size of buffer.				*/
	FILE *		fp ;			/* File, or NULL if this is an internal buffer.		*/
	gmk_floc	floc ;			/* Info on the file in fp (if any).			*/
   }  readbuffer ;


/***
	Heredoc data, as parsed by readline()
 ***/
typedef struct heredocdata
   {
	int		heredoc_start ;		/* Pointer to the "<<" construct			*/
	int		start,			/* heredoc contents first and last characters		*/
	     		end ;
	char *		keyword ;		/* Keyword after the "<<" heredoc construct		*/
	char *		line ;			/* Whole line						*/
    }  heredocdata ;	


/***
	String buffer, where individual strings are separated by newlines.
 ***/
typedef struct stringbuffer
   {
	char *		data ;
	char *		next ;
	char		separator ;
    } stringbuffer ;


/*** 
	Track the modifiers we can have on variable assignments 
 ***/
typedef struct variable_modifiers
   {
	unsigned int	vm_assign:1 ;
	unsigned int	vm_define:1 ;
	unsigned int	vm_undefine:1 ;
	unsigned int	vm_export:1 ;
	unsigned int	vm_override:1 ;
	unsigned int	vm_private:1 ; 
    }  variable_modifiers ;


/*** 
	Types of "words" that can be read in a makefile.  
 ***/
typedef enum makewordtype
   {
	wt_bogus, 
	wt_eol, 
	wt_static, 
	wt_variable, 
	wt_colon, 
	wt_dcolon, 
	wt_semicolon,
	wt_varassign
    }  makewordtype ;


/***
	A 'struct conditionals' contains the information describing all the active conditionals in a makefile.

	The global variable 'conditionals' contains the conditionals information for the current makefile.  
	It is initialized from the static structure 'toplevel_conditionals' and is later changed to new 
	structures for included makefiles.  
 ***/
typedef struct conditional_directives
   {
	unsigned int	if_cmds ;		/* Depth of conditional nesting.  */
	unsigned int	allocated ;		/* Elts allocated in following arrays.  */
	char *		ignoring ;		/* Are we ignoring or interpreting?
							0=interpreting, 
							1=not yet interpreted,
							2=already interpreted 
						 */
	char *		seen_else ;            /* Have we already seen an 'else'?  */
    } conditional_directives ;


/*** 
	Conditional types.
 ***/
typedef enum  conditional_type
   { 
	ct_ifdef, ct_ifndef, ct_ifeq, ct_ifneq, ct_else, ct_endif 
    } conditional_type ;


/*==============================================================================================================

	Parse variables.

  ==============================================================================================================*/
extern conditional_directives		toplevel_conditionals ;
extern conditional_directives *		conditionals ;
extern struct dep *			read_files ;
extern const char **			include_directories ;
extern const char *			default_makefiles [] ;


/*==============================================================================================================

	Parse functions.

  ==============================================================================================================*/
extern int			eval_makefile			( const char *			filename, 
								  int				flags ) ;
extern void			eval				( readbuffer *			buffer, 
								  int				flags ) ;

extern long			readline			( readbuffer *			ebuf,
								  heredocdata **		ehdata ) ;
extern unsigned long		readstring			( readbuffer *			ebuf ) ;

extern void			do_undefine			( char *			name, 
								  enum variable_origin		origin,
								  readbuffer *			ebuf ) ;
extern struct variable *	do_define			( char *			name, 
								  enum variable_origin		origin,
								  readbuffer *			ebuf ) ;

extern int			conditional_line		( char *			line, 
								  int				len, 
								  const gmk_floc *		flocp ) ;

extern void			record_files			( struct nameseq *		filenames, 
								  const char *			pattern,
								  const char *			pattern_percent, 
								  char *			depstr,
								  unsigned int			cmds_started, 
								  char *			commands,
								  unsigned int			commands_idx, 
								  int				two_colon,
								  char				prefix, 
								  const gmk_floc *		flocp,
								  unsigned int			grouped ) ;
extern void			record_target_var		( struct nameseq *		filenames, 
								  char *			defn,
								  enum variable_origin		origin,
							          variable_modifiers *		vmod,
								  const gmk_floc *		flocp ) ;

extern makewordtype		get_next_makeword		( char *			buffer, 
								  char *			delim,
								  char **			startp, 
								  unsigned int *		length ) ;
extern void			remove_comments			( char *			line ) ;
extern char *			find_char_unquote		( char *			string, 
								  int				map ) ;
extern char *			unescape_char			( char *			string, 
								  int				c ) ;
extern void			format_commands			( char *			text ) ;

extern char *			parse_var_assignment		( const char *			line, 
								  variable_modifiers *		vmod ) ;

extern void			readbuffer_alloc		( readbuffer *			ebuf,
								  int				size ) ;
extern void			readbuffer_grow			( readbuffer *			ebuf,
								  int				increment ) ;

extern conditional_directives *	install_conditionals		( conditional_directives *	new ) ;
extern void			restore_conditionals		( conditional_directives *	saved ) ;

extern char *			transform_heredoc		( char *			line,
								  heredocdata *			ehdata,
								  int				include_backslash ) ;
extern void			replace_heredoc			( char *			text,
								  heredocdata *			heredoc,
								  char				old,
								  char				new ) ;

extern void			stringbuffer_init		( stringbuffer *		sb,
								  char *			line,
								  char				separator ) ;
extern void 			stringbuffer_reset		( stringbuffer *		sb ) ;
extern char *			stringbuffer_next		( stringbuffer *		sb ) ;
extern int			stringbuffer_empty		( stringbuffer *		sb ) ;
extern int			stringbuffer_lines		( stringbuffer *		sb ) ;


# endif		/*  __PARSE_H__  */
# endif		/*  WAKE_EXTENSIONS */
