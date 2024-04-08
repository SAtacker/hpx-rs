#!/bin/bash

export _HOME="$(realpath ~)"
export CXX=clang++
export CC=clang
export HPX_INSTALLED_DIR=$_HOME/workspace/hpx-compiled
export Boost_DIR=$_HOME/workspace/boost-1.84.0/
export HWLOC_SRC=$_HOME/workspace/hpx/build/_deps/hwloc-installed
export LD_FLAGS="-L$HPX_INSTALLED_DIR/lib -L $_HOME/workspace/hpx/build/lib -L ${HWLOC_SRC}/lib -L${Boost_DIR}/stage/lib -lhpx_init -lhpx -lboost_context -lboost_coroutine -lhpx_core"
export LD_LIBRARY_PATH="$HPX_INSTALLED_DIR/lib:$_HOME/workspace/hpx/build/lib:${HWLOC_SRC}/lib:${Boost_DIR}/stage/lib:$Boost_DIR/stage/lib"
export DYLD_LIBRARY_PATH="$HPX_INSTALLED_DIR/lib:$Boost_DIR/stage/lib:$_HOME/workspace/hpx/build/lib"

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