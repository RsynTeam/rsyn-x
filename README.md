# Rsyn <img src="https://raw.githubusercontent.com/rsyn/rsyn-x/master/x/media/ufrgs-logo.png" width=80> 

Rsyn is an open-source C++ framework for physical synthesis (e.g placement, routing, sizing, buffering, restructuring, etc)
research and education.  The framework integrates parsers for common academic and industrial formats as Bookshelf, LEF/DEF, 
Verilog, Liberty, SDC and SPEF. Benchmarks from several EDA contests from ISPD and ICCAD can be easily loaded. 

It also includes standard tools as static timing analysis (STA), routing and congestion estimation. 
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

- [Running](#running)
- [Reference](#reference)
- [Creators](#creators)
   - [Contributors](#contributors)
   - [Former Contributors](#former-contributors)
- [Third-Party Projects](#third-party-projects)
- [Contact](#contact)
- [License](#license)
- [Acknowledgement](#acknowledgement)


## Running

Rsyn was tested in Ubuntu 16.04.2 LST and compiled with g++ 5.4.0.
```sh
$ git clone https://github.com/rsyn/rsyn-x.git
$ cd rsyn-x/x/bin/
$ sudo ../../rsyn/script/install-libs.sh
$ make all 
$ make rgui 
```
[Bash script to install Rsyn dependencies](https://github.com/rsyn/rsyn-x/blob/master/rsyn/script/install-libs.sh)

#### Opening a design in Rsyn Graphics User Interface:
 * File -> Run Script (ctrl+r)
 * Go to *rsyn-x/x/demo/simple* directory
 * Open simple.rsyn file
 * Rsyn GUI shows the simple circuit

###### Tip: 
* Selecting a cell: *shift+left click*
* Moving a cell: Keep *shift key* pressed ->  *left click* on a cell -> *move the mouse*

#### Running with command-line mode:	
  * GUI is not available in command-line mode

```sh
$ make rscript script=../demo/simple/simple.rsyn
```

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
 Tiago Reimann, Julia Puget, André Oliveira, Carolina Lima, Felipe Pinto and Lucas Cavalheiro

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
