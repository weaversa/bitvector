[![Build Status](https://travis-ci.com/weaversa/bitvector.svg?branch=master)](https://travis-ci.com/weaversa/bitvector)

# A Simple C Bitvector Library

This library assists in the construction and manipulation of
fixed-width bitvectors.

The main features of this library are (so far) the ability to slice
and reshape bitvectors as well as support for equality testing and the
Boolean operations AND, OR, and XOR. The full list is available here:
https://github.com/weaversa/bitvector/blob/master/src/bitvector.c


# SAW Proof

There are formal correctness proofs for every function, written in
(saw-script)[https://github.com/GaloisInc/saw-script]. However, the
proofs are only able to be done at specified bitvector widths.
