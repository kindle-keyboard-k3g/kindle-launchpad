#! /bin/sh
#
# $Id: build-updates.sh 6683 2010-08-30 00:43:01Z NiLuJe $
#

HACKNAME="launchpad"
PKGNAME="${HACKNAME}"
PKGVER="0.0.1c"

KINDLE_MODELS="k2 k2i dx dxi dxg k3g k3w k3gb"
#KINDLE_MODELS="dxg"

# Archive custom directory
tar --exclude="*.svn" -cvzf ${HACKNAME}.tar.gz ../src/${HACKNAME}

for model in ${KINDLE_MODELS} ; do
	# Prepare our files for this specific kindle model...
	ARCH=${PKGNAME}_${PKGVER}_${model}

	# Build install update
	../packager/kindle_update_tool.py m --${model} --sign ${ARCH}_install install.sh ${HACKNAME}-init ${HACKNAME}.tar.gz

	# Build uninstall update
	../packager/kindle_update_tool.py m --${model} --sign ${ARCH}_uninstall uninstall.sh
done

# Remove custom directory archive
rm -f ${HACKNAME}.tar.gz

# Move our updates :)
mv -f *.bin ../
