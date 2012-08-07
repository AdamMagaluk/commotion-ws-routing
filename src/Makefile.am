AUTOMAKE_OPTIONS = subdir-objects gnu
AM_CFLAGS = -Wall -g
DATE=`date -R`
AM_CPPFLAGS = -I $(top_srcdir)/src/ -DBUILD_DATE=\"`date '+%Y%m%d-%H:%M'`\" -DHG_VERSION=\"`hg id | sed 's/ /_/g'`\"
AM_LDFLAGS = 
ACLOCAL_AMFLAGS = -I m4 

lib_LTLIBRARIES = libcommotionws.la
libcommotionws_la_SOURCES = commotion.c \
                            topology.c

bin_PROGRAMS = wsroutingserver test_topology
wsroutingserver_SOURCES = main.c
wsroutingserver_LDFLAGS = -lcommotionws

test_topology_SOURCES = test-topology.c
test_topology_LDFLAGS = -lcommotionws 

