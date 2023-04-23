cd cpp
if ! [ -d "build" ]; then
    mkdir build
fi
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/g++
make -j
./src/trace_gen ../../configs/trace_config.json ../../traces/trace
