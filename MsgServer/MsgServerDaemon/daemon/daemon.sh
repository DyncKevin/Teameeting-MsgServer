#!/bin/bash
# 
# zhengxiaokai@dync.cc
# /etc/rc.d/init.d/MsgServerDaemon
# init script for MsgServerDaemon processes
#
# processname: MsgServerDaemon
# description: MsgServerDaemon is a client daemon.
# chkconfig: 2345 99 10
#
if [ -f /etc/init.d/functions ]; then
	. /etc/init.d/functions
elif [ -f /etc/rc.d/init.d/functions ]; then
	. /etc/rc.d/init.d/functions
else
	echo -e "\aDaemon: unable to locate functions lib. Cannot continue."
	exit -1
fi

ulimit -n 65000
echo 1024 65000 > /proc/sys/net/ipv4/ip_local_port_range

#---------------------------------------------------------------------------
# GLOBAL
#---------------------------------------------------------------------------

RT_HOME=/usr/local/dync/msgserver/daemon/bin
RT_CONF=/usr/local/dync/msgserver/daemon/conf
RT_LIB=/usr/local/dync/msgserver/daemon/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RT_LIB}
retval=0

#---------------------------------------------------------------------------
# START
#---------------------------------------------------------------------------
Start()
{
	if [ $( pidof -s MsgServerDaemon ) ]; then
		echo -n "MsgServerDaemon process [${prog}] already running"
		echo_failure
		echo
		return -1;
	fi
		
	if [ ! -x ${RT_HOME}/MsgServerDaemon ]; then
		echo -n "MsgServerDaemon binary [${prog}] not found."
		echo_failure
		echo
		return -1
	fi
		
	echo -n "Starting Daemon Server(MsgServerDaemon): "
	${RT_HOME}/MsgServerDaemon ${RT_CONF}/daemon.conf ${RT_CONF}/params.conf & 2> /dev/null
	retval=$?
	if [ ${retval} == 0 ]; then
		echo_success
		echo
	else
		echo_failure
		echo
		break
	fi
	sleep 1
	
	return 0
}

#---------------------------------------------------------------------------
# STOP
#---------------------------------------------------------------------------
Stop()
{
	echo -n "Stopping Daemon Server(MsgServerDaemon): "
	killproc MsgServerDaemon
    killproc MsgServerConnector
    killproc MsgServerDispatcher
    killproc MsgServerRTLive
    killproc MsgServerLogical
    killproc MsgServerSequence
    killproc MsgServerStorage
    killproc MsgServerGrouper
    killproc MsgServerPusher
	echo
	return 0
}

#---------------------------------------------------------------------------
# MAIN
#---------------------------------------------------------------------------
case "$1" in
	start)
		Start
		;;
	stop)
		Stop
		;;
	restart)
		Stop
		sleep 5
		Start
		;;
	*)
		echo "Usage: $0 {start|stop|restart}"
esac

exit 0

