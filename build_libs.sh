cd ${1}/libcurl/
mkdir build.bcu
cd ${1}/libcurl/build.bcu
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_FIND_LIBRARY_SUFFIXES=.a -DCURL_STATICLIB=ON -DCMAKE_FIND_LIBRARY_SUFFIXES=.a -DBUILD_TESTING=OFF -DCMAKE_USE_OPENSSL=ON -DCURL_ZLIB=OFF
cmake --build . --config Release --clean-first
cd ../../
