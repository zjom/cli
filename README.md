# Simple C Implementations of command line tools

This repository contains simple implementations of common command line tools in C.
I have implemented these tools to learn more about the C programming language and to get a better understanding of how these tools work.

## Tools

- [x] cat
- [x] echo
- [x] grep
- [x] ls
- [x] tail
- [ ] wc


## Building

To build the tools, simply run `make` in the root directory of the repository. This will create a `bin` directory containing the compiled tools.
To build a specific tool, run `make <tool>` where `<tool>` is the name of the tool you want to build.

## Usage
do what you usually do with the tools
I probably have different flags tho so check the help message with `bin/<tool> -h`


You can also pipe the tools together like so:
```sh
bin/ls -A | bin/grep README | bin/cat | bin/echo
```
