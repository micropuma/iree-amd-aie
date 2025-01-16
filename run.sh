#!/bin/bash
export PATH=/home/douliyang/local/bin:$PATH

cmake \
  -B build/ \
  -S third_party/iree \
  -DIREE_CMAKE_PLUGIN_PATHS=$PWD \
  -DIREE_BUILD_PYTHON_BINDINGS=ON