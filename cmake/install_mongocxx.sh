# Instructions found at https://mongocxx.org/mongocxx-v3/installation/linux/

if [ ! $MONGOCXX_VERSION ]; then
  MONGOCXX_VERSION="3.7.0"
fi

if [ ! $STORAGE_DIR ]; then
  STORAGE_DIR="third_parties"
fi


# Install C driver
sudo apt install libmongoc-dev
sudo apt install libmongoc-doc
## use at least CPP 11

#sudo apt install libstdc++-10-dev
sudo apt install libstdc++-12-dev

# install C++ driver
## from https://github.com/mongodb/mongo-cxx-driver/releases
## then follow instructions from step 4 of https://mongocxx.org/mongocxx-v3/installation/linux/

if [ ! -f $STORAGE_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz ]; then
  curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r$MONGOCXX_VERSION/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
  mv mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz $STORAGE_DIR/mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
fi

cd $STORAGE_DIR/
tar -xzf mongo-cxx-driver-r$MONGOCXX_VERSION.tar.gz
cd mongo-cxx-driver-r$MONGOCXX_VERSION/

## note that polyfill steps have been skipped

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/mongo ## WORKS
sudo make -C build install ## WORKS

## uninstall with :
# sudo cmake -B build
# sudo make -C build uninstall
## or :
# sudo /opt/mongo/share/mongo-cxx-driver/uninstall.sh