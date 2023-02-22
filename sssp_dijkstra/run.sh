echo =======================================
echo Times and Validation checks are 
echo inside respective textfiles
echo =======================================
echo ---------Make----------
make
echo
echo ---------Serial has started----------
echo
./sssp
echo
echo ---------OpenMP has started----------
echo
./sssp_omp
echo
echo ---------MPI has started----------
echo 
mpirun -np 2 ./sssp_mpi
echo
echo ---------Finished----------