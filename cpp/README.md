# rootio
Pure ROOT I/O, where I/O extends only up to getting ROOT Records into memory. No deserialization/serialization, or analysis capabilities. Only I/O, memory handling.

## Core
Core is written in `c`. And provides the most fundamental and basic facilities to interact with a _ROOT_ file structure. Ability to read/write `ROOT` records. A few examples are already provided (in _tests_ folders in the source tree) showcasing various capabilities even at this low level, which at the moment requires being explicit and a lot of boiler plate. This is to be removed at a higher level abstraction.

### Core Roadmap
- dev: hdfs support + tests. __see core/hdfs/tests__
- dev: eos support + tests
- docs: outline and distinguish physical and logical structure of the ROOT storage
- docs: outline what the Core actually aims to provide

## Core Extensions
Given core implemented in c, core extensions can then be further developed in any other language/platform. Languages like `rust` and `python` (which have foreign function interfaces) have already been tried. Most of the focus will be devoted to `c++`.

###  Core Extensions Roadmap
- dev: build on top of the physical layer (Keys / Records) and complement/create logical layer (Directories)
- dev: tests with arbitrary nestedness of ROOT Directories of Records inside
 - simple interfaces for key generations given a binary blob
 - simple directories and key lists generation
