#ifndef CONFIG_H
#define CONFIG_H


#define PATH_TO_FONT "./font/VictorMono-Regular.otf"

#define DIM 200  //size of the matrix
#define COUNTER 10000 //cycles

//cells type
#define CTYPE unsigned int    //you can easly change types using these two macros, just make sure
#define MPITYPE MPI_UNSIGNED  //to pick the right corresponding MPI_Type ( e.g. int -> MPI_INT )

//mpi stuff
#define MASTER 0

#define FC_TAG 0 //first column tag ( send )
#define LC_TAG 1 //last column tag ( send )
#define PRINT_TAG 2
#define IM_TAG 3
#define PROC_COL_TAG 5 //process sends col to master

//graphics stuff
#define HAS_GRAPHICS true //toggles graphic output  <----- VERY IMPORTANT ( should be false if you're measuring performances )
#define SLEEP_TIME 0 //50 is the intended speed. Although lowering this value speeds everything up ( only works with graphics on )
#define BLOCK_SIZE 2
#define PADDING 0 //default 0
//define D_WIDTH ( DIM * BLOCK_SIZE )
//define D_HEIGHT ( DIM * BLOCK_SIZE )
#define D_WIDTH 1280
#define D_HEIGHT 720

#define SHOW_FPS true

//GOL 1
#define GOL_DENSITY 26

//FOREST 2
#define P_ZAPP 1
#define P_TREE 50

#endif



