## Instructions found at https://mongocxx.org/mongocxx-v3/installation/linux/

check_exit_failure()
{
    if [ $? -ne 0 ]
    then
        echo "$1" 1>&2
        exit 1
    fi
}

echo starting install of mongo cxx
if [ ! $MONGOCXX_VERSION ]; then
  MONGOCXX_VERSION="3.7.0"
fi

if [ ! $INSTALL_DIR ]; then
  INSTALL_DIR="third_parties"
fi

## Install C driver
if [ `command -v sudo` ]; then
#  sudo apt-get update
  sudo apt-get install -y libssl-dev libsasl2-dev
  sudo apt-get install -y libmongoc-dev
  sudo apt-get install -y libmongoc-doc
# sudo apt-get install -y libstdc++-10-dev
  sudo apt-get install -y libstdc++-12-dev
  sudo apt-get install -y libmongoc-1.0-0
  sudo apt-get install -y libbson-1.0-0

#else
#  apt-get update
  apt-get install -y libssl-dev libsasl2-dev
  apt-get install -y libmongoc-dev
  apt-get install -y libmongoc-doc
# apt-get install -y libstdc++-10-dev
  apt-get install -y libstdc++-12-dev
  apt-get install -y libmongoc-1.0-0
  apt-get install -y libbson-1.0-0
fi


## install C++ driver
### use at least CPP 11
### from https://github.com/mongodb/mongo-cxx-driver/releases
### then follow instructions from step 4 of https://mongocxx.org/mongocxx-v3/installation/linux/

if [ ! -f $INSTALL_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz ]; then
  curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r$MONGOCXX_VERSION/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
  check_exit_failure "Fail to download mongo-cxx-driver"
  mv mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz $INSTALL_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
  check_exit_failure "Fail to move mongo-cxx-driver"
fi

cd $INSTALL_DIR/
if [ ! -d mongo-cxx-driver-r$MONGOCXX_VERSION ]; then
  tar -xzf mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
  check_exit_failure "Fail to extract mongo-cxx-driver"
fi
cd mongo-cxx-driver-r$MONGOCXX_VERSION/

## note that polyfill steps have been skipped

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/mongo
check_exit_failure "Fail to cmake mongo-cxx-driver"
if [ `command -v sudo` ]; then
  sudo make -C build install
  check_exit_failure "Fail to build mongo-cxx-driver as root"
else
  make -C build install
  check_exit_failure "Fail to build mongo-cxx-driver"
fi

echo ending install of mongo cxx

## uninstall with :
#if [ `command -v sudo` ]; then
# sudo cmake -B build
#else
# cmake -B build
#fi
# sudo make -C build uninstall

## or :

#if [ `command -v sudo` ]; then
# sudo /opt/mongo/share/mongo-cxx-driver/uninstall.sh
# else
# /opt/mongo/share/mongo-cxx-driver/uninstall.sh
# fi