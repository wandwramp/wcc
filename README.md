# `wcc`

`wcc` is a C compiler targeted at the WRAMP architecture. It is based on
`lcc`, a retargetable compiler. Copyright information for `lcc` can be found
in [CPYRIGHT](CPYRIGHT).

## Usage

Using `wcc` requires binaries for `wcpp`, `rcc`, and `wasm` to be either in the
PATH or in the same directory as the built binaries.
`wcpp` and `rcc` are built by this project, and `wasm` can be found in
wasm.

Help for `wcc` can be found by running `wcc -h`, but a few particularly
useful arguments are listed below.

```
-c          compile only
-o file     leave the output in `file'
-S          compile to assembly language
```

`-c` can be used to manually link the output with files written in assembly.  
`-S` allows you to view the generated code.

## Building

The [makefile](makefile) contains several build targets, many of which are
required for `wcc` to run properly. Run `make all` to build everything.
By default binaries will be placed in a bin folder within the same directory as
the makefile, otherwise `make install` will place the binaries in the folder 
specified in the makefile, defaulting to `~/wramp-install/`.
