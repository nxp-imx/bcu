echo creating version.h

if [ "${APPVEYOR_BUILD_VERSION}" = "" ];
then
	echo build not in appveyor
else
	git tag ${APPVEYOR_BUILD_VERSION}
fi

version=`git describe --tags --long`
echo "#define GIT_VERSION \"$version\"" > version.h
