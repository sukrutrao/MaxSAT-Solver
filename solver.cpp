/*
 * Program to implement a MaxSAT solver using Minisat 2.2
 * This solver uses Sequential Encoding and performs a linear search from
 * unsatisfied to satisfied
 *
 * Sukrut Rao
 * CS15BTECH11036
 *
 */

#include "core/Solver.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace Minisat;

/*
 * return code for checking the result of the current iteration
 */
enum RetVal {
  satisfied,   // the formula has been satisfied
  unsatisfied, // the formula has been unsatisfied
};

/*
 * class to implement the MaxSAT solver
 */
class MaxSATSolver {
private:
  Solver solver;      // the Minisat solver object to solve a SAT formula
  int variable_count; // number of variables in the input
  int clause_count;   // number of clauses in the input
  vec<Var> variables; // Minisat vector to store the variables
  vec<Var> relaxation_variables; // Minisat vector to store the relaxation
                                 // variables for each clause
  vec<Var> assumption_variables; // Minisat vector to store the assumption
                                 // variables for each relaxation
  vec<Lit> assumptions; // Minisat vector to store the assumption literals
  vec<vec<Var>> encoded_variables; // Vec of Vecs to store the encoding
                                   // variables for sequential encoding
  RetVal solve_with_relaxation(
      int); // to solve the formula by relaxing a fixed number of clauses
  void add_encoded_clauses(
      int); // to add encoded clauses for relaxation to the formula
  void display_result(int); // to display the result
public:
  MaxSATSolver() {}  // constructor
  void initialize(); // to accept input and initialize
  void solve();      // to solve and show the result
};

/*
 * function to accept the input and initialize the solver
 */
void MaxSATSolver::initialize() {
  char c;        // store first character
  std::string s; // dummy string

  while (true) {
    std::cin >> c;
    if (c == 'c') // if comment
    {
      std::getline(std::cin, s); // ignore
    } else                       // else, if would be a p
    {
      std::cin >> s; // this would be cnf
      break;
    }
  }
  std::cin >> variable_count;       // accept the number of variables
  std::cin >> clause_count;         // accept the number of clauses
  variables.growTo(variable_count); // set the size of the variables vector
  // the number of relaxation variables and assumption variables are up to the
  // number of clauses
  relaxation_variables.growTo(clause_count);
  assumption_variables.growTo(clause_count);

  for (int i = 0; i < variable_count; i++) {
    variables[i] = solver.newVar(); // add each variable to the solver
  }
  for (int i = 0; i < clause_count; i++) {
    relaxation_variables[i] =
        solver.newVar(); // add each relaxation variable to the solver
  }

  // the first dimension is n-1 for sequential encoding
  encoded_variables.growTo(clause_count - 1);

  for (int i = 0; i < clause_count - 1; i++) {
    // the second dimension is k, which is at most n
    encoded_variables[i].growTo(clause_count);
    for (int j = 0; j < clause_count; j++) {
      // add the encoded variable to the solver
      encoded_variables[i][j] = solver.newVar();
    }
  }

  int literal; // to accept the DIMACS form input
  for (int i = 0; i < clause_count; i++) {
    vec<Lit> this_clause; // to store the current clause in the literal form
    while (true)          // while the ith clause gets more literals
    {
      std::cin >> literal;
      if (literal > 0) // if the variable has positive polarity
      {
        // push with positive polarity
        this_clause.push(mkLit(variables[literal - 1], false));
      } else if (literal < 0) // if the variable has negative polarity
      {
        // push with negative polarity
        this_clause.push(~mkLit(variables[-literal - 1], false));
      } else // end of the current clause
      {
        // add the relaxation variable for this clause
        this_clause.push(mkLit(relaxation_variables[i], false));
        solver.addClause(this_clause); // add the clause to the solver
        break;                         // read 0, so move to next clause
      }
    }
  }
}

/*
 * function to solve the formula and display the result
 */
void MaxSATSolver::solve() {
  /*
   * starting from the complete formula, iteratively relax more and more clauses
   */
  for (int i = clause_count; i > 0; i--) {
    // if a satisfying assignment is found at this stage
    if (solve_with_relaxation(clause_count - i) == satisfied) {
      // display the result and exit
      display_result(i);
      break;
    }
  }
}

/*
 * function to solve after relaxing a set number of clauses and return whether
 * a satisfying condition was found
 * Arguments : relaxed_clauses_count - the maximum number of clauses to relax
 * Return value : a member of the RetVal enum, where the value is satisfied if a
 *                satisfying condition was found, and unsatisfied otherwise
 */
RetVal MaxSATSolver::solve_with_relaxation(int relaxed_clauses_count) {
  bool result; // to store the result for this iteration
  assumptions.clear();
  // add the encoding that imposes the relaxation constraint to the solver
  add_encoded_clauses(relaxed_clauses_count);
  /*
   * set all previous assumptions to false to nullify all past relaxation
   * constraints
   */
  for (int i = 0; i < relaxed_clauses_count; i++) {
    assumptions.push(~mkLit(assumption_variables[i], false));
  }
  /*
   * set the current assumption for relaxing up to relaxed_clauses_count
   * to true
   */
  assumptions.push(mkLit(assumption_variables[relaxed_clauses_count], false));
  // solve under these assumptions with the Minisat solver
  result = solver.solve(assumptions);
  // return the result
  return result ? satisfied : unsatisfied;
}

/*
 * function to add the encoded clauses
 * Arguments : relaxed_clauses_count - the maximum number of clauses to relax
 */
void MaxSATSolver::add_encoded_clauses(int relaxed_clauses_count) {
  // create a new assumption variable for this iteration
  assumption_variables[relaxed_clauses_count] = solver.newVar();
  // get the equivalent literal with positive polarity for use, with negation
  // when required
  Lit assumption_literal =
      mkLit(assumption_variables[relaxed_clauses_count], false);
  Lit p, q; // placeholder literals

  // for the base case when no clauses are relaxed
  if (relaxed_clauses_count == 0) {
    for (int i = 0; i < clause_count; i++) {
      // get the literal for each relaxation variable
      p = mkLit(relaxation_variables[i], false);
      /*
       * enforce that under this assumption, each relaxation
       * variable is false
       */
      solver.addClause(~p, ~assumption_literal);
    }
    return;
  }

  /*
   * the following creates the sequential encoding
   * let the relaxation variables be y1,y2,...,yn
   * let the encoding variables be a1_1,a1_2,...,an-1_k
   * all clauses are imposed under the given assumption
   */
  for (int i = 0; i < clause_count - 2; i++) {
    for (int j = 0; j < relaxed_clauses_count; j++) {
      p = mkLit(encoded_variables[i][j], false);
      q = mkLit(encoded_variables[i + 1][j], false);
      /*
       * impose the clause that for i in range 1 to n-2 and for j in range 1 to
       * k ai_j -> ai+1_j
       */
      solver.addClause(~p, q, ~assumption_literal);
      if (j < relaxed_clauses_count - 1) {
        vec<Lit> clause;
        clause.push(~mkLit(encoded_variables[i][j], false));
        clause.push(~mkLit(relaxation_variables[i + 1], false));
        clause.push(mkLit(encoded_variables[i + 1][j + 1], false));
        clause.push(~assumption_literal);
        /*
         * impose the clause that for i in range 1 to n-2 and j in range 1 to
         * k-1 (ai_j and yj+1) -> ai+1_j+1
         */
        solver.addClause(clause);
      }
    }
  }
  for (int i = 0; i < clause_count - 1; i++) {
    p = mkLit(encoded_variables[i][relaxed_clauses_count - 1], false);
    q = mkLit(relaxation_variables[i + 1], false);
    /*
     * impose the clause that for i in range 1 to n-1
     * ai_k -> ~yi+1
     */
    solver.addClause(~p, ~q, ~assumption_literal);
    p = mkLit(relaxation_variables[i], false);
    q = mkLit(encoded_variables[i][0], false);
    /*
     * impose the clause that for i in range 1 to n-1
     * yi -> ai_1
     */
    solver.addClause(~p, q, ~assumption_literal);
  }

  /*
   * the following clauses are added to make the encoding tight
   * impose the clause that for j in range 1 to k and i<j
   * ~ai_j
   */
  for (int j = 0; j < relaxed_clauses_count; j++) {
    for (int i = 0; i < j; i++) {
      p = mkLit(encoded_variables[i][j], false);
      solver.addClause(~p, ~assumption_literal);
    }
  }
}

/*
 * function to display the result of the MaxSAT solver
 * Arguments : number_of_clauses_satisfied - the maximum number of clauses that
 * could be satisfied
 */
void MaxSATSolver::display_result(int number_of_clauses_satisfied) {
  std::cout << number_of_clauses_satisfied << std::endl;
  // display the value of each variable
  for (int i = 0; i < variable_count; i++) {
    std::cout << (i == 0 ? "" : " ");
    std::cout << (solver.model[i] == l_True ? "" : "-");
    std::cout << i + 1;
  }
  std::cout << " 0";
}

/*
 * the main() function
 */
int main() {
  MaxSATSolver solver; // create the solver
  solver.initialize(); // initialize it
  solver.solve();      // solve the formula
  return 0;
}
