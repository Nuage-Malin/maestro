#!/bin/bash
# Installing gRPC according to official website : https://grpc.io/docs/languages/cpp/quickstart/

if [ ! $INSTALL_DIR ]; then
  INSTALL_DIR=$HOME/.local
fi

if [ ! $CLONE_DIR ]; then
  CLONE_DIR="third_parties"
fi

## install git
sudo apt-get install -y git
## install cmake
sudo apt-get install -y cmake
## install dependancy
sudo apt-get install -y build-essential autoconf libtool pkg-config

pushd $CLONE_DIR

if [ ! -d grpc ]; then
  ## clone gRPC
#  git clone https://github.com/grpc/grpc
  git submodule add git@github.com:grpc/grpc.git ./grpc/
fi
git submodule update --init --recursive # todo put that after going into the gRPC folder

## compile gRPC
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j 10

mkdir -p $INSTALL_DIR

## install gRPC from source
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      ../..
      ## todo install on entire system is useless if used as submodule ?

export PATH="$INSTALL_DIR/bin:$PATH"

## TODO put that into $HOME/.zshrc or $HOME/.bashrc
##  with one of these command lines
ZSH=$( ( env | grep SHELL | grep zsh) )
if [ $ZSH ]; then
 echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.zshrc
else
 echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.bashrc
 ## todo add other possible shells
fi

make -j 10
make install
popd
popd