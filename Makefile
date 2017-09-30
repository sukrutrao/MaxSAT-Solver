CS15BTECH11036-maxsat:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core CS15BTECH11036-maxsat.cpp -lminisat -o CS15BTECH11036-maxsat
	
all:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core maxsat.cpp -lminisat
	
msu3:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core msu3.cpp -lminisat -g
	
maxsat2:
	g++ -O2 -I ../original_minisat/ -L ../original_minisat/core maxsat2.cpp -lminisat -g
    
clean:
	rm a.out
