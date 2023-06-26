#ifndef MPI_WRAP_H
#define MPI_WRAP_H

#include <mpi.h>
#include "uint8_array2d.h"
#include "config.h"

class MpiWrapper {
    public:
        int num_procs;
        int my_id;
        int num_cols;
        int surplus_cols;
        int arrayWidth;
        int arrayHeight;
        MPI_Datatype masterArrayColType;
        MPI_Datatype procArrayColType;
        MpiWrapper(int arrayWidthIn, int arrayHeightIn) : arrayWidth(arrayWidthIn), arrayHeight(arrayHeightIn) {};
        void init(int&, char**);
        void sendInitialArray(Array2D*,Array2D*);
        void recvInitialArray(Array2D*);
        void syncArray2d();
        inline void exit() {MPI_Finalize();};
};

#endif