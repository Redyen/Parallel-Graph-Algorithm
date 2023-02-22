#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#define MAX_LINE_LENGTH 1000
#define Inf 10000000
/* This program runs single source Dijkstra's algorithm. Given the distance
 matrix that defines a graph, we seek a minimum distance array between
 source vertex and all other vertices. */
int main(int argc, char **argv) {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];

    const char* graphs[8] = {"Graphs/graph_0.txt", "Graphs/graph_1.txt","Graphs/graph_2.txt", "Graphs/graph_3.txt",
                            "Graphs/graph_4.txt", "Graphs/graph_5.txt", "Graphs/graph_6.txt", "Graphs/graph_7.txt"};

    FILE    *timer;
    FILE    *result;


    //fclose(fopen("Times.txt", "w"));

    timer = fopen("Times.txt", "a");
    result = fopen("Results.txt", "a");

    fprintf(timer, "OpenMP implementation:\n");
    fprintf(result, "OpenMP Result:\n");

    for(int g = 0; g < 8; g++){

        textfile = fopen(graphs[g], "r");
        if(textfile == NULL)
            return 1;

        char * s = fgets(line, MAX_LINE_LENGTH, textfile);

        char * sVec = strtok(s, " ");

        char * sTemp = strtok(NULL, " ");

        char * sEdge = strtok(sTemp, "\n");

        int Vector = atoi(sVec);
        int Edges =  atoi(sEdge);

       // printf("Number of Vectors: %d\nNumber of Edges: %d\n", Vectors, Edges);

        int Adj[Vector][Vector];

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


        //DJIK==================================================================================


        //Private:
        int tFirst; //first vertex for its thread
        int tID;    //ID of each thread
        int tLast;  //Last vertex for its thread
        int tDistance; //min distance per thread
        int tMin; //min distance for a vertex in its thread
        int tStep; //min distance vertex per thread

        //Shared:
        int visited[Vector]; //list of all visisted vertexes
        int mDist;
        int dist[Vector]; //shortest distance array
        int mVert;
        int nThreads; //num threads

        //assigning the adj matrix and the initially visisted vertexes from source vertex

        for (int i = 0; i < Vector; i++) {
            visited[i] = 0;
            dist[i] = Adj[0][i];
        }
        visited[0] = 1;
        clock_t t;
        t = clock();
        omp_set_num_threads(4);
        //begin parallel
        # pragma omp parallel private ( tFirst, tID, tLast, tDistance, tMin, tStep ) shared ( visited, mDist, dist, mVert, nThreads, Adj )
        {
            tID = omp_get_thread_num();
            nThreads = omp_get_num_threads();
            tFirst = (tID * Vector ) / nThreads;
            tLast = ((tID + 1) * Vector) / nThreads - 1;

            for (tStep = 1; tStep < Vector; tStep++) {
                //per thread finds its closest unvisited vertex
                # pragma omp single
                {
                    mDist = Inf;
                    mVert = -1;
                }
                int k;
                tDistance = Inf;
                tMin = -1;

                for (k = tFirst; k <= tLast; k++) {
                    if (!visited[k] && dist[k] < tDistance) {
                        tDistance = dist[k];
                        tMin = k;
                    }
                }
                # pragma omp critical
                {
                    if (tDistance < mDist) {
                        mDist = tDistance;
                        mVert = tMin;
                    }
                }
                //ensuring that mvert and mDist dont overlap or change values
                # pragma omp barrier
                # pragma omp single
                {
                    if (mVert != - 1){
                        visited[mVert] = 1;
                    }
                }
                if ( mVert != -1 ){
                    for (int j = tFirst; j <= tLast; j++) {
                        if (!visited[j] && Adj[mVert][j] < Inf && dist[mVert] + Adj[mVert][j] < dist[j]) {
                            dist[j] = dist[mVert] + Adj[mVert][j];
                        }
                    }
                }
                #pragma omp barrier
            }
        }
        t = clock()-t;

        //PRINTING ==================================================================================

        for (int i = 0; i < Vector; i++) {
            if(g==1){
                fprintf(result, "%d ", dist[i]);
            }
        }

        //t = clock()-t;

        double time_taken = ((double)t)/CLOCKS_PER_SEC;

        fprintf(timer, "Vertices : %d | Time Taken: %f s\n", Vector, time_taken);


    }
    printf("Time recorded and checks are ready within the textfiles\n");
    fprintf(result, "\n");
    fclose(result);
    fclose(timer);
    return 0;

}
