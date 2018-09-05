# MaxSAT Solver

[![Build Status](https://travis-ci.com/GoodDeeds/MaxSAT-Solver.svg?branch=master)](https://travis-ci.com/GoodDeeds/MaxSAT-Solver)

A unweighted MaxSAT Solver that uses Minisat2.2 as its backbone.

This solver implements Sequential Encoding to encode cardinality constraints and uses the LUS search algorithm.

## Installation

### Prerequisites

* g++ (or any other C++ compiler)
* minisat2.2 (included as a submodule)
* GNU make

### Building the project

* Clone the repository
```
$ git clone --recurse-submodules https://github.com/GoodDeeds/MaxSAT-Solver.git
$ cd MaxSAT-Solver
```

* Compile the program
```
$ make
```

If you use a different compiler, please edit the Makefile accordingly.

## Running the solver

The solver accepts input from standard input (STDIN) and sends output to the standard output (STDOUT).

### Input format
The input is a SAT formula is DIMACS format. A detailed description can be found [here](http://www.satcompetition.org/2009/format-benchmarks2009.html).

### Output format
The first line of the output is a single integer that specifies the number of clauses that could be satisfied. The second line is any assignment that satisfies the maximum number of clauses satisfiable. It consists of space separated boolean variables in ascending order, where the variables have a negative sign if assigned false and no negative sign if assigned true. The last variable is followed by a space and then a `0`.

### Running the solver
If the input is in a file input.cnf, use
```
$ ./solver < input.cnf
```

### Example
Let the input be
```
c 3 variables, 6 clauses
p cnf 3 6
1 2 0
1 -2 0
3 2 0
-3 1 0
1 2 3 0
-1 0
```

The output is
```
5
-1 2 -3 0
```

Here, the 5 clauses in the formula are satisfiable. Variable `2` is assigned true, and variables `1` and 3 are assigned false. This is one possible assignment.

## License
This project is licensed under the [MIT License](LICENSE). This project uses Minisat2.2, whose license can be found [here](https://github.com/niklasso/minisat/blob/eb01ad68b75bb3b34ff8657c37ad6a31faae0fc3/LICENSE).

## Author
[Sukrut Rao](https://github.com/GoodDeeds/)

For any issues, queries, or suggestions, please [open an issue](https://github.com/GoodDeeds/MaxSAT-Solver/issues/new).

---

This project was created as a part of the course CS6403: Constraint Solving at IIT Hyderabad.