
#include <mpi.h>
#include <thread>
#include <iostream>
#include <sys/stat.h>
#include <stdexcept>
#include <atomic>
#include "../include/ca_graphics.h"
#include "../include/cellular_automata.h"

using namespace std;

atomic<bool> stop( false );
void *eventH( ALLEGRO_THREAD*, void* );
void compute( CMatrix<CTYPE>*, CMatrix<CTYPE>* );
void computePrinting( CMatrix<CTYPE>* );
void draw( int, CTYPE*, int, int, int );

//globals
bool quit = false;
bool switchAutoma = false;
bool restart = false;
int numProcs, //number of processes
myId, //current process id
counter = COUNTER, //main loop counter
numCols, //number of columns foreach process
master_plus = 0;
long startt, endt;
MPI_Datatype column;
MPI_Datatype bigColumn;
CMatrix<CTYPE>* initialBoard;

#if HAS_GRAPHICS
CAGraphics<CTYPE> graphics;
ALLEGRO_DISPLAY *display; 
ALLEGRO_TRANSFORM resizeTransform;
ALLEGRO_TRANSFORM identityTransform;

struct DisplayAndMatrix {
	ALLEGRO_DISPLAY *display;
	CMatrix<CTYPE>* board1;
	CMatrix<CTYPE>* board2;

};
#endif

void initBoardsALL( int, CMatrix<CTYPE>&, CMatrix<CTYPE>& );

int main( int argc, char *argv[] ) {
		
	//mpi stuff 
	//MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size( MPI_COMM_WORLD, &numProcs );
	MPI_Comm_rank( MPI_COMM_WORLD, &myId );


	//START TIME
	if( myId == MASTER ) startt = MPI_Wtime();
			
	numCols = ( DIM / numProcs ) + 2; //2 more columns to store first-- and last++

	master_plus += DIM % numProcs;
	
	//needed to cover each column
	if( myId == MASTER ) numCols += master_plus;
	
	//custom data-type to send columns
	MPI_Type_vector( DIM, 1, numCols, MPITYPE, &column );
	MPI_Type_commit( &column );
	
	
	//linear matrix to use dynamic allocation d
	CMatrix<CTYPE> board1( DIM, numCols );
	CMatrix<CTYPE> board2( DIM, numCols );

	//allegro stuff
	#if HAS_GRAPHICS
	ALLEGRO_FONT *ttf_font;
	ALLEGRO_THREAD *t1;
	if( !al_init() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon() || !al_install_keyboard() ) return -1;
	//Check if font file exists
	struct stat buffer;
	
	char fontPath[] = "font/VictorMono-Regular.otf";
  	if (!stat (fontPath, &buffer) == 0) {
		throw runtime_error("NO FONT in ./font directory");
	}
	ttf_font = al_load_ttf_font(fontPath, 14, 0);
	assert(ttf_font);
	
	graphics.setFont(ttf_font);
	if( myId == MASTER ) {
		al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_DIRECT3D_INTERNAL);
		display = al_create_display( D_WIDTH, D_HEIGHT );
		float scaleX = D_WIDTH / DIM;
		float scaleY = D_HEIGHT / DIM;
		al_identity_transform(&identityTransform);
		al_identity_transform(&resizeTransform);
		al_scale_transform(&resizeTransform, scaleX, scaleY);
		al_use_transform(&identityTransform);
		if (display == nullptr) return -1;
		al_set_window_title( display, "AC Project - V.R" );
		al_clear_to_color( al_map_rgb( 0, 0, 0 ) );
		DisplayAndMatrix dispAndMatrix;
		dispAndMatrix.display = display;
		dispAndMatrix.board1 = &board1;
		dispAndMatrix.board2 = &board2;
		t1 = al_create_thread( eventH, &dispAndMatrix );
		al_start_thread( t1 );
	}
	#endif
	
	//custom data-type to send initial matrix columns
	MPI_Type_vector( DIM, 1, DIM, MPITYPE, &bigColumn );
	MPI_Type_commit( &bigColumn );

	initBoardsALL( 0, board1, board2 );
	
	//main loop
	while( counter-- ) {
	
		//END CHECK ( needed to quit when you click the "x" button on the window )
		#if HAS_GRAPHICS
		if( stop )
			quit = true;

		MPI_Bcast( &quit, 1, MPI_C_BOOL, MASTER, MPI_COMM_WORLD );
		if( quit )
			break;

		MPI_Bcast( &switchAutoma, 1, MPI_C_BOOL, MASTER, MPI_COMM_WORLD );
		if( switchAutoma ) {
			initBoardsALL( board1.getCurrentAutomaIndex() + 1, board1, board2);
			switchAutoma = false;
		}

		MPI_Bcast( &restart, 1, MPI_C_BOOL, MASTER, MPI_COMM_WORLD );
		if( restart ) {
			initBoardsALL( board1.getCurrentAutomaIndex(), board1, board2);
			restart = false;
		}
		#endif
			
		//COMPUTING
		compute( &board1, &board2 );
		
		//PRINT
		computePrinting( &board2 );
		#if HAS_GRAPHICS
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );	
		#endif
	}
	
	#if HAS_GRAPHICS
	if( myId == MASTER ) {
		stop = true;
		al_destroy_thread( t1 );
		al_destroy_display( display );
	}
	#endif
	
	//START TIME
	if( myId == MASTER ) {
	endt = MPI_Wtime();
	cout << "Start: " << startt << endl;
	cout << "End: " << endt << endl;
	cout << "Elapsed: " << endt - startt << endl;
	}
	
	MPI_Finalize();  
	
	return 0;
}

void initBoardsALL( int automa, CMatrix<CTYPE>& board1, CMatrix<CTYPE>& board2 ) {
	//init processes boards
	int c1 = numCols - 2, c2 = 1;
	board2.selectAutoma(automa);
	board1.selectAutoma(automa);
	switch( myId ) {
		case MASTER :
		std::cout << "automa: " << automa << std::endl << "colorShift: " << board1.getColorShift() << std::endl;
		graphics.setColorShift(board1.getColorShift());
		initialBoard = new CMatrix<CTYPE>( DIM, DIM );
		initialBoard->selectAutoma( automa );
		initialBoard->initBoard();

		for( int y = 0; y < DIM; y++ )
		for( int x1 = 1, x = 0; x < numCols - 2; x++, x1++ )
		*board2.at( y, x1 ) = *initialBoard->at( y, x );

		for( int proc = 1; proc < numProcs; proc++ )
		for( int x = 0; x < DIM / numProcs; x++, c1++ )
		MPI_Send( initialBoard->col( c1 ), 1, bigColumn, proc, IM_TAG, MPI_COMM_WORLD );
		break;

		default:
		for( ; c2 < numCols - 1; c2++ )
		MPI_Recv( board2.col( c2 ), 1, column, MASTER, IM_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		break;
	}

	if( myId == MASTER )
		delete initialBoard;
}

//catches close event and switch automa
void *eventH( ALLEGRO_THREAD *thread, void *arg ) {
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_EVENT event;
	DisplayAndMatrix *displayAndMatrix = ( DisplayAndMatrix* ) arg;
	ALLEGRO_DISPLAY *display = displayAndMatrix->display;
	al_register_event_source( event_queue, al_get_display_event_source( display ) );	
	al_register_event_source( event_queue, al_get_keyboard_event_source() );
	while( true ) {
		al_wait_for_event( event_queue, &event );
		if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || stop ) {
			std::cout << "exit" << std::endl;
			stop = true;	
			return NULL;
		}
		else if( event.type == ALLEGRO_EVENT_KEY_DOWN ) { //switch automa
			switch(event.keyboard.keycode) {
				default:
					break;
				case ALLEGRO_KEY_Q :
					std::cout << "Switching Automa" << std::endl;
					switchAutoma = true;
					break;
				case ALLEGRO_KEY_R :
					std::cout << "Switching Automa" << std::endl;
					restart = true;
					break;
				case ALLEGRO_KEY_ESCAPE :
					std::cout << "restart" << std::endl;
					quit = true;
					break;
			}
		}
	}
	return NULL;
}

//MPI + AC stuff
void compute( CMatrix<CTYPE> *board1, CMatrix<CTYPE> *board2 ) {

	board1->copyMatrix( board2 );
	board2->clearMatrix();
	
	MPI_Request lastCol, firstCol;
	
	//receive MY first-- col from previous process
	int sen_id2 = ( myId - 1 >= 0 ) ? myId - 1 : numProcs - 1;
	MPI_Irecv( board1->col( 0 ), 1, column, sen_id2, LC_TAG, MPI_COMM_WORLD, &firstCol );
	
	//receive MY last++ col from next process
	int sen_id = ( myId + 1 < numProcs ) ? myId + 1 : 0;
	MPI_Irecv( board1->col( numCols - 1 ), 1, column, sen_id, FC_TAG, MPI_COMM_WORLD, &lastCol );
	
	//send MY last col to next process
	int rec_id2 = ( myId + 1 < numProcs ) ? myId + 1 : 0;
	MPI_Send( board1->col( numCols - 2 ), 1, column, rec_id2, LC_TAG, MPI_COMM_WORLD );
	
	//send MY first col to previous process
	int rec_id = ( myId - 1 >= 0 ) ? myId - 1 : numProcs - 1;
	MPI_Send( board1->col( 1 ), 1, column, rec_id, FC_TAG, MPI_COMM_WORLD );
	
	//computing from 2nd col to last - 2 col
	for( int y = 0; y < DIM; y++ )
		for( int x = 2; x < numCols - 2; x++ )
			*board2->at( y, x ) = board1->updatedValue( y, x, DIM, numCols );
	
	//computing first col
	MPI_Wait( &firstCol, MPI_STATUS_IGNORE );
	for( int y = 0; y < DIM; y++ )
		*board2->at( y, 1 ) = board1->updatedValue( y, 1, DIM, numCols );
		
	//computing last col
	MPI_Wait( &lastCol, MPI_STATUS_IGNORE );
	for( int y = 0; y < DIM; y++ )
		*board2->at( y, numCols - 2 ) = board1->updatedValue( y, numCols - 2, DIM, numCols );

}



void draw( int id, CTYPE *board, int dimY, int dimX, int master_plus ) {
	#if HAS_GRAPHICS
	CMatrix<CTYPE> temp( board, dimY, dimX );
	int offset = ( id * ( dimX - 2 ) + ( master_plus / 2 ) );
	for( int y = 0; y < dimY; y++ ) {
		for( int x = 1, x1 = 0; x < dimX - 1; x++, x1++ ) {
			graphics.draw( y, x1 + offset, *temp.at( y, x ) );
		}
	}
	#endif
}


void computePrinting( CMatrix<CTYPE> *board2 ) {
	switch( myId ) {
	case MASTER :
		#if HAS_GRAPHICS
		al_lock_bitmap(al_get_backbuffer(display),ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		graphics.clear();
		draw( MASTER, board2->getLinearMatrix(), DIM, numCols, master_plus );
		#endif
		for( int proc = 1; proc < numProcs; proc++ )
		{
			int nc = 0;
			MPI_Recv( &nc, 1, MPI_INT, proc, PRINT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
			int size = DIM * nc;
			CTYPE *lm = new CTYPE[ size ];
			MPI_Recv( lm, size, MPITYPE, proc, PRINT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
			#if HAS_GRAPHICS
			draw( proc, lm, DIM, nc, master_plus );
			#endif
			delete[] lm;			
		}
		#if HAS_GRAPHICS
		graphics.drawInfo();
		al_use_transform(&resizeTransform);
		al_flip_display();
		al_unlock_bitmap(al_get_backbuffer(display));
		#endif
	break;
	default:
		int nc = board2->getDimX();
		MPI_Send( &nc, 1, MPI_INT, MASTER, PRINT_TAG, MPI_COMM_WORLD );
		MPI_Send( board2->getLinearMatrix(), board2->getSize(), MPITYPE, MASTER, PRINT_TAG, MPI_COMM_WORLD );
	break;
	}

}

