#!/bin/bash
# Installing gRPC according to official website : https://grpc.io/docs/languages/cpp/quickstart/

if [ ! $INSTALL_DIR ]; then
    INSTALL_DIR=$HOME/.local
fi

if [ ! $CLONE_DIR ]; then
    CLONE_DIR="third_parties"
fi

if [ ! $OS_DISTRIB ]; then
    . /etc/os-release
    OS_DISTRIB=$ID
fi
if [ $OS_DISTRIB = "ubuntu" ]; then
    if [ `command -v sudo` ]; then
        sudo apt-get install -y \
            cmake           \
            build-essential \
            autoconf        \
            libtool         \
            pkg-config
    else
        apt-get install -y      \
            cmake           \
            build-essential \
            autoconf        \
            libtool         \
            pkg-config
    fi
elif [ $OS_DISTRIB = "fedora" ]; then
    if [ `command -v sudo` ]; then
        sudo dnf install    \
            libtool
    else
        dnf install \
            libtool
    fi
else
    echo "Unknown OS distribution $OS_DISTRIB"
fi

pushd $CLONE_DIR

if [ "$GRPC_CLONE_SUBMODULE" == "true" ]; then

    if [ $OS_DISTRIB = "ubuntu" ]; then
        if [ `command -v sudo` ]; then
            ## install git
            sudo apt-get install -y git
        else
            ## install git
            apt-get install -y git
        fi
    elif [ $OS_DISTRIB = "fedora" ]; then
        if [ `command -v sudo` ]; then
            ## install git
            sudo dnf install git
        else
            ## install git
            dnf install git
        fi
    else
        echo "Unknown OS distribution $OS_DISTRIB"
    fi

    if [ ! -d grpc ]; then
        ## clone gRPC
        # git clone https://github.com/grpc/grpc
        git submodule add git@github.com:grpc/grpc.git ./grpc/
    fi
    git submodule update --init --recursive # todo put that after going into the gRPC folder
fi

## compile gRPC
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j 10

if [ "$GRPC_FULL_INSTALL" == "true" ]; then
    echo "hello full install"
    mkdir -p $INSTALL_DIR

    ## install gRPC from source
    cmake -DgRPC_INSTALL=ON \
            -DgRPC_BUILD_TESTS=OFF \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DABSL_ENABLE_INSTALL=ON \
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
fi