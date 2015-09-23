<<<<<<< HEAD
# WHAT IS WAKE ? #
**wake** is a build utility based on GNU **make**. It provides a bunch of additional features, such as new command-line options, tenths of new functions (ie, dynamic variables) and some enhancements to the existing **make** utility.

# WHY YET ANOTHER MAKE UTILITY ? #
I'm using **make** since the late 80's ; during all that time, while designing new Makefiles for my projects, I can recall that I said to myself at least a thousand times : "*Oh, it would really be comfortable if the make utility could provide me with this and that feature*".

So one day, I decided to really implement all the ideas I had had during all these years on top of the **make** utility. This is how **wake** was born.

The starting point was that in one of my projects, I needed a variable that increments each time it was expanded. It was a good reason for me to implement all of the above.

The final reason for yet anoter make utility is : for the fun !

# WHAT IS THE CURRENT VERSION ? #
The current version of **wake** is 1.0.0, based on GNU make 4.1.
See the [WAKE.HISTORY.md](WAKE.HISTORY.md "Wake history") file for more information on release contents.

# SUPPORTED ENVIRONMENTS #
For the moment, Wake has been compiled and tested in the following environments :

- Windows 7 64-bits, using Cygwin 64-bits

There is a plan to package Wake on pure Windows and Linux platforms.

# WHAT ARE THE MAIN FEATURES OF WAKE ?
**wake** provides the following extensions to the existing **make** command (for more detailed information, have a look at the [WAKE.help.md](WAKE.help.md "Wake help file") help file).

- Core : 
	- The concept of "here documents" (such as it can be found in shell or php scripts) has been introduced ; You can, for example, define variables using here documents :

			TARGETS 	=  <<END
						main.c
						utilities.c
						glob.c
			END
	
		Note that here documents are also implemented for rules. The idea is to group rule commands into a single shell script before execution. However, in the current version, this feature is partially functional ; it works for variable definitions, but does not have the intended functionnality for rules, which was to group commands within a shell script before executing them.

	- Multiline comments : they are introduced by the sequence "\#/\*" and terminated by "*/#".
	
	- Variable names support a wider range of characters, including the following ones :

		<>.[]~-_&+=/%*|^!

	- Regarding the C language, the C preprocessor and the make utility (and others), I always hated the fact that the backslash character, used to introduce continuation lines, would have to be followed by a newline character, and nothing else. I spent too many time rerunning a compilation, because there was a continuation line in which spaces where present between the backslash and the newline ; was it so hard to consider spaces after a backslash as inert characters until a newline is met ? this is why **wake** allows for spaces and tabs between the backslash of the continuation line and the newline character. 

- A bunch of extension functions that provide additional features for :
	- String manipulation
	- List manipulation
	- Arithmetic functions
	- Logical functions
	- ... and various other functions, such as defining an auto-increment counter, generating a GUID, etc.