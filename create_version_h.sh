echo creating ${1}/version.h

if [ "${CI_BUILD_VERSION}" = "" ];
then
	echo build not in appveyor
fi

version=`git describe --tags --long`
echo "#define GIT_VERSION \"$version\"" > ${1}/version.h
