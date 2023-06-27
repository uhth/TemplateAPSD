#include "../include/mpi_wrap.h" 

void MpiWrapper::init(int &argc, char** argv) {
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    //2 ecoltra columns are needed to store first-- and last++
    num_cols = (arrayWidth / num_procs) + 2;
	surplus_cols = arrayWidth % num_procs;
    //custom datatypes
    MPI_Type_vector(arrayHeight, 1, num_cols, MPI_UINT8_T, &procArrayColType);
	MPI_Type_commit(&procArrayColType);
	//giving 0 any extra columns
	if(my_id == 0) num_cols += surplus_cols;
    MPI_Type_vector(arrayHeight, 1, arrayWidth, MPI_UINT8_T, &masterArrayColType);
	MPI_Type_commit(&masterArrayColType);
}

void MpiWrapper::sendInitialArray(Array2D *masterArray2d, Array2D *procArray2d) {
	if(my_id != 0) return;
	procArray2d->clearAll();

	int cols_to_send = arrayWidth / num_procs;

	for(int row = 0; row < arrayHeight; ++row)
		for(int p_col = 1, m_col = 0; m_col < cols_to_send + surplus_cols; ++m_col, ++p_col)
			*procArray2d->at(row, p_col) = *masterArray2d->at(row, m_col);
	
	for( int proc = 1; proc < num_procs; ++proc) {
		int start_col = (cols_to_send * proc) + surplus_cols;
		for( int p_col = 0, m_col = start_col; p_col < cols_to_send; ++p_col, ++m_col)
			MPI_Send( masterArray2d->col(m_col), 1, masterArrayColType, proc, IM_TAG, MPI_COMM_WORLD );
	}
	
	syncToBarrier();
}

void MpiWrapper::recvInitialArray(Array2D *procArray2d) {
	procArray2d->clearAll();
    for(int p_col = 1; p_col < num_cols - 1; ++p_col) {
		MPI_Recv( procArray2d->col(p_col), 1, procArrayColType, 0, IM_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	syncToBarrier();
}

void MpiWrapper::ProcessAndSyncProcArray2d(Array2D *procArray2d, std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func) {
	procArray2d->clearAuxArray();
	
	MPI_Request lastCol, firstCol;
	Array2DRange centerColRange = {.startRow = 0, .endRow = arrayHeight, .startCol = 2, .endCol = num_cols - 2, .rowBoundary = arrayHeight, .colBoundary = num_cols};
	Array2DRange firstColRange = {.startRow = 0, .endRow = arrayHeight, .startCol = 1, .endCol = 2, .rowBoundary = arrayHeight, .colBoundary = num_cols};
	Array2DRange lastColRange = {.startRow = 0, .endRow = arrayHeight, .startCol = num_cols - 2, .endCol = num_cols - 1, .rowBoundary = arrayHeight, .colBoundary = num_cols};

	int previous_proc = (my_id - 1 >= 0) ? my_id - 1 : num_procs - 1;
	int next_proc = (my_id + 1) % num_procs;
	//receive MY first-- col from previous process
	MPI_Irecv(procArray2d->col(0), 1, procArrayColType, previous_proc, LC_TAG, MPI_COMM_WORLD, &firstCol);
	
	//receive MY last++ col from next process
	MPI_Irecv(procArray2d->col(num_cols - 1), 1, procArrayColType, next_proc, FC_TAG, MPI_COMM_WORLD, &lastCol);

	//send MY last col to next process
	MPI_Send(procArray2d->col(num_cols - 2), 1, procArrayColType, next_proc, LC_TAG, MPI_COMM_WORLD);
	
	//send MY first col to previous process
	MPI_Send(procArray2d->col(1), 1, procArrayColType, previous_proc, FC_TAG, MPI_COMM_WORLD);

	//computing center col
	procArray2d->callAlgFuncOnRangeUseAux(func, centerColRange);

	//computing first col
	MPI_Wait( &firstCol, MPI_STATUS_IGNORE );
	procArray2d->callAlgFuncOnRangeUseAux(func, firstColRange);

	//computing last col
	MPI_Wait( &lastCol, MPI_STATUS_IGNORE );
	procArray2d->callAlgFuncOnRangeUseAux(func, lastColRange);

	procArray2d->swap();
	
}

void MpiWrapper::sendProcArray(Array2D* procArray) {
	if(my_id == 0) return;
	
	for(int col = 1; col < num_cols - 1; ++col)
		MPI_Send(procArray->col(col), 1, procArrayColType, 0, PROC_COL_TAG, MPI_COMM_WORLD);

}

void MpiWrapper::recvProcArray(Array2D* masterArray2d) {
	if(num_procs < 2 || my_id != 0) return;

	int cols_to_recv_per_process = arrayWidth / num_procs;
	for(int proc = 1; proc < num_procs; ++proc) {
		int start_col = (cols_to_recv_per_process * proc) + surplus_cols;
		for(int col = start_col, p_col = 0; p_col < cols_to_recv_per_process; ++col, ++p_col)
			MPI_Recv(masterArray2d->col(col), 1, masterArrayColType, proc, PROC_COL_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

}

void MpiWrapper::putMastersContribute(Array2D* masterArray, Array2D* procArray) {
	for(int row = 0; row < arrayHeight; ++row)
		for(int p_col = 1, m_col = 0; p_col < num_cols - 1; ++p_col, ++m_col)
			*masterArray->at(row, m_col) = *procArray->at(row, p_col);
}

EVENT MpiWrapper::syncEvents(EVENT event) {
	EVENT snd_event = event;
	EVENT rcv_event = event;
	if(my_id == 0)
		MPI_Bcast(&snd_event, 1, MPI_UINT8_T, 0, MPI_COMM_WORLD);
	else
		MPI_Bcast(&rcv_event, 1, MPI_UINT8_T, 0, MPI_COMM_WORLD);
	return rcv_event;
}

void MpiWrapper::syncToBarrier() {
	MPI_Barrier(MPI_COMM_WORLD);
}