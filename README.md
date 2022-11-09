# Repository name

## Build

```shell
mkdir "build"
cmake  -S . -B build
```

## Run

```shell
./executable
```

## Test

### Unit tests

#### Build

```shell
mkdir "build"
cmake -D unit_tests=true -S . -B build
make -C build unit_tests
```

#### Run

```shell
## either :
./unit_tests
## or :
cd build
ctest
```

### Functional tests

#### Build

```shell
mkdir "build"
cmake -D functional_tests=true -S . -B build
make -C build functional_tests
```

#### Run

```shell
./functional_tests
```

## Learn

### Documentation

The doxygen html documentation should be found in branch gh-pages
(a GitHub action generates and pushes it when commits are pushed on to develop or main)

#### Generate manually

```shell
cmake -D doxygen=true -S . -B build
make -C build doxygen
```

The code documentation should be generated under directory ***[./docs/Doxygen](./docs/Doxygen)***

### Contribute

See [Contribute.md](./Contribute.md), for explanations about how this repo works.