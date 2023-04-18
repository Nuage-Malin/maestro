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

if [ "$GRPC_CLONE_SUBMODULE" == "true" ] || [ "$GRPC_RECOMPILE" == "true"  ]; then
  ## compile gRPC
  cd grpc
  check_exit_failure "Failed to go into grpc folder"
  mkdir -p cmake/build
  pushd cmake/build
  check_exit_failure "Failed to go into grpc/cmake/build folder"
  cmake ../..
  check_exit_failure "Failed to cmake grpc"
  make -j $((`nproc` - 1))
  check_exit_failure "Failed to make grpc"
fi

if [ "$GRPC_FULL_INSTALL" == "true" ]; then
  mkdir -p $INSTALL_DIR

    ## install gRPC from source
    cmake -DgRPC_INSTALL=ON \
            -DgRPC_BUILD_TESTS=OFF \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DABSL_ENABLE_INSTALL=ON \
            ../..
            ## todo install on entire system is useless if used as submodule ?
    check_exit_failure "Failed to cmake grpc (full install)"

    export PATH="$INSTALL_DIR/bin:$PATH"

    ## TODO put that into $HOME/.zshrc or $HOME/.bashrc if it is not already there
    ##  with one of these command lines
    ZSH=$( ( echo $SHELL | grep zsh) )
    if [ $ZSH ] && ! [ cat $HOME/.zshrc | grep "$INSTALL_DIR/bin:$PATH" ]; then
        echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.zshrc
    elif ! [ cat $HOME/.bashrc | grep "$INSTALL_DIR/bin:$PATH" ]; then
        echo "PATH=\"$INSTALL_DIR/bin:$PATH\"" >> $HOME/.bashrc
        ## todo add other possible shells
    fi

    nbr_cpu=`nproc`
    check_exit_failure "Failed to get number of cpu"
    make -j $((`nproc` - 1))
    check_exit_failure "Failed to make grpc (full install)"
    make install
    check_exit_failure "Failed to install grpc (full install)"
    popd
    popd
fi