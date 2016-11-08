#!/bin/sh

############find . -name "*.Po" | xargs rm -rf
############find . -name "*.bz2" | xargs rm -rf

##############################################################
#####################    usage    ############################
##############################################################
function shell_usage()
{
    echo "################## USAGE: $0 [-cflh]"
    sleep 1
    echo "################## USAGE: -c [yes/no] clean"
    sleep 1
    echo "################## USAGE: -f [yes/no] force config"
    sleep 1
    echo "################## USAGE: -l [file name] log file name"
    sleep 1
    echo "################## USAGE: -h help usage"
    sleep 1
    exit
}

##############################################################
###################    parameter    ##########################
##############################################################
CUR_PATH=`pwd`
BASE_PATH=$CUR_PATH/..
if [ ! -d $BASE_PATH/MsgServer/MSCommonLib ]
then
    mkdir -p $BASE_PATH/MsgServer/MSCommonLib
fi

CUR_DATE=$(date +%Y-%m-%d-%H-%M-%S)

SRC_PATH=
BUILD_NAME=
PREFIX_PATH=
THIRDPARTY_LIB_PATH=

export PARAM_FORCE="no"
export PARAM_CLEAN="no"
export PARAM_LOG=""
export LOG_FILE=""
PARAM_LOGNAME=$CUR_DATE

while getopts "cfl:h" arg
do
    case $arg in
        c)  PARAM_CLEAN="yes";;
        f)  PARAM_FORCE="yes";;
        l)  PARAM_LOGNAME=$OPTARG
            PARAM_LOG=$OPTARG;;
        h)  shell_usage $0;;
        ?)  shell_usage $0;;
    esac
done

echo "param_clean:" $PARAM_CLEAN
echo "param_force:" $PARAM_FORCE
echo "param_logname:" $PARAM_LOGNAME
echo "param_log:" $PARAM_LOG

if [ "$PARAM_LOG"x != ""x ]
then
    LOG_FILE=$CUR_PATH/log/log-$PARAM_LOGNAME
    touch $LOG_FILE
    echo "need log"
fi
echo "LOG_FILE: " $LOG_FILE

##############################################################
###################    function    ###########################
##############################################################

###################log to file ###############################
function log2f()
{
    if [ "$PARAM_LOG"x = ""x ]
    then
        echo -e "PLEASE GIVE THE LOG NAME"
        echo -e "IF YOU INVOKE function log2f"
        shell_usage
    fi
    if [ $# -eq 0 ]; then
        echo -e "[LOG]Date: $CUR_DATE" >> $LOG_FILE
    elif [ $# -eq 1 ]; then
        echo -e "[LOG]$CURTIME: $1" >> $LOG_FILE
    elif [ $# -eq 2 ]; then
        echo -e "[LOG]$CURTIME: $1 $2" >> $LOG_FILE
    else
        echo -e "just support two params hahaha~~~"
    fi
}
export -f log2f

###################log info ###############################
function loginfo()
{
    if [ $# -eq 0 ]; then
        echo -e "\033[1;34m [INFO]Date: $CUR_DATE \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]Date: $CUR_DATE" >> $LOG_FILE
        fi
    elif [ $# -eq 1 ]; then
        echo -e "\033[1;34m [INFO]$CUR_DATE: $1 \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]$CURTIME: $1" >> $LOG_FILE
        fi
    elif [ $# -eq 2 ]; then
        echo -e "\033[1;34m [INFO]$CUR_DATE: $1 $2 \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]$CURTIME: $1 $2" >> $LOG_FILE
        fi
    else
        echo -e "\033[1;34m just support two params hahaha~~~ \033[0m"
    fi
}
export -f loginfo

###################log error ###############################
function logerr()
{
    if [ $# -eq 0 ]; then
        echo -e "\033[0;31m [ERR]Date: $CUR_DATE \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]Date: $CUR_DATE" >> $LOG_FILE
        fi
    elif [ $# -eq 1 ]; then
        echo -e "\033[0;31m [ERR]$CUR_DATE: $1 \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]$CURTIME: $1" >> $LOG_FILE
        fi
    elif [ $# -eq 2 ]; then
        echo -e "\033[0;31m [ERR]$CUR_DATE: $1 $2 \033[0m"
        if [ "$PARAM_LOG"x != ""x ]
        then
            echo -e "[LOG]$CURTIME: $1 $2" >> $LOG_FILE
        fi
    else
        echo -e "\033[1;34m just support two params hahaha~~~ \033[0m"
    fi
}
export -f logerr

###################tar src.tar src###############################
function tar_tar()
{
    if [ $# -eq 1 ] && [ -d $1 ]
    then
        if [ -f $1.tar ]
        then
            loginfo "$1.tar exist, mv $1.tar $1.tar.bak..."
            mv $1.tar $1.tar.bak
        fi
        tar -cvf $1.tar $1
        if [ $? -eq 0 ]
        then
            cp $1.tar $CUR_PATH
            loginfo "tar $1 ok...:" $?
        else
            logerr "tar $1 err...:" $?
            exit 1
        fi
    else
        logerr "params error or $1 not exist..."
        exit 1
    fi
}
export -f tar_tar

###################tar src.tar.gz src###############################
function tar_tar_gz()
{
    if [ $# -eq 1 ] && [ -d $1 ]
    then
        if [ -f $1.tar.gz ]
        then
            loginfo "$1.tar.gz exist, mv $1.tar.gz $1.tar.gz.bak..."
            mv $1.tar.gz $1.tar.gz.bak
        fi
        tar -zcvf $1.tar.gz $1
        if [ $? -eq 0 ]
        then
            cp $1.tar.gz $CUR_PATH
            loginfo "tar $1 ok...:" $?
        else
            logerr "tar $1 err...:" $?
            exit 1
        fi
    else
        logerr "params error or $1 not exist..."
        exit 1
    fi
}
export -f tar_tar_gz

###################tar src.tar.bz2 src###############################
function tar_tar_bz2()
{
    if [ $# -eq 1 ] && [ -d $1 ]
    then
        if [ -f $1.tar.bz2 ]
        then
            loginfo "$1.tar.bz2 exist, mv $1.tar.bz2 $1.tar.bz2.bak..."
            mv $1.tar.bz2 $1.tar.bz2.bak
        fi
        tar -jcvf $1.tar.bz2 $1
        if [ $? -eq 0 ]
        then
            cp $1.tar.bz2 $CUR_PATH
            loginfo "tar $1 ok...:" $?
        else
            logerr "tar $1 err...:" $?
            exit 1
        fi
    else
        logerr "params error or $1 not exist..."
        exit 1
    fi
}
export -f tar_tar_bz2


###################shell_backup ###############################
function shell_backup()
{
    cp $CUR_PATH/run_build.sh $CUR_PATH/.run_build.sh
}

###################start log ###############################
function start_log()
{
    loginfo
    loginfo "##########################  LOG START BUILDING ################################"
}

###################end log ###############################
function end_log()
{
    loginfo "##########################  LOG END BUILDING ################################"
    loginfo
}

###################clean all libs ###############################
function clean_libs()
{
    if [ "$PARAM_CLEAN"x = "yes"x ]
    then
        rm $1/*
        rm $2/*
    fi
}

###################build third party libs ###############################
###################SRC_PATH ###############################
###################BUILD_NAME ###############################
function build_3rdpartylibs()
{
    if [ $# -eq 2 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./build_3rdlibs.sh
            if [ $? -eq 0 ]
            then
                loginfo "build $2 ok: " $?
                sleep 1
                cd $BASE_PATH
            else
                logerr "build $2 err: " $?
                exit 1
            fi
        else
            logerr "dir $1 not exist..."
            exit 1
        fi
    else
        logerr "params error..."
        exit 1
    fi
}


###################build MSCommon lib ###############################
###################SRC_PATH ###############################
###################PREFIX_PATH ###############################
###################3RD_PARTY_PATH ###############################
###################BUILD_NAME ###############################
function build_mscommonlib()
{
    if [ $# -eq 4 ]
    then
        if [ -d $1 ] && [ -d $2 ]
        then
            cd $1
            if [ "$PARAM_FORCE"x = "yes"x ]
            then
                autoreconf -ivf
                ./configure --prefix=$2
                make clean
            fi
            if [ "$PARAM_CLEAN"x = "yes"x ]
            then
                make clean
            fi
            make && make install
            if [ $? -eq 0 ]
            then
                cp $3/*.a $2/
                cp *.a $2/
                loginfo "build $4 ok: " $?
                cd $BASE_PATH
            else
                logerr "build $4 err: " $?
                exit 1
            fi
        else
            logerr "dir $1 not exist..."
            exit 1
        fi
    else
        logerr "params error..."
        exit 1
    fi
}


###################build bin ###############################
###################SRC_PATH ###############################
###################PREFIX_PATH ###############################
###################BIN_NAME ###############################
function build_bin()
{
    if [ $# -eq 3 ]
    then
        if [ -d $1 ] && [ -d $2 ]
        then
            cd $1
            if [ "$PARAM_FORCE"x = "yes"x ]
            then
                autoreconf -ivf
                ./configure --prefix=$2
                make clean
            fi
            if [ "$PARAM_CLEAN"x = "yes"x ]
            then
                make clean
            fi
            make && make install
            if [ $? -eq 0 ]
            then
                loginfo "build $3 ok: " $?
                cd $BASE_PATH
            else
                logerr "build $3 err: " $?
                exit 1
            fi
        else
            logerr "dir $1 not exist..."
            exit 1
        fi
    else
        logerr "params error..."
        exit 1
    fi
}

##############################################################
####################    program    ###########################
##############################################################
shell_backup
start_log
sleep 2

####################    building 3rdpartylibs    ###########################
if [ "yes"x = "yes"x ]
then
    COMMON_LIB_PATH=$BASE_PATH/MsgServer/MSCommonLib
    THIRDPARTY_LIB_PATH=$BASE_PATH/MsgServer/MSCommon/lib_linux_a
    clean_libs $COMMON_LIB_PATH $THIRDPARTY_LIB_PATH
else
    logerr "building 3rdpartylibs not running..."
fi

####################    building 3rdpartylibs    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServer/MSCommon
    BUILD_NAME=ThirdParybLibs
    build_3rdpartylibs $SRC_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_3rdpartylibs error..."
        exit 1
    fi
else
    logerr "building 3rdpartylibs not running..."
fi

####################    building mscommonlib    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServer
    BUILD_NAME=MsgCommonLib
    PREFIX_PATH=$BASE_PATH/MsgServer/MSCommonLib
    THIRDPARTY_LIB_PATH=$BASE_PATH/MsgServer/MSCommon/lib_linux_a
    build_mscommonlib $SRC_PATH $PREFIX_PATH $THIRDPARTY_LIB_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_mscommonlib error..."
        exit 1
    fi
else
    logerr "building mscommonlib not running..."
fi


####################    building msgserverconnector    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerConnector
    BUILD_NAME=MsgServerConnector
    PREFIX_PATH=$SRC_PATH/connector
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz connector
else
    logerr "building msgserverconnector not running..."
fi

####################    building msgserverdispatcher    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerDispatcher
    BUILD_NAME=MsgServerDispatcher
    PREFIX_PATH=$SRC_PATH/dispatcher
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz dispatcher
else
    logerr "building msgserverdispatcher not running..."
fi

####################    building msgservermeeting    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerMeeting
    BUILD_NAME=MsgServerMeeting
    PREFIX_PATH=$SRC_PATH/meeting
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz meeting
else
    logerr "building msgservermeeting not running..."
fi

####################    building msgserverrtlive    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerRTLive
    BUILD_NAME=MsgServerRTLive
    PREFIX_PATH=$SRC_PATH/rtlive
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz rtlive
else
    logerr "building msgserverrtlive not running..."
fi

####################    building msgserverlogical    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerLogical
    BUILD_NAME=MsgServerLogical
    PREFIX_PATH=$SRC_PATH/logical
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz logical
else
    logerr "building msgserverlogical not running..."
fi

####################    building msgserversequence    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerSequence
    BUILD_NAME=MsgServerSequence
    PREFIX_PATH=$SRC_PATH/sequence
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz sequence
else
    logerr "building msgserversequence not running..."
fi

####################    building msgserverstorage    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerStorage
    BUILD_NAME=MsgServerStorage
    PREFIX_PATH=$SRC_PATH/storage
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz storage
else
    logerr "building msgserverstorage not running..."
fi

####################    building msgservergrouper    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerGrouper
    BUILD_NAME=MsgServerGrouper
    PREFIX_PATH=$SRC_PATH/grouper
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz grouper
else
    logerr "building msgservergrouper not running..."
fi

####################    building msgserverpusher    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerPusher
    BUILD_NAME=MsgServerPusher
    PREFIX_PATH=$SRC_PATH/pusher
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz pusher
else
    logerr "building msgserverpusher not running..."
fi

####################    building msgserverdaemon    ###########################
if [ "yes"x = "yes"x ]
then
    SRC_PATH=$BASE_PATH/MsgServerDaemon
    BUILD_NAME=MsgServerDaemon
    PREFIX_PATH=$SRC_PATH/daemon
    build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME
    if [ $? -ne 0 ]
    then
        logerr "build_bin $BUILD_NAME error..."
        exit 1
    fi
    cd $SRC_PATH
    tar_tar_gz daemon
else
    logerr "building msgserverdaemon not running..."
fi


####################    taring all.tar.gz to msgserver    ###########################
if [ "yes"x = "yes"x ]
then
    loginfo "tar all the bin program..."
    sleep 1
    cd $CUR_PATH
    rm msgserver.tar.gz
    rm -rf msgserver
    mkdir -p msgserver
    mv *.tar.gz msgserver
    cp *.sh msgserver
    rm msgserver/run_build.sh
    tar -zcvf msgserver.tar.gz msgserver
    loginfo "tar all the bin program ok..."
    sleep 1

    find $BASE_PATH -iname *.o | xargs rm
    loginfo "remove all the *.o ..."
else
    logerr "taring all.tar.gz not running..."
fi

end_log
loginfo "build all bin successfully..."
sleep 1
loginfo "build all bin successfully..."
sleep 1
loginfo "build all bin successfully..."
sleep 1
exit
