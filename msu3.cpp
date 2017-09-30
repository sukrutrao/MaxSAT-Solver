#include "core/Solver.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

using namespace Minisat;

enum RetVal
{
    satisfied, // the formula has been satisfied
    unsatisfied, // the formula has been unsatisfied
};

class MaxSATSolver
{
    public:
        Solver solver;
        int variable_count;
        int clause_count;
        vec<Var> variables;
        vec<Var> relaxation_variables;
        vec<Var> assumption_variables;
        vec< vec<Var> > encoded_variables;
        MaxSATSolver() {}
        void initialize();
        void solve();
        RetVal solve_with_relaxation(int);
        void add_encoded_clauses(int);
        void display_result(int);
};

void MaxSATSolver::initialize()
{
    char c; // store first character
    std::string s; // dummy string

    while(true)
    {
        std::cin >> c;
        if(c=='c') // if comment
        {
            std::getline(std::cin,s); // ignore
        }
        else // else, if would be a p
        {
            std::cin >> s; // this would be cnf
            break;
        }
    }
    std::cin >> variable_count;
    std::cin >> clause_count;
    variables.growTo(variable_count);
    relaxation_variables.growTo(clause_count);
    assumption_variables.growTo(clause_count);
    
    for(int i = 0; i < variable_count; i++)
    {
        variables[i] = solver.newVar();
    }
    for(int i = 0; i < clause_count; i++)
    {
    	relaxation_variables[i] = solver.newVar();
    }
    for(int i = 0; i < clause_count; i++)
    {
    	assumption_variables[i] = solver.newVar();
    } 
    
    encoded_variables.growTo(clause_count-1);
    
    for(int i = 0; i < clause_count-1; i++)
	{
		encoded_variables[i].growTo(clause_count);
		for(int j = 0; j < clause_count; j++)
		{
			encoded_variables[i][j] = solver.newVar();
		} 
	}
    
    int literal;
    for(int i = 0; i < clause_count; i++)
    {
        vec<Lit> this_clause;
        while(true) // while the ith clause gets more literals
        {
            std::cin >> literal;     
            if(literal > 0) // if the variable has positive polarity
            {
                this_clause.push(mkLit(variables[literal-1],false));
            }
            else if(literal < 0) // if the variable has negative polarity
            {
                this_clause.push(~mkLit(variables[-literal-1],false));
            }
            else
            {
                this_clause.push(mkLit(relaxation_variables[i],false));
                solver.addClause(this_clause);
                break; // read 0, so move to next clause
            }    
        }       
    }
}

void MaxSATSolver::solve()
{
	for(int i = clause_count; i > 0; i--)
	{
		if(solve_with_relaxation(clause_count-i) == satisfied)
		{
			display_result(i);
			break;
		}
	}
}

RetVal MaxSATSolver::solve_with_relaxation(int relaxed_clauses_count)
{
	bool result;
	vec<Lit> assumptions;
	assumptions.clear();
	add_encoded_clauses(relaxed_clauses_count);
	for(int i = 0; i < relaxed_clauses_count; i++)
	{
		assumptions.push(~mkLit(assumption_variables[i],false));
	}
	assumptions.push(mkLit(assumption_variables[relaxed_clauses_count],false));
	result = solver.solve(assumptions);
	return result ? satisfied : unsatisfied;
}

void MaxSATSolver::add_encoded_clauses(int relaxed_clauses_count)
{
//	assumption_variables[relaxed_clauses_count] = solver.newVar();
	Lit assumption_literal = mkLit(assumption_variables[relaxed_clauses_count],false);
	if(relaxed_clauses_count == 0)
	{
		
		for(int i = 0; i < clause_count; i++)
		{
			Lit p = mkLit(relaxation_variables[i],false);
			solver.addClause(~p,~assumption_literal);
		}
		return;
	}
	
/*	for(int i = 0; i < clause_count-1; i++)
	{
		encoded_variables[i][relaxed_clauses_count] = solver.newVar();
	} 	*/
	
	for(int i = 0; i < clause_count-2; i++)
	{
		for(int j = 0; j < relaxed_clauses_count; j++)
		{
			Lit p = mkLit(encoded_variables[i][j],false);
			Lit q = mkLit(encoded_variables[i+1][j],false);
			solver.addClause(~p,q,~assumption_literal);
			if(j < relaxed_clauses_count-1)
			{
				vec<Lit> clause;
				clause.push(~mkLit(encoded_variables[i][j],false));
				clause.push(~mkLit(relaxation_variables[i+1],false));
				clause.push(mkLit(encoded_variables[i+1][j+1],false));
				clause.push(~assumption_literal);
				solver.addClause(clause);
			}
		}
	}
	for(int i = 0; i < clause_count-1; i++)
	{
		Lit p = mkLit(encoded_variables[i][relaxed_clauses_count-1],false);
		Lit q = mkLit(relaxation_variables[i+1],false);
		solver.addClause(~p,~q,~assumption_literal);
		p = mkLit(relaxation_variables[i],false);
		q = mkLit(encoded_variables[i][0],false);
		solver.addClause(~p,q,~assumption_literal);
	}
	for(int j = 0; j < relaxed_clauses_count; j++)
	{
		for(int i = 0; i < j; i++)
		{
			Lit p = mkLit(encoded_variables[i][j],false);
			solver.addClause(~p,~assumption_literal);
		}
	} 
	
	for(int i = 0; i < clause_count-1; i++)
	{
		for(int j = relaxed_clauses_count; j < clause_count; j++)
		{
			Lit p = mkLit(encoded_variables[i][j],false);
			solver.addClause(p,~assumption_literal);
		}
	} 
	
    for(int i = relaxed_clauses_count+1; i < clause_count; i++)
	{
		Lit p = mkLit(assumption_variables[i],false);
		solver.addClause(p,~assumption_literal);
	}  
}

void MaxSATSolver::display_result(int number_of_clauses_satisfied)
{
	std::cout << number_of_clauses_satisfied << std::endl;
	for(int i = 0; i < variable_count; i++)
	{
		std::cout << (i == 0 ? "" : " ");
		std::cout << (solver.model[i] == l_True ? "" : "-");
		std::cout << i+1;
	}
	std::cout << " 0";
}

int main()
{
    MaxSATSolver solver;
    solver.initialize();
    solver.solve();
    return 0;
}
