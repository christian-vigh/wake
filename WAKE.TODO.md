- Package Wake for the following platforms :
	- Windows 7 and later, using Visual Studio 2010 up to... ?
	- Linux Debian
- Put object files into their source directory, not in the wake root directory
- Run here documents for make rules as a separate script : this will require a new construct\_command\_argv() substitute in job.c
- Implement the following functions :
	- Checksum functions :
		- For values : < crc16>, < crc32>, < crc64>, < md5>, < sha1> etc.
		- For file contents : < crc16f>, < crc32f>, < crc64f>, < md5f>, < sha1f> etc.
	- File functions : TBD