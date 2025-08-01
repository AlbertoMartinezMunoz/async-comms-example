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

### Robot Framework

#### Python

Install Python3

```shell
sudo apt update
sudo apt install python3
```

Install pip3

```shell
sudo apt-get install python3-pip
python3 -m pip install --user --upgrade pip
```

#### Python Virtual Environment

Install venv

```shell
sudo apt-get install python3-venv
```

The scripts in this folder are designed to be set up using a Python virtual environment (`venv`). The first time you want to set up the application on Linux, execute the steps below on a shell:

```sh
# Set up a virtual environment
python3 -m venv .venv

# Activate it
. .venv/bin/activate

# Install necessary packages and upgrade
pip install -r requirements.txt --upgrade
```

Please do _not_ use `sudo` within a virtual environment; it will not work as intended. Instead, set up your system correctly so that you do not need root access to run the script.

**To use the application again at a later time, you only need to perform the activation step.**

#### Requirements File

**While in the python virtuel anvironment**, you can use the following code to generate / update a pip requirements.txt file:

```console
(.venv) async-comms-example$ pip freeze > requirements.txt
```

#### Install Robot Framework

Robot Framework is implemented with Python, so you need to have Python installed.

**While in the python virtuel anvironment**, installing Robot Framework with pip is simple:

```console
(.venv) async-comms-example$ pip install robotframework
```

To check that the installation was successful, run

```console
robot --version
```

### pre-commit

Git hook scripts are useful for identifying simple issues before submission to code review. We run our hooks on every commit to automatically point out issues in code such as missing semicolons, trailing whitespace, and debug statements. By pointing these issues out before code review, this allows a code reviewer to focus on the architecture of a change while not wasting time with trivial style nitpicks.

The whole documentation in on the website [pre-commit](https://pre-commit.com/)

#### Installation

Before you can run hooks, you need to have the pre-commit package manager installed.

Using pip:

```console
pip install pre-commit
```

#### Setup

1. Add a pre-commit configuration

> create a file named .pre-commit-config.yaml you can generate a very basic configuration using `pre-commit sample-config > .pre-commit-config.yaml`

3. Run against all the files

> Before installing, it's usually a good idea to run the hooks against all of the files when adding new hooks

3. Install the git hook scripts

> run `pre-commit install` to set up the git hook scripts

### clang-format

The `clang-format` tool will be used to format the c++ files. To install clang-format:

```console
sudo apt install clang-format
```

There some default styles when formatting, but you can modify them using the `.clang-format` configuration file and the option `-style=file`

It will be used to format the c++ files before commiting them to the repo. To manually run it:

```console
find ./src -iname '*.h' -o -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i -style=file
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

### Acceptance Tests

To run the robot framework acceptance tests, **you have to activate the python virtual environment** and run:

```console
robot --loglevel TRACE:INFO --exclude EXCL --skip SKIP --removekeywords tag:REMOVE --flattenkeywords tag:FLAT --outputdir ./atest-results ./atest/
```

### Build C4 diagrams

The C4 diagrams are made using [PlantUML](https://plantuml.com/). First, you have to install the `plantuml` package:

```console
sudo apt-get update
sudo apt-get install plantuml
```

Then, to build the diagrams, you only have to run the `plantuml` command:

```console
plantuml -tpng <path-to-the-files>
```
