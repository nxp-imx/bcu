cd ${1}/libcurl/
mkdir build.bcu
cd ${1}/build.bcu
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_FIND_LIBRARY_SUFFIXES=.a -DCURL_STATICLIB=ON -DCMAKE_FIND_LIBRARY_SUFFIXES=.a -DBUILD_TESTING=OFF -DCURL_DISABLE_LDAP=1 -DCMAKE_USE_OPENSSL=ON -DCURL_ZLIB=OFF -DOPENSSL_ROOT_DIR=${2} -DOPENSSL_LIBRARIES=${2}/lib
cmake --build . --config Release --clean-first
cd ../../

cd ${1}/libftdi/
cmake .
make
cd ../