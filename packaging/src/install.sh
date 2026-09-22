#!/bin/sh
#
# $Id: install.sh 6682 2010-08-30 00:32:30Z NiLuJe $
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

# Remove our deprecated content
# Nothing to do here

update_progressbar 3

# Install our hack's custom content
# But keep the user's custom content...

update_progressbar 20

# Okay, now we can extract it. Since busybox's tar is very limited, we have to use a tmp directory to perform our filtering
logmsg "I" "update" "installing custom directory"
tar -xvzf ${HACKNAME}.tar.gz

# That's very much inspired from official update scripts ;)
cd src
# And now we filter the content to preserve user's custom content
for custom_file in ${HACK_EXCLUDE} ; do
    if [ -f "./${custom_file}" ] ; then
        logmsg "I" "update" "preserving custom content (${custom_file})"
        rm -f "./${custom_file}"
    fi
done

killall ${HACKNAME} 2>/dev/null

# Finally, re-tape our filtered dir and unleash it on the live userstore
tar cf - . | (cd /mnt/us ; tar xvf -)
_RET=$?
if [ ${_RET} -ne 0 ] ; then
    logmsg "C" "update" "code=${_RET}" "failure to update userstore with custom directory"
    return 1
fi
cd - >/dev/null
rm -rf src

update_progressbar 40

# Install our hack's init script
## mntroot rw
logmsg "I" "update" "installing init script"
cp -f ${HACKNAME}-init /etc/init.d/${HACKNAME}

update_progressbar 60

# Make it executable
logmsg "I" "update" "chmoding init script"
[ -x /etc/init.d/${HACKNAME} ] || chmod +x /etc/init.d/${HACKNAME}

update_progressbar 80

# Make it start at early at boot (rcS), after the userstore
logmsg "I" "update" "creating early boot runlevel symlink"
[ -h /etc/rcS.d/S${SLEVEL}${HACKNAME} ] || ln -fs /etc/init.d/${HACKNAME} /etc/rcS.d/S${SLEVEL}${HACKNAME}

## mntroot ro

update_progressbar 90

# Cleanup
logmsg "I" "update" "cleaning up"
rm -f ${HACKNAME}-init ${HACKNAME}.tar.gz

update_progressbar 99

/etc/init.d/${HACKNAME} start

logmsg "I" "update" "done"
update_progressbar 100

return 0
