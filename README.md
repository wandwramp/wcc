# `wcc`

`wcc` is a C compiler targeted at the WRAMP architecture. It is based on
`lcc`, a retargetable compiler. Copyright information for `lcc` can be found
in [CPYRIGHT](CPYRIGHT).

## Usage

Using `wcc` requires binaries for `wcpp`, `rcc`, and `wasm` to be in the
directory referenced by `LCCDIR`, defined in [linux.c](etc/linux.c).
`wcpp` and `rcc` are built by this project, and `wasm` can be found in wasm.

Help for `wcc` can be found by running `wcc -h`, but a few particularly
useful arguments are listed below.

```
-c	compile only
-o file	leave the output in `file'
-S	compile to assembly language
```

`-c` can be used to manually link the output with files written in assembly.
`-S` allows you to view the generated code.

## Building

First, ensure that `LCCDIR` is defined in [etc/linux.c](etc/linux.c) to a reasonable
location in which it is possible to place the output binaries.
A good example is the build/ folder of the `wcc` directory.  
The [makefile](makefile) contains several build targets, many of which are
required for `wcc` to run properly. Run `make all` to build everything.
