#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"
#define MAX_LINE_LENGTH 1000
#define Inf 10000000

int main(int argc, char *argv[]) {

    //READING FROM FILE =============================================================

    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];

    textfile = fopen("Graphs/graph_1.txt", "r");
    if(textfile == NULL)
        return 1;

    char * s = fgets(line, MAX_LINE_LENGTH, textfile);

    char * sVec = strtok(s, " ");

    char * sTemp = strtok(NULL, " ");

    char * sEdge = strtok(sTemp, "\n");

    int Vector = atoi(sVec);
    int Edges =  atoi(sEdge);


    //==============================================================================



    int num_procs, myrank, nlocal;
    int Adj[Vector][Vector]; //Adj Matrix
    int dist[Vector]; //final distance vector

    MPI_Init(&argc, &argv); // initialise
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    nlocal = Vector/num_procs; // vertexes per processor

    int * pWeight = (int *)malloc(nlocal*Vector*sizeof(int));//weight per process
    int * pDist = (int *)malloc(nlocal*sizeof(int)); //distance per process
    int localBuffer[Vector*Vector]; //local weight to send out

    if (myrank == 0) {
        for(int i = 0; i < Vector; i++){
            for(int j = 0; j < Vector; j++){
                if(i == j){
                    Adj[i][j] = 0;
                }
                else{
                    Adj[i][j] = Inf;
                }
            }
        }
        char * vert1;
        char * vert2;
        char * temp2;
        char * weight;
        int v1;
        int v2;
        int w;


        for(int i = 0; i < Edges; i++){
            fgets(line, MAX_LINE_LENGTH, textfile);

            vert1 = strtok(line, " ");
            v1 = atoi(vert1);

            vert2 = strtok(NULL, " ");
            v2 = atoi(vert2);

            temp2 = strtok(NULL, " ");

            weight = strtok(temp2, "\n");
            w = atoi(weight);

            Adj[v1][v2] = w;
            Adj[v2][v1] = w;
        }

        fclose(textfile);

        for(int i=0; i<num_procs;i++) {
            for(int j=0; j<Vector;j++) {
                for(int k=0; k<nlocal;k++) {
                    localBuffer[i*Vector*nlocal+j*nlocal+k]=Adj[j][i*nlocal+k];
                }
            }
        }
    }
    //send the buffers to each of their processors
    double t1, t2;
    t1 = MPI_Wtime();

    MPI_Scatter(localBuffer, nlocal*Vector, MPI_INT, pWeight, nlocal*Vector, MPI_INT, 0,
    MPI_COMM_WORLD);

    //DIJKSTRAS ===================================================================================================

    int *visited; //vertexes visited by the initial vertex(0)
    int fVertex; // first local vertex
    int lVertex; //last local vertex
    int u, udist;
    int lminpair[2]; //storing the local minumum distance
    int gminpair[2]; //storing the global min distance

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


    nlocal = Vector / num_procs;
    fVertex = myrank*nlocal; //ensuring the first vertex is unique per process
    lVertex = fVertex + nlocal - 1; //same as above

    //first vertex's visited
    for (int i = 0; i<nlocal; i++) {
        pDist[i] = pWeight[0*nlocal + i];
    }
    //update the visited vertexes
    visited = (int *)malloc(nlocal*sizeof(int));
    for (int i = 0; i<nlocal; i++) {
        visited[i] = 1;
    }
    if (0 >= fVertex && 0 <= lVertex) {
        visited[0 - fVertex] = 0;
    }


    for (int i = 1; i<Vector; i++) {
        //find shortest distance
        lminpair[0] = Inf;
        lminpair[1] = -1;
        for (int j = 0; j<nlocal; j++) {
            if (visited[j] && pDist[j] < lminpair[0]) {
                //update minumums when found
                lminpair[0] = pDist[j];
                lminpair[1] = fVertex + j;
            }
        }
        //track the local min compared to global min
        MPI_Allreduce(lminpair, gminpair, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
        udist = gminpair[0];
        u = gminpair[1];

        if (u == lminpair[1]) {
            visited[u - fVertex] = 0;
        }
        for (int j = 0; j<nlocal; j++) {
            if (visited[j] && ((udist + pWeight[u*nlocal + j]) < pDist[j])) {
                pDist[j] = udist + pWeight[u*nlocal + j];
            }
        }
    }
    free(visited);
    //gather the respective local distances
    MPI_Gather(pDist, nlocal, MPI_INT, dist, nlocal, MPI_INT, 0, MPI_COMM_WORLD);


    //PRINTING=============================================================================================================

    if (myrank == 0) {
        t2 = MPI_Wtime();
        printf( "Elapsed time is %f\n", t2 - t1 );
    }
    free(pWeight);
    free(pDist);
    MPI_Finalize();
    return 0;
}
