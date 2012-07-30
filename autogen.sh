#!/bin/sh

mkdir -p m4
rm -R aclocal.m4 autom4te.cache 
rm install-sh ltmain.sh depcomp
rm config.*
rm -R missing
mkdir m4
rm -R m4/*


autoreconf --force --install --verbose

