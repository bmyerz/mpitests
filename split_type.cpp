


#include <mpi.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);


MPI_Comm lcom;
MPI_Comm_split_type(MPI_COMM_WORLD, 
MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &lcom );

int shared_rank = -1;
int shared_size = -1;
MPI_Comm_rank(lcom, &shared_rank);
MPI_Comm_size(lcom, &shared_size);
    
// Print off a hello world message
    printf("Hello world from processor %s, rank %d"
           " out of %d processors\n and %d of %d\n",
           processor_name, world_rank, world_size, shared_rank, shared_size);



    // Finalize the MPI environment.
    MPI_Finalize();
}
