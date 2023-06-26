#include "../include/mpi_wrap.h" 

void MpiWrapper::init(int &argc, char** argv) {
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    //2 ecoltra columns are needed to store first-- and last++
    num_cols = (arrayWidth / num_procs) + 2;
	surplus_cols = arrayWidth % num_procs;
	if(my_id == 0) num_cols += surplus_cols;
    //custom datatypes
    MPI_Type_vector(arrayHeight, 1, arrayWidth, MPI_INT8_T, &masterArrayColType);
	MPI_Type_commit(&masterArrayColType);
    MPI_Type_vector(arrayHeight, 1, num_cols, MPI_INT8_T, &procArrayColType);
	MPI_Type_commit(&procArrayColType);
}

void MpiWrapper::sendInitialArray(Array2D *masterArray2d, Array2D *procArray2d) {
    	for(int row = 0; row < arrayHeight; ++row)
		    for(int col1 = 1, col = 0; col < num_cols - 2; ++col, ++col1)
		        *procArray2d->at(row, col1) = *masterArray2d->at(row, col);

		for( int proc = 1; proc < num_procs; ++proc)
		    for( int col = 0, c1 = num_cols - 2; col < arrayWidth / num_procs; ++col, ++c1)
		        MPI_Send( masterArray2d->col( c1 ), 1, masterArrayColType, proc, IM_TAG, MPI_COMM_WORLD );
}

void MpiWrapper::recvInitialArray(Array2D *procArray2d) {
    for( int c2 = 1; c2 < num_cols - 1; c2++ )
		MPI_Recv( procArray2d->col( c2 ), 1, procArrayColType, MASTER, IM_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}