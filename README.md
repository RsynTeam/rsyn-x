<a href="http://ufrgs.br">
    <img src="https://raw.githubusercontent.com/rsyn/rsyn-x/master/x/media/ufrgs-logo.png" height=59 alt="Universidade Federal do Rio Grande do Sul" title="UFRGS" align="right"/>
</a>

Rsyn
======================
[Getting Started](https://github.com/rsyn/rsyn-x/wiki/Getting-Started) | [Tutorials](https://github.com/rsyn/rsyn-x/wiki#tutorials) |  [Wiki](https://github.com/rsyn/rsyn-x/wiki)

**Rsyn is an open-source C++ framework for physical synthesis research and education.**

Typical optimizations that can be built on top of Rsyn include but not limited to placement, routing, sizing, buffering, restructuring.

The framework integrates parsers for common academic and industrial formats as Bookshelf, LEF/DEF, 
Verilog, Liberty, SDC and SPEF. It provides support for benchmarks from several EDA contests (e.g ISPD, ICCAD).

Standard tools as static timing analysis (STA), routing and congestion estimation are included. 
A built-in graphics user interface (GUI) is also available.

![Rsyn-gui](https://raw.githubusercontent.com/rsyn/rsyn/master/doc/rsyn-gui.png)

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

Rsyn::Net newNet = module.createNet();
attr[newNet] = computeSomething(net);
```
## Index

- [Reference](#reference)
- [Creators](#creators)
   - [Contributors](#contributors)
   - [Former Contributors](#former-contributors)
- [Third-Party Projects](#third-party-projects)
- [Contact](#contact)
- [License](#license)
- [Acknowledgement](#acknowledgement)

**More details about Rsyn are available at [wiki](https://github.com/rsyn/rsyn-x/wiki).**

## Reference

Please cite the ISPD 17 paper:

G. Flach, M. Fogaça, J. Monteiro, M. Johann and R. Reis, ["Rsyn: An Extensible Physical Synthesis Framework"](http://doi.acm.org/10.1145/3036669.3038249), Proceedings of the 2017 ACM on International Symposium on Physical Design (ISPD), pp. 33-40, Portland, Oregon, USA 2017. 

## Contributors

#### Creators
- [Guilherme Flach](mailto:guilherme.augusto.flach@gmail.com )
- [Mateus Fogaça](mailto:mateus.p.fogaca@gmail.com)
- [Jucemar Monteiro](mailto:jucemar.monteiro@gmail.com)

#### Advisors
- [Marcelo Johann](mailto:johann@inf.ufrgs.br)
- [Ricardo Reis](mailto:reis@inf.ufrgs.br)

#### Contributors
- [Henrique Placido](mailto:hplacido@inf.ufrgs.br)
- [Isadora Oliveira](mailto:isoliveira@inf.ufrgs.br)

#### Former Contributors
 Tiago Reimann, Julia Puget, André Oliveira, Carolina Lima, Felipe Pinto, and Lucas Cavalheiro

###### Honorable Mention
 Renato Hentschke, and Gustavo Wilke
 
## Third-Party Projects

![third-projects](https://github.com/rsyn/rsyn/blob/master/doc/third-parties.png)

## Contact

  [rsyn.design@gmail.com](mailto:rsyn.design@gmail.com)

## License

 Copyright 2014-2017 Rsyn
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 ## Acknowledgement
 
 This work is partially supported by Brazilian Coordination for the Improvement of Higher Education Personnel
([CAPES](http://www.capes.gov.br/)) and by the National Council for Scientific and Technological Development ([CNPq](http://cnpq.br/))
