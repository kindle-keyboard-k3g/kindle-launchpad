#!/bin/sh
#
# $Id: uninstall.sh 6682 2010-08-30 00:32:30Z NiLuJe $
#
# diff OTA patch script

_FUNCTIONS=/etc/rc.d/functions
[ -f ${_FUNCTIONS} ] && . ${_FUNCTIONS}


MSG_SLLVL_D="debug"
MSG_SLLVL_I="info"
MSG_SLLVL_W="warn"
MSG_SLLVL_E="err"
MSG_SLLVL_C="crit"
MSG_SLNUM_D=0
MSG_SLNUM_I=1
MSG_SLNUM_W=2
MSG_SLNUM_E=3
MSG_SLNUM_C=4
MSG_CUR_LVL=/var/local/system/syslog_level

logmsg()
{
    local _NVPAIRS
    local _FREETEXT
    local _MSG_SLLVL
    local _MSG_SLNUM

    _MSG_LEVEL=$1
    _MSG_COMP=$2

    { [ $# -ge 4 ] && _NVPAIRS=$3 && shift ; }

    _FREETEXT=$3

    eval _MSG_SLLVL=\${MSG_SLLVL_$_MSG_LEVEL}
    eval _MSG_SLNUM=\${MSG_SLNUM_$_MSG_LEVEL}

    local _CURLVL

    { [ -f $MSG_CUR_LVL ] && _CURLVL=`cat $MSG_CUR_LVL` ; } || _CURLVL=1

    if [ $_MSG_SLNUM -ge $_CURLVL ]; then
        /usr/bin/logger -p local4.$_MSG_SLLVL -t "ota_install" "$_MSG_LEVEL def:$_MSG_COMP:$_NVPAIRS:$_FREETEXT"
    fi

    [ "$_MSG_LEVEL" != "D" ] && echo "ota_install: $_MSG_LEVEL def:$_MSG_COMP:$_NVPAIRS:$_FREETEXT"
}

if [ -z "${_PERCENT_COMPLETE}" ]; then
    export _PERCENT_COMPLETE=0
fi

update_percent_complete()
{
    _PERCENT_COMPLETE=$((${_PERCENT_COMPLETE} + $1))
    update_progressbar ${_PERCENT_COMPLETE}
}

# Hack specific config (name and when to start/stop)
HACKNAME="launchpad"
SLEVEL="76"
#KLEVEL="09"

update_percent_complete 2

# From v1.0.N
# Early boot symlink
## mntroot rw
logmsg "I" "update" "removing early boot symlink"
[ -h /etc/rcS.d/S${SLEVEL}${HACKNAME} ] && rm -f /etc/rcS.d/S${SLEVEL}${HACKNAME}

update_progressbar 33

# Remove our hack's init script
logmsg "I" "update" "removing init script"
[ -f /etc/init.d/${HACKNAME} ] && rm -f /etc/init.d/${HACKNAME}

## mntroot ro

update_progressbar 66

killall ${HACKNAME} 2>/dev/null

# Remove custom directory in userstore?
#logmsg "I" "update" "removing custom directory (only if /mnt/us/${HACKNAME}/uninstall exists)"
#[ -d /mnt/us/${HACKNAME} -a -f /mnt/us/${HACKNAME}/uninstall ] && rm -rf /mnt/us/${HACKNAME}
[ -d /mnt/us/${HACKNAME} ] && rm -rf /mnt/us/${HACKNAME}

logmsg "I" "update" "done"
update_progressbar 100

return 0
