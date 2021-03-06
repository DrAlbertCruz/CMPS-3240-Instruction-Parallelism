# CMPS-3240-Instruction-Parallelism

CMPS 3240 Computer Architecture: Instruction level parallelism in x86-64

# Introduction

## Requirements

* Use ``odin.cs.csubak.edu`` or your own machine with x86 intrinsics but not ``sleipnir.cs.csubak.edu``

## Prerequisites

* Completed lab on sub-word parallelism
* Understanding of DGMM and DAXPY operations

## Objectives

* Improve DAXPY implementation with loop unrolling
* Observe changes made in 

## Background

Instruction-level parallelism makes use of loop unrolling to reduce the number of delays due to branch prediction. At the end (or start) of a for loop a check is carried out to determine if a loop should keep going or not. 
```c
for(i=0;i<someLimit;i++) {
   // do something
}
```
Consider how this would translate to MIPS:
```mips
looptop: beq $counter $limit loopexit
# do something
j looptop
loopexit:
```
A branch delay would be caused at the start of every loop body. This would cause a real delay and also a hazard. So, to improve execution speed of the loop body we should try to reduce the amount of times we carry out a check. Consider, *how many times do you actually need to carry out this check?* Not often. Actually, it really only needs to exit the loop at the very end. We could try this:
```mips
# something 1
# something 2
# something 3
# and so on until our wrists are tired from having written out 1000 loop executions
```
but this has limited usefulness. It would also cause the code to have a hard-coded number of times to execute ``something``. There is indeed a middle ground, where we can actually execute multiple ``somethings`` within a single iteration of the loop body:
```c
for(i=0;i<someLimit;i++) {
// do something 1
// do something 2
// do something 3
// do something 4
}
```
This is a good middle-ground, and it's what we will implement in this lab. Your goal is to improve your previous DAXPY code with loop unrolling.

# Approach

This lab is structured similarly to the last lab. First, you will study the DGEMM implementation of AVX with loop unrolling. Then, you will apply what you have learned to improve your DAXPY AVX implementation with loop unrolling.

## Part 1 - DGEMM

Clone this lab:

```terminal
$ https://github.com/DrAlbertCruz/CMPS-3240-Instruction-Parallelism.git
```

Then run the make file:

```terminal
$ make all
```

You should get something like this:

```terminal
$ make all
gcc -mavx -std=c99 -c unopt_dgmm.c
gcc -mavx -std=c99 -O0 -o unopt_dgmm.out unopt_dgmm.o
gcc -mavx -std=c99 -c unroll_dgmm.c
gcc -mavx -std=c99 -O3 -o unroll_dgmm.out unroll_dgmm.o
gcc -S -mavx -std=c99 -o unopt_dgmm.asm -c unopt_dgmm.c
gcc -S -mavx -std=c99 -o unroll_dgmm.asm -c unroll_dgmm.c
```

Consider only the first four lines (we will get back to the last two lines, which generate x86 assembly code later). If you get errors, either (1) your processor doesn't have AVX or (2) you're on a Windows machine. It is possible to compile this on a Windows machine, you just need to look at the original AVX lab to see what changes need to be made. Note the following things:

1. The unoptimized version continues to use ``-O0`` from the last time we did AVX. Normally the compiler will attempt to optimize the code with the strategies we are learning about. ``-O0`` will prevent it from doing so. Hence, it is unoptimized.
1. The optimized version (`unroll_dgmm.c`) has the flag ``-O3``. This allows the compiler to optimize your code. It is helpful but we need to do one more thing ...
1. The optimized version (`unroll_dgmm.c`) also has the flag ``funroll-loops``. This allows the compiler to unroll loops where the number of iterations can be determined during compile time ...
1. Previously, we allowed the user to specify the size of the array. However, for `gcc`'s unroll to work, it needs to know the size of the arrays during compile time. This version of `unroll_dgmm.c` has the matrixes located in the stack, versus the heap. I.e. it does not use `malloc`, and the user cannot specify the size of the operation during run time.

With ``-O3`` and ``funroll-loops`` flags the compiler will tend to unroll stuff for us, but we will take additional steps to help with the unroll. Open ``unroll_dgmm.c`` and look at the following definition:

```c
#define UNROLL (4)
```

and:

```c
for ( int i = 0; i < n; i+=UNROLL*4 )
...
__m256d c[4];
for ( int x = 0; x < UNROLL; x++ )
...
```

We change the loop body to execute four times, rather than once. Study the code, and when you're confident you undertand what's going one time the results. *Again, note that you can no longer specify the size of the arrays during run time because of gcc needing to know the number of loop iterations during compile time*.

```term
$ time ./unopt_dgmm.out
Running matrix multiplication of size 512 x 512
real    0m0.634s
user    0m0.625s
sys    0m0.005s
$ time ./unroll_dgmm.out
Running matrix multiplication of size 512 x 512
real    0m0.276s
user    0m0.269s
sys    0m0.004s
```
This verifies the improvement at a high-level. 

### Sidebar

*This section is not necessary to complete the lab. It provides instructions for the curious who may want to see how the unrolling occured at the assembly level*. To view the improvement at a low-level you will need to take a look at the ``.asm`` files that were generated from the make file. The exact implemention of the unroll will vary depending on your environment (Mac, Windows, Linux), and which version of GCC you are using. You will want to compare `unopt_dgmm.asm` and `unroll_dgmm.asm` and find places where `unopt_dgmm.asm` repeated a chunk of code four times. When you're satisfied with your understanding of loop unrolling with DGEMM, proceed to the next section.

## Part 2 - DAXPY

As with the last AVX lab, your job is to apply what you have studied with a different operation, double-precision constant times a vector plus a vector (DAXPY):

D = a * X + Y

Where D, X and Y are vectors (not matrixes this time) of the same size, and a is a scalar. The pseudo-code is repeated here for convienience:

```
for i from 0 to length of the vectors 
d[i] <- a * x[i] + y[i] 
endfor
```
Feel free to build off your last AVX lab's DAXPY code. 

# Discussion

Include responses to the following questions in your lab report:

1. With DGEMM, why did we have to change ``__m256d c;`` to ``__m256d c[4];`` after unrolling?
2. Investigate the flag(s) that can be used to have the gcc compiler automatically unroll loops for you. What are the pros and cons?
3. Can the you run this experiment if the vector lengths are given during run time?
