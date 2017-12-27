Rsyn ISPD18 - Detailed Routing Contest
======================================

In this page, you find instruction on how to use Rsyn for ISPD18 Detailed Routing Contest.

## Binary
TODO

## Getting Started

### Source Code

```cpp
git clone https://github.com/rsyn/rsyn-x.git
```

### Dependencies

```cpp
cd rsyn-x/ispd18
./script/install-dependencies.sh
```

### Compiling Rsyn using Cmake

```cpp
cd rsyn-x/ispd18
mkdir build
cd build
cmake ..
make -j 4
```

## NetBeans

Open the project at rsyn-x/ispd18/ide/netbeans.

**Configure NetBeans to use multi-threading compilation:**

Tools -> Options -> C/C++ -> Project Options

Type "-j 4" in "Make Options".

## Opening a Benchmark using Rsyn

```cpp
./rsyn
Ctrl+R
Select file "rsyn-x/ispd18/sample/ispd18_sample/ispd18_sample.rsyn"
```

![Rsyn](https://github.com/rsyn/rsyn-x/blob/master/ispd18/media/rsyn-screenshot-01.png)

