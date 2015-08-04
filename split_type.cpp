


#include <mpi.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include "common.hpp"

void vector_to_charlist(const std::vector<std::string>& vec, char* charlist, int maxsize, char end='|') {
  int total = 0;
  char* p = charlist;
  for (auto n : vec) {
    int sl = n.length()+1;
    
    // avoid going over
    if (total + sl >= maxsize) {
      *p = end;
      return;
    }

    memcpy(p, n.c_str(), sl);
    p += sl;
    total += sl;
  }
  *p = end;
}

std::vector<std::string> charlist_to_vector(char* charlist, int maxsize, char end='|') {
  std::vector<std::string> res;
  char buffer[1024];
  int pos = 0;
  int total = 0;
  char * p = charlist;
  while (*p != end && total < maxsize) {
    buffer[pos] = *p;
    if (*p == 0) {
      res.push_back(std::string(buffer));
      pos = 0;
    } else {
      pos++;
    }
    p++;
    total++;
  }
  return res;
}

int find_or_insert(std::vector<std::string>& vec, std::string n) {
  int i = 0;
  for ( ; i<vec.size(); i++) {
    if (vec[i] == n) {
      return i;
    }
  }
  vec.push_back(n);
  return i;
}

// split type support without MPI3 
// algorithm is from http://stackoverflow.com/questions/18130170/classify-node-processes-together-with-mpi-and-fortran
void mpi2_split_type(MPI_Comm* newcomm) {
  const int tag = 0;

  int rank;
  int size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  auto prev_rank = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
  auto next_rank = (rank < size-1) ? rank + 1 : MPI_PROC_NULL;

  char proc_name[1024];
  int proc_name_len;
  MPI_Get_processor_name(proc_name, &proc_name_len);

  char list[4096];
  MPI_CHECK( MPI_Recv(list, 4096, MPI_CHAR, prev_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE) );

  auto v = charlist_to_vector(list, 4096);

  // list not contains me
  auto color = find_or_insert(v, std::string(proc_name));
  auto key = 0;  // use rank if care about ordering

  vector_to_charlist(v, list, 4096); 

  MPI_CHECK( MPI_Send(list, 4096, MPI_CHAR, next_rank, tag, MPI_COMM_WORLD) );

  // do the split by type
  MPI_Comm_split(MPI_COMM_WORLD, color, key, newcomm);
}


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

MPI_Comm l2com;
mpi2_split_type(&l2com);

int shared_rank = -1;
int shared_size = -1;
MPI_Comm_rank(lcom, &shared_rank);  
MPI_Comm_size(lcom, &shared_size);  // expect np % cores-per-node

int shared_rank2 = -1;
int shared_size2 = -1;
MPI_Comm_rank(l2com, &shared_rank2);  
MPI_Comm_size(l2com, &shared_size2);  // expect np % cores-per-node
  
char proc_name[1024];
int proc_name_len;
MPI_Get_processor_name(proc_name, &proc_name_len);
    
// Print off a hello world message
    printf("processor %s, rank %d"
           " out of %d processors\n"
           "  MPI3 split type: %d of %d\n"
           "  MPI2 split type: %d of %d, node name %s\n",
           processor_name, world_rank, world_size, shared_rank, shared_size,
        shared_rank2, shared_size2, proc_name);



    // Finalize the MPI environment.
    MPI_Finalize();
}
