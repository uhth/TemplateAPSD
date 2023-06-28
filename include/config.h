#ifndef CONFIG_H
#define CONFIG_H


#define PATH_TO_FONT "./font/VictorMono-Regular.otf"

#define DIM 400  //size of the matrix

//mpi tags
#define FC_TAG 0 //first column tag ( send )
#define LC_TAG 1 //last column tag ( send )
#define IA_TAG 3 //initial Array tag
#define PROC_COL_TAG 5 //process sends col to master

//graphics stuff
#define SLEEP_TIME 0 //use it to slow down simulation
#define PADDING 0 //not really working
#define D_WIDTH 1280
#define D_HEIGHT 720

//GOL
#define GOL_DENSITY 30

//FOREST
#define P_ZAPP 1
#define P_TREE 40

#endif



