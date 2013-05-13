#!/bin/bash

echo " * Checking out Assimp ..."
svn checkout -r1347 https://assimp.svn.sourceforge.net/svnroot/assimp

for f in assimp-r1347-iso-c-enumerators.patch assimp-r1347-no-visibility-for-types.patch ; do
	echo " * Applying $f ..."
	patch -d assimp -p0 < $f
	if [ $? -ne 0 ] ; then
		echo "FAILED"
		exit 1
	fi
done
