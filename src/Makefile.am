AUTOMAKE_OPTIONS = subdir-objects gnu
AM_CFLAGS = -Wall -g
DATE=`date -R`
AM_CPPFLAGS = -I $(top_srcdir)/src/ -DBUILD_DATE=\"`date '+%Y%m%d-%H:%M'`\" -DHG_VERSION=\"`hg id | sed 's/ /_/g'`\"
AM_LDFLAGS = 
ACLOCAL_AMFLAGS = -I m4 

bin_PROGRAMS = wsroutingserver
wsroutingserver_SOURCES = main.c \
		          commotion.c \
			  topology.c
 

