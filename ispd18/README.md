Rsyn ISPD18 - Detailed Routing Contest
======================================

In this page, you find instruction on how to use Rsyn for ISPD18 Detailed Routing Contest.

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

## Routing Guides

You can visualize the routing guides of a net by clicking on the net or searching by its name in the search box in the Design tab.

![Rsyn](https://github.com/rsyn/rsyn-x/blob/master/rsyn/doc/image/routing-guides-visualization.png)

See ExampleService::doSomething() to check how to access routing guides from the code.

## Transforms

Rsyn::PhysicalTransform allows one to handle cell orientation as shown in the image below.

![Rsyn](https://github.com/rsyn/rsyn-x/blob/master/rsyn/doc/image/physical-transform.png)

## Routing Grid

The routing grid is represented by the class Rsyn::PhysicalRoutingGrid, which is accessed  via Rsyn::PhysicalDesign.

![Rsyn](https://github.com/rsyn/rsyn-x/blob/master/rsyn/doc/image/tracks-visualization.png)

## Routing

The routing of a net is defined via Rsyn::PhysicalRouting.

```cpp
Rsyn::Session session;
Rsyn::PhysicalDesign physicalDesing = session.getPhysicalDesign();

Rsyn::Net net = ...

Rsyn::PhysicalRouting routing;
routing.addWire(layer, p0, p1);
routing.addWire(layer, p2, p3);
physicalDesign.setNetRouting(net, routing);
```

## Writing DEF

The DEF can be written via the command writeDEF.

```cpp
Rsyn::Session session;
session.evaluateString("writeDEF");
```
