cd libpng-1.6.40
emcmake cmake
emmake make
cd ..
emconfigure ./build.sh uqm config
./build.sh uqm
rm -rf obj
cp src/emscripten/live.html .