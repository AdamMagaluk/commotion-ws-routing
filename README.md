Commotion Websocket Messaging Service

A websocket server that allows routing of websockets between clients connected to multiple
commotion aps.

Version: 0.0.1 ()
Tagline: 


Unix Installation
=================
You must use GNU `make' to build SWIG.

http://www.gnu.org/software/make/

PCRE needs to be installed on your system to build SWIG, in particular
pcre-config must be available. If you have PCRE headers and libraries but not
pcre-config itself or, alternatively, wish to override the compiler or linker
flags returned by pcre-config, you may set PCRE_LIBS and PCRE_CFLAGS variables
to be used instead. And if you don't have PCRE at all, the configure script
will provide instructions for obtaining it.

To build and install , simply type the following:

     % ./configure
     % make
     % make install

By default SWIG installs itself in /usr/local.  If you need to install SWIG in
a different location or in your home directory, use the --prefix option
to ./configure.  For example:

     % ./configure --prefix=/home/yourname/projects
     % make
     % make install

Note: the directory given to --prefix must be an absolute pathname.  Do *NOT* use
the ~ shell-escape to refer to your home directory.  SWIG won't work properly
if you do this.

The file INSTALL details more about using configure. Also try

     % ./configure --help.
