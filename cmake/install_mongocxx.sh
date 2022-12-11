## Instructions found at https://mongocxx.org/mongocxx-v3/installation/linux/

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
  sudo apt-get install libssl-dev libsasl2-dev
  sudo apt-get install libmongoc-dev
  sudo apt-get install libmongoc-doc
# sudo apt-get install libstdc++-10-dev
  sudo apt-get install libstdc++-12-dev
  sudo apt-get install libmongoc-1.0-0
  sudo apt-get install libbson-1.0-0

#else
#  apt-get update
  apt-get install libssl-dev libsasl2-dev
  apt-get install libmongoc-dev
  apt-get install libmongoc-doc
# apt-get install libstdc++-10-dev
  apt-get install libstdc++-12-dev
  apt-get install libmongoc-1.0-0
  apt-get install libbson-1.0-0
fi


## install C++ driver
### use at least CPP 11
### from https://github.com/mongodb/mongo-cxx-driver/releases
### then follow instructions from step 4 of https://mongocxx.org/mongocxx-v3/installation/linux/

if [ ! -f $INSTALL_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz ]; then
  curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r$MONGOCXX_VERSION/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
  mv mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz $INSTALL_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
fi

cd $INSTALL_DIR/
if [ ! -d mongo-cxx-driver-r$MONGOCXX_VERSION ]; then
  tar -xzf mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
fi
cd mongo-cxx-driver-r$MONGOCXX_VERSION/

## note that polyfill steps have been skipped

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/mongo
if [ `command -v sudo` ]; then
  sudo make -C build install
else
  make -C build install
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