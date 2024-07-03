## TAOCP v4 Dancing Link

### Advanced Data Structures and Algorithm: Dancing Links

Dancing link is an advanced data strucuture for backtrack algorithms.  It is
very efficient for _undo_ operations. One example for its use cases is solving
Exact Cover Problems.

Reference Books and Papers
- [VOLUME 4B](https://www.amazon.com/Art-Computer-Programming-Combinatorial-Information/dp/0201038064).
- [VOLUME 4 PRE-FASCICLE 5C](https://www.amazon.com/Art-Computer-Programming-Fascicle-Preliminaries/dp/0134671791).
- [Dancing Links][1] Paper.

### Use Case: Sudoku

See code, [`cmd/sudoku/main.cc`](cmd/sudoku/main.cc), for an interesting example
demonstrates Dancing Links algorithm. It is based on Dancing Links [1] Paper by
Donald E. Knuth.

[1]: https://arxiv.org/pdf/cs/0011047.pdf
