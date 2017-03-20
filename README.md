## Rsyn
Rsyn is an open-source C++ framework for physical synthesis (e.g placement, routing, sizing, buffering, restructuring, etc)
research and education.  The framework integrates parsers for common academic and industrial formats as Bookshelf, LEF/DEF, 
Verilog, Liberty, SDC and SPEF. Benchmarks from several EDA contests from ISPD and ICCAD can be easily loaded. 

It also includes standard tools as static timing analysis (STA), routing and congestion estimation. 
A built-in graphics user interface (GUI) is also available.

![Rsyn](https://raw.githubusercontent.com/rsyn/rsyn/master/doc/rsyn-gui.png)

The netlist data model makes use of the new features of C++11. So it becomes very natural and easy to traverse the netlist.
```cpp
// Traverse pins in reverse topological order.
for (Rsyn::Pin pin : module.allPinsInReverseTopologicalOrder()) {
	for (Rsyn::Arc arc : pin.allOutgoingArcs()) { /* … */ }
	for (Rsyn::Arc arc : pin.allIncomingArcs()) { /* … */ }

  Rsyn::Net net = pin.getNet();
  if (!net) {
	  std::cout << “Unconnected pin ‘” + pin.getFullName() + “‘\n”;
  } // end if
} // end for 
```

Objects can be extended via attributes, which internally handle modifications in the netlist.

```cpp
// Creating a new attribute called “visited” to the nets
Rsyn::Attribute<Rsyn::Net, int> attr = design.createAttribute();

for (Rsyn::Net net : module.allNets()) {
  attr[net] = computeSomething(net); 
} // end for
```

Rsyn::Net newNet = module.createNet();
attr[newNet] = computeSomething(net);

## Index

- [Running Rsyn](#running-rsyn)
- [License](#license)
- [Reference](#reference)
- [Creators](#creators)
   - [Contributors](#contributors)
   - [Former Contributors](#former-contributors)
- [Contact](#contact)

## Running

Soon...
 
## Reference

Please cite the ISPD 17 paper:

G. Flach, M. Fogaça, J. Monteiro, M. Johann and R. Reis, ["Rsyn: An Extensible Physical Synthesis Framework"](http://doi.acm.org/10.1145/3036669.3038249), Proceedings of the 2017 ACM on International Symposium on Physical Design (ISPD), pp. 33-40, Portland, Oregon, USA 2017. 

## Creators
- [Guilherme Flach](mailto:guilherme.augusto.flach@gmail.com )
- [Mateus Fogaça](mailto:mateus.p.fogaca@gmail.com)
- [Jucemar Monteiro](mailto:jucemar.monteiro@gmail.com)

#### Contributors
- [Henrique Placido](mailto:hplacido@inf.ufrgs.br)
- [Isadora Oliveira](mailto:isoliveira@inf.ufrgs.br)

#### Former Contributors
  Julia Puget, André Oliveira, Carolina Lima, Felipe Pinto and Lucas Cavalheiro

## Contact

  [rsyn.design@gmail.com](mailto:rsyn.design@gmail.com)