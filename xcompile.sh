#! /bin/bash
#
# @(!--#) @(#) xcompile.sh, sversion 0.1.0, fversion 001, 26-october-2023";
#
# cross compile using the nas1 RedHat 7.9 node on the local LAN
#

CPROG=multish.c
BNAME=`basename $CPROG .c`
CHOST=nas1
CUSER=andyc

scp $CPROG ${CUSER}@${CHOST}:/var/tmp/$CPROG

sshcmds -h $CHOST -u $CUSER -p [PASS1] -l "cd /var/tmp ; gcc -o $BNAME $CPROG"

scp ${CUSER}@${CHOST}:/var/tmp/$BNAME ${BNAME}.rh7

cp ${BNAME}.rh7 "/mnt/c/Users/wc1034/Downloads"

exit 0
