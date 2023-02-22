P1 = sssp
P2 = sssp_omp
P3 = sssp_mpi
#Choose either the C compiler or the C++ compiler from the following 2 lines
GCC = gcc
MPI = mpicc
CFLAGS = -g -Wall
OMPFLAG = -fopenmp
MPIFLAG = -lm

all: $(P1) $(P2) $(P3)

$(P1): $(P1).c
	$(GCC) $(CFLAGS) $(OMPFLAG) $(P1).c -o $(P1)

$(P2): $(P2).c
	$(GCC) $(CFLAGS) $(OMPFLAG) $(P2).c -o $(P2)

$(P3): $(P3).c
	$(MPI) $(CFLAGS) -o $(P3) $(P3).c $(MPIFLAG)

clean:
	rm -vf $(P1) $(P2) $(P3)
