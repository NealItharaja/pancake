# pancake
An implementation of the Pancake security system, with both static and dynamic distributions

## Requirements
- `libcrypto++-dev`
- `redis-server`
- `libhiredis-dev`
- `libthrift-dev`
- `thrift-compiler`
- `cmake`
- `pkg-config`
- `build-essential`
- `parallel-hashmap`

## Commands:
1. Create cmakebuild folder (mkdir cmakebuild) & enter it

2. Run cmake (cmake .. -DPANCAKE_BUILD_BENCHMARK=ON & cmake --build . -j"$(nproc)")

3. Start proxy server within cmakebuild (ex: ./bin/pancake_proxy_server -h 127.0.0.1 -p 9090 -s redis -n 1 -l /tmp/ycsb_a.trace -b 50 -c 50 -v 128 -q 50)

4. Run benchmark on seperate window (ex: ./bin/benchmark -h 127.0.0.1 -p 9090 -t /tmp/ycsb_a.trace -n 8 -b 50 -o benchmark_data_n8)
