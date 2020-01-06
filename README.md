# mer, a C compiler.

Actually, a [C0](http://c0.typesafety.net) compiler: C with fewer features but more safety.

A quick overview:

 * Outputs Mach-O x86-64 assembly for macOS
 * Compiles only single files, the output of which must be linked with the mer runtime (see [usage](#usage) below)
 * Supports booleans, 64-bit unsigned integers, and structs and arrays thereof (no floats)
 * Supports all usual integer arithmetic and bitwise operators apart from left and right shift
 * Includes array bounds checking: accessing an array out of bounds results in a `SIGUSR2` rather than undefined behaviour
 * Neither casting nor the address-of (`&`) operator are supported
 * Structs and arrays must always be heap allocated
 * A notable amount of control flow syntax is missing. None of `switch, case, break`, or `continue` are yet implemented
 * Only the most basic of compiler optimizations are performed (currently only redundant move removal)

## Architecture

Intermediate languages: `C0 -> CST -> AST -> IRT -> Inst -> x86`

Pipeline stages:

    C0   -- (parse) -->
    CST  -- (elaborate) -->
    AST  -- (translate) -->
    IRT  -- (intermediate codegen) -->
    Inst -- (register allocation & codegen) -->
    X86

 1. The parser outputs a Concrete Syntax Tree (CST)
 2. The CST is elaborated to a set of Abstract Syntax Trees (AST), one for each function.
    * Elaboration lowers e.g. `a->b` to `(*a).b` and performs some basic type checking
    * Type inference / checking and  static code analysis are performed on the AST
 3. Each AST is translated first to a higher intermediate representation: an Intermediate Representation Tree (IRT)
    * This stage lowers control flow statements to jumps and labels but preserves the tree structure of expressions
 4. Intermediate code generation lowers IRTs to instruction language, a lower intermediate representation
    * Instruction language is a three-address-code linear representation, similar in form to a RISC instruction set
    * Language has an unbounded number of temporary "registers"
 5. Register allocation is performed on instructions before code generation finally emits x86-64 assembly

The output of each stage (parsing, elaboration, translation, intermediate code generation, register allocation and code generation) can be viewed by adding a `-parse, -ast, -hir, -lir, -regalloc, -asm` flag (respectively) when running the compiler (see [usage](#usage) below).

The visitor pattern is used (possibly overused) to traverse all recursive data types in the compiler (roll on [pattern matching](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1371r1.pdf) in C++!).

# References

 1. Register allocation is based on the algorithm in [Register Allocation via Coloring of Chordal Graphs](http://web.cs.ucla.edu/~palsberg/paper/aplas05.pdf) by Pereira et al.

     The algorithm relies on the fact that chordal graphs can be optimally colored in polynomial time by finding a simplicial elimination ordering of the vertices.
     Further, graphs that are *almost* chordal (in some sense) can be *almost* optimally colored using the same SEO algorithm 

 2. Much of the implementation of the "middle end" (static analysis and code generation) of mer is based on content from the [Tiger Book](https://www.cs.princeton.edu/~appel/modern/ml/) by Andrew Appel, and also 
    the [15-411](https://www.cs.cmu.edu/~janh/courses/411/18/index.html) course at CMU
 
 3. Expression parsing is done via Pratt parsing, inspired by [this](http://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/) article by Bob Nystrom. Otherwise, parsing is done via recursive descent


## TODO

 * More targets: RISC-V, MIPS, LLVM IR?
 * More compiler optimizations
 * Garbage collection
 * Implement remaining control flow (`switch, case, break`, and `continue`)
 * Implement pre-spilling from Pereira et al. to obtain better graph colorings in register allocation
 * Implement a preprocessor and allow compilation of multiple files
 * More tests

## Build

Requires CMake and a modern C++ compiler (tested with `clang-1000.10.44.4`)

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

## Test

Run `$ test/run`. Many tests are taken from [here](https://github.com/kevinburg/zinc).

<a name="usage"></a>
## Usage

e.g. `$ build/mer my-src-file.c` prints the corresponding x86 to stdout on successful compilation.
Redirect this output to e.g. `out.s` and run `gcc -m64 out.s runtime/mer.c` to obtain an executable. i.e.

    $ build/mer my-src-file.c > out.s  # compile
    $ gcc -m64 out.s runtime/mer.c     # link with runtime and output executable

To see debug output of intermediate compiler stages, add the corresponding flag as below

    $ build/mer my-src-file.c -(parse|ast|hir|lir|regalloc|asm)