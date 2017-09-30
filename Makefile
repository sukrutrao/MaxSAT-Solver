all:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core maxsat.cpp -lminisat -o without_loop.out
    
clean:
	rm a.out
