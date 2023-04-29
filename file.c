#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "math.h"
#include "time.h"
int main( int argc, char *argv[])
{
    int sum=0, n=100000000, myid, numprocs, i, num_inside=0;
    double PI25DT = 3.141592653589793238462643;
    double mypi, pi, h, x , y, z, d;

    double startwtime = 0.0, endwtime;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);

    fprintf(stderr,"Process %d on %s\n",
            myid, processor_name);

    if(myid==0)
        startwtime = MPI_Wtime();
    printf("numprocs=%d",numprocs);


    int seeds[numprocs];
    if (myid == 0) {
        srand(time(NULL)); // seed the random number generator only on Rank 0
        for (int i = 0; i < numprocs; i++) {
            seeds[i] = rand(); // generate a random seed for each process
        }
    }
    
    MPI_Bcast(seeds, numprocs, MPI_INT, 0, MPI_COMM_WORLD); // broadcast the array of random seeds to all processes
    
    srand(seeds[myid]); // seed the random number generator on each process using the received random seed

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for(int j=myid;j<n;j+=numprocs){
        x = (double)(myid)/numprocs+((double)rand()/RAND_MAX)/numprocs;
        y = (double)rand()/RAND_MAX;
        z = (double)rand()/RAND_MAX;
        d = x*x +y*y + z*z;
//	printf("im process %d,x:%lf,y:%lf,z:%lf, d is %lf\n ",myid,x,y,z,d);
        if (d<1)
            num_inside+=1;

    }
    MPI_Reduce(&num_inside, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    pi =(double)(6*sum)/n;
    if (myid == 0)
    {
        printf("pi is approximately %.16f, Error is %.16f\n",
               pi, fabs(pi - PI25DT));
        endwtime = MPI_Wtime();
        printf("wall clock time = %f\n",
               endwtime-startwtime);
    }
    
    MPI_Finalize();
    return 0;
}

