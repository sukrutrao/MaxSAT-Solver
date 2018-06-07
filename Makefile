CC = g++
INCLUDE_DIRS = minisat
LIB_DIRS = minisat/core
LFLAGS = -lminisat
EXEC = solver

all:
	@echo "Building minisat..."
	@LIB=minisat MROOT=$(CURDIR)/minisat make libr -C $(LIB_DIRS)
	@echo "Done."
	@echo "Building MaxSAT Solver..."
	@$(CC) -I $(INCLUDE_DIRS) -L $(LIB_DIRS) solver.cpp $(LFLAGS) -o $(EXEC)
	@echo "Done."

clean:
	@echo "Cleaning up..."
	@LIB=minisat MROOT=$(CURDIR)/minisat make clean -C $(LIB_DIRS)
	@rm $(EXEC)
	@echo "Done."
