all:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core solver.cpp -lminisat
    
clean:
	rm a.out
