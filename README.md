# async-comms-example
Test repo to develop different async comms approaches

## Setup

### Google Test

Google test can be added to your git repo as a git submodule.

```console
$ git submodule add https://github.com/google/googletest.git ./vendor/gtest
```

and then adding it to the CMakeList

```cmake
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/gtest")
```

Also if we are using C++20 in our project, we have to ensure that C++20 is used adding the following line in the root CMakeList file

```cmake
set(CMAKE_CXX_STANDARD 20)
```

### Fake Function Framework (FFF)

Fake Function Framework can be added to your git repo as a git submodule.

```console
$ git submodule add https://github.com/meekrosoft/fff.git ./vendor/fff
```

and then adding it to the CMakeList

```cmake
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/fff")
```

### Valgrind

To install valgrind we can use hte package manager

```console
sudo apt-get update
sudo apt-get -y install valgrind
```

## Build

### Release

To build the release, first we launch CMake and then make.

```console
cmake . -H. -Bbuild
cd build
make -j3
```

All in one line:

```console
cmake . -H. -Bbuild; cd build; make -j3; cd ..
```

### Unit tests

The unit tests will be launched using **ctest**

To build the unitary tests, first we launch CMake with the UTEST custom option and then make.

```console
cmake . -DUTEST=ON -H. -Bbuildtest
cd buildtest
make -j3
```

To launch **all** the tests:

```console
ctest -VV;
```

To launch only a testsuite:

```console
ctest -R "testsuite_name" -VV
```

To use **valgrind** for checking memory leaks:

```console
ctest -T memcheck -VV
```

All in one line:

```console
cmake . -DUTEST=ON -H. -Bbuildtest; cd buildtest; make -j3 && ctest -T memcheck -VV; cd ..
```

### Build C4 diagrams

The C4 diagrams are made using [PlantUML](https://plantuml.com/). First, you have to install the `plantuml` package:

```console
sudo apt-get update
sudo apt-get install plantuml
```

Then, to build the diagrams, you only have to run the `plantuml` command:

```console
plantuml -tsvg <path-to-the-files>
```
