#!/bin/sh

is_file()
{
    for f in "$@"
    do
	[ -f "$f" ] && return
    done;
    return 1
}

return_failure()
{
#    echo Failure.
    cd "$@"
    exit 1
}

USER_ROOT_DIR=/mnt/us
UPDATE_DIR=customupdates

SAVE_DIR=`pwd`

# return failure by default
RC=1

cd $USER_ROOT_DIR

pattern="$UPDATE_DIR/*.zip"

is_file $pattern && \
for archive in $pattern
do
#    echo $archive
    unzip -oq $archive
    RC=$?
    if [ "$RC" -ne 0 ]
    then
	return_failure $SAVE_DIR
    fi
    rm $archive
done

pattern="$UPDATE_DIR/*.tar.gz"

is_file $pattern && \
for archive in $pattern
do
#    echo $archive
    tar zxvf $archive
    RC=$?
    if [ "$RC" -ne 0 ]
    then
	return_failure $SAVE_DIR
    fi
    rm $archive
done

pattern="$UPDATE_DIR/*.tgz"

is_file $pattern && \
for archive in $pattern
do
#    echo $archive
    tar zxvf $archive
    RC=$?
    if [ "$RC" -ne 0 ]
    then
	return_failure $SAVE_DIR
    fi
    rm $archive
done

pattern="$UPDATE_DIR/*.sh"

is_file $pattern && \
for script in $pattern
do
#    echo $script
    if [ -x "$script" ]
    then 
	$script
    else
	. $script
    fi
    RC=$?
    if [ "$RC" -ne 0 ]
    then
	return_failure $SAVE_DIR
    fi
    rm $script
done


cd $SAVE_DIR

exit $RC

