# async-comms-example
Test repo to develop different async comms approaches

## Setup

### Google Test

Google test can be added to your git repo as a git submodule.

```console
$ git submodule add https://github.com/google/googletest.git ./vendor/gtest
```

Also if we are using C++20 in our project, we have to ensure that C++20 is used adding the following line in the root CMakeList file

```cmake
set(CMAKE_CXX_STANDARD 20)
```

### Valgrind

To install valgrind we can use hte package manager

```console
sudo apt-get update
sudo apt-get -y install valgrind
```

## Build

### Build C4 diagrams

The C4 diagrams are made using [PlantUML](https://plantuml.com/). First, you have to install the `plantuml` package:

```shell
sudo apt-get update
sudo apt-get install plantuml
```

Then, to build the diagrams, you only have to run the `plantuml` command:

```shell
plantuml -tsvg <path-to-the-files>
```
