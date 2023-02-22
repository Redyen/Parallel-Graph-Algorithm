#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LENGTH 1000
#define Inf 10000000

int main()
{
    const char* graphs[8] = {"Graphs/graph_0.txt", "Graphs/graph_1.txt","Graphs/graph_2.txt", "Graphs/graph_3.txt",
                            "Graphs/graph_4.txt", "Graphs/graph_5.txt", "Graphs/graph_6.txt", "Graphs/graph_7.txt"};

    FILE    *textfile;
    FILE    *timer;
    FILE    *result;

    char    line[MAX_LINE_LENGTH];

    fclose(fopen("Times.txt", "w"));
    fclose(fopen("Results.txt", "w"));

    timer = fopen("Times.txt", "a");
    result = fopen("Results.txt", "a");

    fprintf(timer, "Serial implementation:\n");
    fprintf(result, "Serial Result:\n");


    for(int g = 0; g < 8; g++){

         clock_t t;
        t = clock();

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


        //ADJACENT ARRAY IS NOW POPULATED===============================================

        int dist[Vector]; // keep track of the distances between vectors
        int visited[Vector]; //1 is visited 0 is unvisited

        int c; //counter
        int u; //this is the next vertex explored
        int min;

        //assign the distance from the source to each vertex
        for (int i = 0; i < Vector; i++) {
            dist[i] = Adj[0][i];
            visited[i] = 0;
        }

        visited[0] = 1;
        c = 1;

        while (c < Vector) { //while the number of visited vertexs is less than total vertex's
            min = Inf; //init the min
            for (int i = 0; i < Vector; i++) { //finding closest vertex
                //printf("%d\n", min);
                if (dist[i] < min && !visited[i]) {
                    min = dist[i];
                    u = i;
                }
            }
            //printf("%d\n", min);
            visited[u] = 1;
            c++;

            for (int i = 0; i < Vector; i++) {
                if (visited[i] == 0 && min + Adj[u][i] < dist[i]) {
                        dist[i] = min + Adj[u][i];

                }
            }
        }

        //PRINTING ==================================================================================

        for (int i = 0; i < Vector; i++) {
            if(g==1){
                fprintf(result, "%d ", dist[i]);
            }

        }

        t = clock()-t;

        double time_taken = ((double)t)/CLOCKS_PER_SEC;

        fprintf(timer, "Vertices : %d | Time Taken: %f s\n", Vector, time_taken);


        }
    printf("Time recorded and checks are ready within the textfiles\n");
    fprintf(result, "\n");
    fclose(result);
    fclose(timer);


    return 0;
}















