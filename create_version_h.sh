echo creating ${1}/version.h

if [ "${APPVEYOR_BUILD_VERSION}" = "" ];
then
	echo build not in appveyor
else
    (cd "${1}"; git tag ${APPVEYOR_BUILD_VERSION})
fi

version=$(cd "${1}"; git describe --tags --long)
echo "#define GIT_VERSION \"$version\"" > ${1}/version.h
