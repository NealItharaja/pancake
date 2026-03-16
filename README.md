# Pancake
An implementation of the Pancake security system, with both static, dynamic and gradual distributions. The major additions in this project are the dynamic and gradual implementations, which detect active changes in the access frequency distributions by using the Kolmogorov-Smirnov (KS) and the Anderson-Darling (AD) statistical tests, respectively. Thus, unlike in previous iterations, this implementation now is able to apply Pancake's novel access frequency smoothing techniques on both distributions that change suddenly (through the KS test), and those that change more gradually (through the AD test). 

## Requirements
- `libcrypto++-dev`
- `redis-server`
- `libhiredis-dev`
- `libthrift-dev`
- `thrift-compiler`
- `cmake`
- `pkg-config`
- `build-essential`
- `parallel-hashmap` - https://github.com/greg7mdp/parallel-hashmap (Note: No need to clone this repo, CMake will automatically fetch it for you)

## Steps to Run:
1. Create cmakebuild folder (mkdir cmakebuild) & enter it

2. Run cmake using the following lines:
```courseignore
cmake .. -DPANCAKE_BUILD_BENCHMARK=ON
cmake --build . -j"$(nproc)"
```

3. Start proxy server within cmakebuild (ex: ./bin/pancake_proxy_server \ -h 127.0.0.1 \ -p 9090 \ -s redis \ -n 1 \ -t 32 \ -l /tmp/pancake_traces/ycsb_a_1m.trace \ -b 3 \ -c 50 \ -v 128 \ -q 50)

4. Run benchmark on seperate window (ex: ./bin/benchmark -h 127.0.0.1 -p 9090 -t /tmp/pancake_traces/ycsb_a_1m.trace -n 8 -b 50 -o ./data)

## Required Traces & Commands:
In order to start the proxy server, first start redis-server:
```courseignore
redis-server --daemonize yes
redis-cli ping //Should output PONG
```

Then, start the server using the following command:
```linux
./bin/pancake_proxy_server \
  -h <HOST> \
  -p <PORT> \
  -s <BACKEND_TYPE> \
  -n <BACKEND_COUNT> \
  -t <NUM_PROXY_THREADS> \
  -l <WORKLOAD_FILE> \
  -b <SECURITY_BATCH_SIZE> \
  -c <STORAGE_BATCH_SIZE> \
  -v <VALUE_SIZE> \
  -q <CLIENT_BATCH_SIZE>
```

Then, run the following benchmark command in a new window, with the desired YCSB Workload Trace:
```linux
./bin/benchmark \
  -h <PROXY_IP> \
  -p <PROXY_PORT> \
  -t <WORKLOAD_FILE> \
  -n <NUM_CLIENTS> \
  -b <BATCH_SIZE> \
  -o <OUTPUT_DIR>
```

For the YCSB workload traces, run the following in the terminal for the different workloads (from the root folder):
```linux
mkdir -p /tmp/pancake_traces

python3 - <<'PY'
import os, random, bisect

OPS = 1_000_000
KEYSPACE = 1_048_576
THETA = 0.99
OUTDIR = "/tmp/pancake_traces"

random.seed(7)

def build_zipf_cdf(n, theta):
    w = [1.0 / ((i + 1) ** theta) for i in range(n)]
    s = 0.0
    cdf = []
    for x in w:
        s += x
        cdf.append(s)
    inv = 1.0 / s
    for i in range(n):
        cdf[i] *= inv
    return cdf

cdf = build_zipf_cdf(KEYSPACE, THETA)

def zipf_key():
    r = random.random()
    idx = bisect.bisect_left(cdf, r)
    if idx >= KEYSPACE:
        idx = KEYSPACE - 1
    return f"user{idx+1}"

def rand_val():
    return f"val{random.randint(1, 10_000_000)}"

def gen_A(path):
    # YCSB A
    with open(path, "w") as f:
        for _ in range(OPS):
            k = zipf_key()
            if random.random() < 0.5:
                f.write(f"GET {k}\n")
            else:
                f.write(f"PUT {k} {rand_val()}\n")

def gen_B(path):
    # YCSB B
    with open(path, "w") as f:
        for _ in range(OPS):
            k = zipf_key()
            if random.random() < 0.95:
                f.write(f"GET {k}\n")
            else:
                f.write(f"PUT {k} {rand_val()}\n")

def gen_C(path):
    # YCSB C
    with open(path, "w") as f:
        for _ in range(OPS):
            f.write(f"GET {zipf_key()}\n")

def gen_D(path):
    # YCSB D
    latest = KEYSPACE
    recent_window = 100_000
    with open(path, "w") as f:
        for _ in range(OPS):
            if random.random() < 0.95:
                if random.random() < 0.8:
                    lo = max(1, latest - recent_window + 1)
                    k = f"user{random.randint(lo, latest)}"
                else:
                    k = f"user{random.randint(1, latest)}"
                f.write(f"GET {k}\n")
            else:
                latest += 1
                k = f"user{latest}"
                f.write(f"PUT {k} {rand_val()}\n")

os.makedirs(OUTDIR, exist_ok=True)
gen_A(f"{OUTDIR}/ycsb_a_1m.trace")
gen_B(f"{OUTDIR}/ycsb_b_1m.trace")
gen_C(f"{OUTDIR}/ycsb_c_1m.trace")
gen_D(f"{OUTDIR}/ycsb_d_1m.trace")

print("Generated:")
for name in ["ycsb_a_1m.trace", "ycsb_b_1m.trace", "ycsb_c_1m.trace", "ycsb_d_1m.trace"]:
    p = f"{OUTDIR}/{name}"
    print(" -", p, f"({os.path.getsize(p)} bytes)")
PY   
```