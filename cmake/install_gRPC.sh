#!/bin/bash
# Installing gRPC according to official website : https://grpc.io/docs/languages/cpp/quickstart/

check_exit_failure()
{
    if [ $? -ne 0 ]
    then
        echo "$1" 1>&2
        exit 1
    fi
}

if [ ! $INSTALL_DIR ]; then
    INSTALL_DIR=$HOME/.local
fi

if [ ! $CLONE_DIR ]; then
    CLONE_DIR=$THIRD_PARTIES_DIR
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
        check_exit_failure "Failed to install gRPC dependencies"
    else
        apt-get install -y      \
            cmake           \
            build-essential \
            autoconf        \
            libtool         \
            pkg-config
        check_exit_failure "Failed to install gRPC dependencies"
    fi
elif [ $OS_DISTRIB = "fedora" ]; then
    if ! dnf list installed | grep libtool; then
        if [ `command -v sudo` ]; then
            sudo dnf install    \
                libtool
            check_exit_failure "Failed to install gRPC dependencies"
        else
            dnf install \
                libtool
            check_exit_failure "Failed to install gRPC dependencies"
        fi
    fi
else
    echo "Unknown OS distribution $OS_DISTRIB"
    exit 1
fi

pushd $CLONE_DIR

if [ "$GRPC_CLONE_SUBMODULE" == "true" ]; then

    if [ $OS_DISTRIB = "ubuntu" ]; then
        if [ `command -v sudo` ]; then
            sudo apt-get install -y git
            check_exit_failure "Failed to install git"
        else
            apt-get install -y git
            check_exit_failure "Failed to install git"
        fi
    elif [ $OS_DISTRIB = "fedora" ]; then
        if [ `command -v sudo` ]; then
            sudo dnf install git
            check_exit_failure "Failed to install git"
        else
            dnf install git
            check_exit_failure "Failed to install git"
        fi
    else
        echo "Unknown OS distribution $OS_DISTRIB"
        exit 1
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
check_exit_failure "Failed to go into grpc folder"
mkdir -p cmake/build
pushd cmake/build
check_exit_failure "Failed to go into grpc/cmake/build folder"
if [ `command -v sudo` ]; then
    sudo cmake ../..
    check_exit_failure "Failed to cmake grpc as root"
    sudo make -j $((`nproc` - 1))
    check_exit_failure "Failed to make grpc as root"

    sudo mkdir -p $INSTALL_DIR
    ## install gRPC from source
    sudo cmake -DgRPC_INSTALL=ON \
            -DgRPC_BUILD_TESTS=OFF \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DABSL_ENABLE_INSTALL=ON \
            ../..
    check_exit_failure "Failed to cmake grpc (full install) as root"
else
    cmake ../..
    check_exit_failure "Failed to cmake grpc"
    make -j $((`nproc` - 1))
    check_exit_failure "Failed to make grpc"

    mkdir -p $INSTALL_DIR
    ## install gRPC from source
    cmake -DgRPC_INSTALL=ON \
            -DgRPC_BUILD_TESTS=OFF \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DABSL_ENABLE_INSTALL=ON \
            ../..
    check_exit_failure "Failed to cmake grpc (full install)"
fi

export PATH="$INSTALL_DIR/bin:$PATH"

ZSH=$( ( echo $SHELL | grep zsh) )
if ! [[ -z $ZSH ]] && (! cat $HOME/.zshrc | grep PATH | grep "$INSTALL_DIR/bin"); then
    echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.zshrc
elif (! cat $HOME/.bashrc | grep PATH | grep "$INSTALL_DIR/bin"); then
    echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.bashrc
else
    echo "gRPC path already added to PATH"
fi
echo "Install directory : $INSTALL_DIR"

if [ `command -v sudo` ]; then
    sudo make -j $((`nproc` - 1))
    check_exit_failure "Failed to make grpc (full install)"

    sudo make install -j $((`nproc` - 1))
    check_exit_failure "Failed to install grpc (full install) as root"
else
    make -j $((`nproc` - 1))
    check_exit_failure "Failed to make grpc (full install)"

    make install -j $((`nproc` - 1))
    check_exit_failure "Failed to install grpc (full install)"
fi
popd
popd