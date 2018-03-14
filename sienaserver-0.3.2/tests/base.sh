#!/bin/sh
#
#  This file is part of Siena, a wide-area event notification system.
#  See http://www.cs.colorado.edu/serl/dot/siena.html
#
#  Author: Antonio Carzaniga <carzanig@cs.colorado.edu>
#  See the file AUTHORS for full details. 
#
#  Copyright (C) 1998-1999 University of Colorado
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
#  USA, or send email to serl@cs.colorado.edu.
#
# $Id: base.sh,v 1.1 2002/06/10 20:19:15 carzanig Exp $
#

TESTDIR=.
TMPDIR=/tmp
#
client=$TESTDIR/client
sender=$TESTDIR/sender
compare=$TESTDIR/compare
covers=$TESTDIR/covers
#
serverhost=${SIENASERVER:-127.0.0.1}
clienthost=${SIENACLIENT:-127.0.0.1}
clientport=7001
#
clientpid=$TESTDIR/client-${clienthost}:${clientport}.pid
clientlog=$TESTDIR/client-${clienthost}:${clientport}.out
senderlog=$TESTDIR/sender-${clienthost}:${clientport}.out
clientexpect=$TESTDIR/client-${clienthost}:${clientport}.expect
senderexpect=$TESTDIR/sender-${clienthost}:${clientport}.expect
#

#  it is fair to add a 1 second pause on most platform.  On slow machines, 
#  some tests might fail due to slow server responses, for instance a 
#  subscription might not be processed in time so one or more notifications
#  might get lost.  In these cases, try increasing the pause value to 2-5 
#  seconds. 
# 
pause=1
#
ssh=ssh
#
# folowing check borrowed from autoconf
#
if (echo "testing\c"; echo 1,2,3) | grep c >/dev/null; then
  # Stardent Vistra SVR4 grep lacks -e, says ghazi@caip.rutgers.edu.
  if (echo -n testing; echo 1,2,3) | sed s/-n/xn/ | grep xn >/dev/null; then
    ac_n= ac_c='
'
  else
    ac_n=-n ac_c= 
  fi
else
  ac_n= ac_c='\c'
fi
#
echon() {
    echo $ac_n "$*$ac_c"
}
#
#  startcmd_remote() {
#      if test $# -le 2; then return; fi
#      host=$1
#      shift
#      $ssh $host /homes/carzanig/siena/c++/test/startcmd.sh $* 
#  }
#  startcmd() {
#      /homes/carzanig/siena/c++/test/startcmd.sh $*
#  }
#
localhost=`hostname`
#
topology_kill=$TESTDIR/topology.kill
topology_info=$TESTDIR/topology.info
#
#
ref=0
incref() {
    ref=`expr $ref + 1`
#    ref=$[$ref + 1]
    echo $ac_n ".$ac_c"
    case "X$pause" in X) ;; *) sleep $pause;; esac
}
#
closetest() {
#
# terminate servers and client
#
    sleep 1
    test -r $clientpid && xargs kill < $clientpid && rm -f $clientpid
#
#
# now verifies and returns the results
#
    echo 
    echon checking results...
    #$compare $clientlog $clientexpect > $clientlog.diff
    clientok=$?
    if test "$clientok" = 0; 
    then 
	echo PASSED
	#rm -f $senderexpect $senderlog $clientexpect $clientlog
	if test "$1" != notopology; 
	then 
	    test -r $topology_kill && xargs kill < $topology_kill && rm -f $topology_kill
	fi
	exit 0
    else
	echo FAILED
	echo see $clientlog.diff
	test "$1" != notopology || echo Servers still active.
	exit 1
    fi
}
#
opentest() {
    test $topology_id = 0 && topology_id=$1
    test "$2" = notopology || $TESTHOME/topology$topology_id.sh $2
    rm -f $clientlog $senderlog $clientexpect $senderexpect
    #
    # starts client
    #
    echon starting client...
    test -r $clientpid && kill `cat $clientpid` > /dev/null 2>&1
    $client -port $clientport -detach -o $clientlog > $clientpid
    if test "$?" != 0; 
    then 
	echo FAILED
	exit 1
    else
	echo Ok
    fi
    sleep 1
    client=senp://${clienthost}:${clientport}
    if test -r $topology_info;
    then
	. $topology_info
    else
	exit 1
    fi
}
#
send() {
    $sender >> $senderlog 2>&1
    case "$?" in 0) ;; *) echo sender error ref=$ref;; esac
}
#
expect() {
    cat >> $clientexpect
}
#
testcovers () {
    echo $ac_n ".$ac_c"
    $covers -f "$2"
    case "$1$?" in
	yes0 | no1) # ok 
	return 0
	;;
	*2)
	echo
	echo syntax error: "$*"
	exit 1
	;;
	*)
	echo
	echo "wrong covering: expecting $1 for $2"
	exit 1
    esac
}
#
shutdownservers() {
if test -r $topology_kill;
then
    if test "$1" = "force";
    then 
	echon shutting down running servers...
	xargs -n 1 kill < $topology_kill > /dev/null 2>&1
	#rm -f $topology_kill
	sleep 3
	echo Ok
    else
	echo servers already running.
	exit 0
    fi
#    rm -f $topology_info
fi
}
#
# startserver id host port [connect]
#
startserver() {
id=$1
shift
host=$1
shift
port=$1
shift
echon starting server senp://${host}:${port} $* ...
$server -detach -log "log.txt" -port $port -identity senp://${host}:${port} $* >> $topology_kill
sleep 2
if test $? != 0;
then
    echo FAILED
    rm -f $topology_info
    shutdownservers force
    exit 1
fi
echo "siena$id='senp://${host}:${port}'" >> $topology_info
echo Ok
}
#
dumpstate() {
$sender <<EOF
senp{to="$1" method="DBG"}
EOF
}
sighandler() {
    shutdownservers force
    exit 1
}
printusage() {
    echo $1 
}
trap sighandler 15 2 
topology_id=0
persistent_servers=false
for option 
do
    case "$option" in 
	--help | -h )
	printusage $0
	;;
	--persist )
	persistent_servers=true
	;;
	-*=* )
	opt_value=`echo "$option" | sed 's/[-_a-zA-Z0-9]*=//'`
	case "$option" in
	    --topology=* )
	    topology_id=$opt_value
	    ;;
	    --servers=* )
	    servers_num=$opt_value
	    ;;
	    *)
	    printusage $0
	    ;;
	esac
	;;
	*)
	printusage $0
	;;
    esac
done
