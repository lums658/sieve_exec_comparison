# sieve_exec_comparison

Multiple asynchronous implementations of sieve of Eratosthenes

## Prerequisites

### concurrencpp 

One of the implementations uses the concurrencpp library.  
Get the latest version of concurrencpp as a submodule (the
Makefile expects it to be installed as a subdirectory).

```bash
  $ git submodule update --init --recursive
```

Build the library:
```bash
  $ cd concurrencpp
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```

That should create a library `libconcurrencpp.a` in the `concurrencpp/build` subdirectory.

### TBB

You will need a recent version of Intel Threading Building Blocks -- oneTBB.  The most
straightforward way to access that is to install it with the appropriate package manager 
(brew on MacOS, apt on Ubuntu, etc).

### Makefile


