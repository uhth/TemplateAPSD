#ifndef AUTOMATAS_H
#define AUTOMATAS_H

#include "config.h"

template <typename T> 
class CMatrix;

template<class T>
class Automa {
	public:
	virtual T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) = 0;

	virtual void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) = 0;

	virtual int getColorShift() { return 10; }
};

template<class T>
class AutomaFractal : public Automa<T> {
	public:
 	T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) override {
		//vertical scan from -1 to 1
		int y_k;
		int x_l;
		int diag = 0;
		for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
			y_k = y + k;
			if( y_k < 0 ) y_k = Ymax - 1;
			else if( y_k >= Ymax ) y_k = 0;
	 		for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
				x_l = x + l;
				if( ( !k && !l ) || x_l >= Xmax || x_l < 0 ) continue;
				if( ( k == -1 || k == 1 ) && ( l == -1 || l == 1 ) ) //diag tiles
					diag += *(matrix->at( y_k, x_l ));
			}
		}
				float p_tree = 10;
		//edit this to change AC behaviour
        T self = *(matrix->at( y, x ));
		T newSelf = ( ( diag & 1 ) == 1 ) ? 1 : 0;
				
		return ( ( self << 1 ) | newSelf );
	}

	int getColorShift() override {
		return 10;
	}

	void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) override { 		
		*(matrix->at( dimY / 2, (dimX / 2 ) + 1)) = 1;
		*(matrix->at( (dimY / 2) + 1, (dimX / 2 ) + 1)) = 1;
		*(matrix->at( (dimY / 2) + 1, dimX / 2 )) = 1;
		*(matrix->at( dimY / 2, dimX / 2 )) = 1;
	}

};


template<class T>
class AutomaGoL: public Automa<T> {
	public:
	T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) override {
		//vertical scan from -1 to 1
		int y_k;
		int x_l;
		int alive_nb = 0;
		for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
			y_k = y + k;
			if( y_k < 0 ) y_k = Ymax - 1;
			else if( y_k >= Ymax ) y_k = 0;
	 		for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
				x_l = x + l;
				if( ( !k && !l ) || x_l >= Xmax || x_l < 0 ) continue;
					alive_nb += *(matrix->at( y_k, x_l )) & 1;
			}
		}
		//edit this to change AC behaviour
        T self = *(matrix->at( y, x ));
		T newSelf = 0;
		switch( self & 1 ) {
			case true:
				newSelf = ( alive_nb == 2 || alive_nb == 3 ) ? self : 0;
			break;
			case false:
				if( alive_nb == 3 ) newSelf = 1;
			break;				
		}				
		return newSelf;
	}
    void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) override { 		
        long a;
        for( int y = 0; y < dimY; y++ )
            for( int x = 0; x < dimX; x++ ) {
                a = std::rand() % 100;
                if( a >= GOL_DENSITY ) *(matrix->at( y, x )) = 1;
            }
	}

	int getColorShift() override {
		return 1;
	}

};

template<class T>
class AutomaForest: public Automa<T> {
	public:
	T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) override {
		//vertical scan from -1 to 1
		int y_k;
		int x_l;
		int on_fire_nb = 0;
		for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
			y_k = y + k;
			if( y_k < 0 ) y_k = Ymax - 1;
			else if( y_k >= Ymax ) y_k = 0;
	 		for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
				x_l = x + l;
				if( ( !k && !l ) || x_l >= Xmax || x_l < 0 ) continue;
					on_fire_nb += *(matrix->at( y_k, x_l )) == 2 ? 1 : 0;
			}
		}
		//edit this to change AC behaviour
        T self = *(matrix->at( y, x ));
		T newSelf = 0;

		if( self == 0 && yesOrNo(P_TREE)) {
			newSelf = 1;
		}		
		else if( self == 2 ) {
			newSelf = 0;
		}		
		else if( self == 1 && ( yesOrNo(P_ZAPP) || on_fire_nb > 0 ) ) {
			newSelf = 2;
		}
		else newSelf = self;

		return newSelf;
	}

	int getColorShift() override {
		return 122;
	}

	bool yesOrNo(float probabilityOfYes) {
		return std::rand() % 5000 < (probabilityOfYes);
	}

	void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) override { 		
	}
};

template<class T>
class AutomaEcolibra: public Automa<T> {
	public:
        

     /*  Here rather than thinking of bits, we think of state numbers.

        State 0 is dead sea
        State 1 is firing brain in sea
        State 2 is refractory brain in sea
        State 3 is dead land
        State 4 is firing life on land  */
     
	T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) override {
		//vertical scan from -1 to 1
		int y_k;
		int x_l;
		int SUM_8 = 0;
		for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
			y_k = y + k;
			if( y_k < 0 ) y_k = Ymax - 1;
			else if( y_k >= Ymax ) y_k = 0;
	 		for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
				x_l = x + l;
				if( ( !k && !l ) || x_l >= Xmax || x_l < 0 ) continue;
					SUM_8 += *(matrix->at( y_k, x_l )) & 1 ? 1 : 0;
			}
		}
		//edit this to change AC behaviour
        T self = *(matrix->at( y, x ));
		T newSelf = 0;

		if ((self & 1) != 0) {
        newSelf = 3;
        } else {
            newSelf = 0;
        }

        if (self == 0) {
            switch (SUM_8) {
                case 2:
                    newSelf = 1;
                    break;

                case 6:
                case 7:
                    newSelf = 3;
                    break;

                default:
                    newSelf = 0;
                    break;
            }
        } else if (self == 1) {
            newSelf = 2;
        } else if (self == 2) {
            newSelf = 0;
        } else if (self == 3) {
            switch (SUM_8) {
                case 5:
                    newSelf = 4;
                    break;

                case 1:
                    newSelf = 0;
                    break;

                default:
                    newSelf = 3;
                    break;
            }
        } else if (self == 4) {
            if (SUM_8 == 5 || SUM_8 == 6) {
                newSelf = 4;
            } else {
                newSelf = 3;
            }
        }
        return newSelf;
	}

	int getColorShift() override {
		return 15;
	}


    void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) override { 	
        int m1 = 30;
        int m2 = 60;
        for( int y = 0; y < dimY; y++ )
            for( int x = 0; x < dimX; x++ ) {
                if( y < dimY / 2 ) *(matrix->at( y, x )) = 0; //half sea
                else *(matrix->at( y, x )) = 3; //half land
                
                if( ( y > m1 && y < (dimY / 2) - m2 ) && ( x > m1 && x < (dimX / 2) - m2 ) ) *(matrix->at( y, x )) = 3;
                if( y > (dimY / 2) + m2 && y < dimY - m1 && x < dimX - m1 && x > (dimX / 2) + m2 ) *(matrix->at( y, x )) = 0;
            }
	}
};


template<class T>
class AutomaFaders: public Automa<T> {
	public:
	T execute(int y, int x, int Ymax, int Xmax, CMatrix<T>* matrix) override {
		//vertical scan from -1 to 1
		int y_k;
		int x_l;
		int SUM_8 = 0;
		for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
			y_k = y + k;
			if( y_k < 0 ) y_k = Ymax - 1;
			else if( y_k >= Ymax ) y_k = 0;
	 		for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
				x_l = x + l;
				if( ( !k && !l ) || x_l >= Xmax || x_l < 0 ) continue;
					SUM_8 += *(matrix->at( y_k, x_l )) & 1;
			}
		}
		//edit this to change AC behaviour
		
		T N = 64, L = 2, U = 2, K = 2, Y = 2;
		
        T self = *(matrix->at( y, x ));
		T newSelf = 0;
        
        if ((self == 0) && (L <= SUM_8) && (SUM_8 <= U)) {
            newSelf = 1;
        }
        if (self == 1) {
            if ((K <= SUM_8) && (SUM_8 <= Y)) {
                newSelf = 1;
            } else {
                newSelf = 2;
            }
        }
        if (((self & 1) == 0) && (0 < self) && (self < (2 * N))) {
            newSelf = self + 2;
        }		
		return newSelf;
	}

	int getColorShift() override {
		return 10;
	}
	
	void initMatrix(int dimY, int dimX, CMatrix<T>* matrix) override { 	
        int value = 1;
		*(matrix->at( dimY / 2, (dimX / 2 ) + 1)) = value;
		*(matrix->at( (dimY / 2) + 1, dimX / 2 )) = value;
		*(matrix->at( dimY / 2, dimX / 2 )) = value;
	}

};

template<typename T>
T execute() {
	return 0;
}

#endif