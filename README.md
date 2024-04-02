# hpx-rs

### Installing Boost

```sh
_boost_version_="$(git -c 'versionsort.suffix=-' \
  ls-remote --tags --sort='v:refname' \
  https://github.com/boostorg/boost.git |\
  grep -v beta | tail --lines=1 |\
  cut --delimiter='/' --fields=3)"


pushd /tmp
wget https://github.com/boostorg/boost/releases/download/$_boost_version_/$_boost_version_.tar.gz
tar -xvzf $_boost_version_.tar.gz

pushd $_boost_version_

./bootstrap.sh
./b2 toolset=clang link=static

############################################################
export Boost_INCLUDE_DIR=/tmp/$_boost_version_
export LD_LIBRARY_PATH=/tmp/$_boost_version_/stage/lib

popd
popd
```


### Compiling HPX

```sh
cmake .. -DHPX_WITH_FETCH_ASIO=ON -DHPX_WITH_FETCH_HWLOC=ON -DHPX_WITH_MALLOC=system -DBoost_INCLUDE_DIR="$Boost_INCLUDE_DIR" -DCMAKE_INSTALL_PREFIX=$(realpath ../../hpx-compiled) -DHPX_WITH_EXAMPLES=OFF -DHPX_WITH_TESTS=OFF -G Ninja
ninja
```


### Compiling demo

Example build instructions:

```sh
#!/bin/bash
export _HOME="$(realpath ~)"
export CXX=clang++
export CC=clang
export HPX_INSTALLED_DIR=$_HOME/workspace/hpx-compiled
export Boost_DIR=$_HOME/workspace/boost-1.84.0/
export HWLOC_SRC=$_HOME/workspace/hpx/build/_deps/hwloc-installed
export LD_FLAGS="-L$HPX_INSTALLED_DIR/lib -L $_HOME/workspace/hpx/build/lib -L ${HWLOC_SRC}/lib -L${Boost_DIR}/stage/lib -lhpx_init -lhpx -lboost_context -lboost_coroutine -lhpx_core -lhpx_iostreams"

export CXXFLAGS="${CPPFLAGS}"" -I./include -I$HPX_INSTALLED_DIR/include  -I$Boost_DIR -I$HWLOC_SRC/include"

cargo clean
cargo build

unset CC
unset CXX
unset HPX_INSTALLED_DIR
unset Boost_DIR
unset HWLOC_SRC
unset LD_FLAGS
unset CXXFLAGS
unset _HOME
```

### Running

```sh
export _HOME="$(realpath ~)"
export CXX=clang++
export CC=clang
export HPX_INSTALLED_DIR=$_HOME/workspace/hpx-compiled
export Boost_DIR=$_HOME/workspace/boost-1.84.0/
export HWLOC_SRC=$_HOME/workspace/hpx/build/_deps/hwloc-installed
export LD_FLAGS="-L$HPX_INSTALLED_DIR/lib -L $_HOME/workspace/hpx/build/lib -L ${HWLOC_SRC}/lib -L${Boost_DIR}/stage/lib -lhpx_init -lhpx -lboost_context -lboost_coroutine -lhpx_core"
export LD_LIBRARY_PATH="$HPX_INSTALLED_DIR/lib:$_HOME/workspace/hpx/build/lib:${HWLOC_SRC}/lib:${Boost_DIR}/stage/lib"

export CXXFLAGS="${CPPFLAGS}"" -I./include -I$HPX_INSTALLED_DIR/include  -I$Boost_DIR -I$HWLOC_SRC/include"

./target/debug/hpx-rs-demo

unset CC
unset CXX
unset HPX_INSTALLED_DIR
unset Boost_DIR
unset HWLOC_SRC
unset LD_FLAGS
unset CXXFLAGS
unset _HOME
```