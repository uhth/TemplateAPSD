#ifndef MPI_WRAP_H
#define MPI_WRAP_H

#include <mpi.h>
#include "uint8_array2d.h"
#include "config.h"
#include "common.h"

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
        void sendProcArray(Array2D*);
        void recvProcArray(Array2D*);
        void syncToBarrier();
        void putMastersContribute(Array2D*, Array2D*);
        EVENT syncEvents(EVENT=EVENT::NONE);
        void ProcessAndSyncProcArray2d(Array2D*, std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)>);
        inline void exit() {MPI_Finalize();}
};

#endif