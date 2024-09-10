# syslib

Header-only C++ logging & IO library.

## Install
```console
$ git clone https://github.com/Yitao-Huang/syslib.git
$ cd syslib && mkdir build && cd build
$ cmake ..
$ make test_debug && ./bin/test_debug   # build target test
$ make benchmark_release                # build target benchmark
$ ./bin/benchmark sync                  # run synchronous logging benchmark
$ ./bin/benchmark async                 # run asynchronous logging benchmark
```