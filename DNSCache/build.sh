#!/bin/sh

build_type=Release
build_tests=OFF

for param in $*; do
  name=${param%%=*}
  value=${param#*=}
  case "$name" in
    --type) build_type=$value;;
    --tests) build_tests=$value;;
  esac
done

for dir in build output; do
  if [ ! -d $dir ]; then
    mkdir $dir
  fi
done

for dir in build/$build_type output/$build_type; do
  if [ ! -d $dir ]; then
    mkdir $dir
  fi
  rm -r $dir/* 2> /dev/null
done

cd build/$build_type
cmake  -DCMAKE_BUILD_TYPE="$build_type"  -DBUILD_GTESTS="$build_tests" ../../src
make

cp ../../data/* ../../output/$build_type/bin/
