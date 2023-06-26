#include "../include/cellular_automata.h"
#include "../include/config.h"

#include <stdexcept>
#include <iostream>


/* COMMON */
bool yesOrNo(float probabilityOfYes) {
    return std::rand() % 5000 < (probabilityOfYes);
}

/* FRACTAL */
uint_fast8_t fractalAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
    //vertical scan from -1 to 1
    if((rowMax < 1) || (colMax < 1)) throw std::runtime_error("Array needs to be at least 1x1");
    int row_k, col_l;
    int diag = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 row
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  col
            col_l = (col == 0 && l == -1) ? -2 : (col + l >= colMax) ? -2 : col + l;
            if( ( !k && !l ) || col_l == -2) continue;
            if( ( k == -1 || k == 1 ) && ( l == -1 || l == 1 ) ) //diag tiles
                diag += *(array2d->at( row_k, col_l ));
        }
    }
    uint_fast8_t self = *(array2d->at( row, col ));
    uint_fast8_t newSelf = ( ( diag & 1 ) == 1 ) ? 1 : 0;
            
    return ( ( self << 1 ) | newSelf );
}

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataFractal::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = fractalAlgorithm;
    return algorithm;
}

uint_fast8_t AutomataFractal::getColorToneMultiplier() {
	return 10;
}

std::unique_ptr<Array2D> AutomataFractal::generateUniqueArray(const size_t& size) {
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    *(array2d->at( size / 2, (size / 2 ) + 1)) = 1;
	*(array2d->at( (size / 2) + 1, (size / 2 ) + 1)) = 1;
	*(array2d->at( (size / 2) + 1, size / 2 )) = 1;
	*(array2d->at( size / 2, size / 2 )) = 1;
    return array2d;
}

/* GAME OF LIFE */
uint_fast8_t golAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
    //vertical scan from -1 to 1
    int row_k, col_l, alive_nb = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
            col_l = (col == 0 && l == -1) ? -2 : (col + l >= colMax) ? -2 : col + l;
            if((!k && !l)|| col_l == -2) continue;
                alive_nb += *(array2d->at( row_k, col_l )) & 1;
        }
    }
    //edit this to change AC behaviour
    uint_fast8_t self = *(array2d->at( row, col ));
    uint_fast8_t newSelf = 0;
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

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataGoL::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = golAlgorithm;
    return algorithm;
}


std::unique_ptr<Array2D> AutomataGoL::generateUniqueArray(const size_t& size) {
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    long probability;
    for(size_t row = 0; row < size; row++)
        for( size_t col = 0; col < size; col++) {
            probability = std::rand() % 100;
            if(probability >= GOL_DENSITY) *(array2d->at(row, col)) = 1;
        }
    return array2d;
}

uint_fast8_t AutomataGoL::getColorToneMultiplier() {
	return 1;
}

/* FOREST */
uint_fast8_t forestAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
    //vertical scan from -1 to 1
    int row_k, col_l, on_fire_nb = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 Y
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1  X
            col_l = (col == 0 && l == -1) ? -2 : (col + l >= colMax) ? -2 : col + l;
            if((!k && !l)|| col_l == -2) continue;
                on_fire_nb += *(array2d->at( row_k, col_l )) == 2 ? 1 : 0;
        }
    }
    //edit this to change AC behaviour
    uint_fast8_t self = *(array2d->at(row, col));
    uint_fast8_t newSelf = 0;

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

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataForest::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = forestAlgorithm;
    return algorithm;
}

uint_fast8_t AutomataForest::getColorToneMultiplier() {
    return 122;
}

std::unique_ptr<Array2D> AutomataForest::generateUniqueArray(const size_t& size) {
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    return array2d;
}

/* ECOLIBRA */
uint_fast8_t ecolibraAutomata(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
//vertical scan from -1 to 1
    int row_k, col_l, SUM_8 = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 row
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1 col
            col_l = (col == 0 && l == -1) ? -2 : (col + l >= colMax) ? -2 : col + l;
            if((!k && !l)|| col_l == -2) continue;
            SUM_8 += *(array2d->at(row_k, col_l)) & 1 ? 1 : 0;
        }
    }
    //edit this to change AC behaviour
    uint_fast8_t self = *(array2d->at(row, col));
    uint_fast8_t newSelf = 0;

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

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataEcoLibra::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = ecolibraAutomata;
    return algorithm;
}

uint_fast8_t AutomataEcoLibra::getColorToneMultiplier() {
    return 120;
}

std::unique_ptr<Array2D> AutomataEcoLibra::generateUniqueArray(const size_t& size) {
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    size_t m1 = 30, m2 = 60;
    for( size_t row = 0; row < size; row++ )
        for( size_t col = 0; col < size; col++ ) {
            if( row < size / 2 ) *(array2d->at(row, col)) = 0; //half sea
            else *(array2d->at(row, col)) = 3; //half land
            if((row > m1 && row < (size / 2) - m2 ) && ( col > m1 && col < (size / 2) - m2 ) ) *(array2d->at(row, col)) = 3;
            if(row > (size / 2) + m2 && row < size - m1 && col < size - m1 && col > (size / 2) + m2) *(array2d->at(row, col)) = 0;
        }
    return array2d;
}

/* FADERS */
uint_fast8_t fadersAutomata(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
    //vertical scan from -1 to 1
    int row_k, col_l, SUM_8 = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 row
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1 col
            col_l = (col == 0 && l == -1) ? -2 : (col + l >= colMax) ? -2 : col + l;
            if((!k && !l)|| col_l == -2) continue;
            SUM_8 += *(array2d->at(row_k, col_l)) & 1 ? 1 : 0;
        }
    }
    //edit this to change AC behaviour
    
    uint_fast8_t N = 64, L = 2, U = 2, K = 2, Y = 2;
    
    uint_fast8_t self = *(array2d->at(row, col));
    uint_fast8_t newSelf = 0;
    
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


std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataFaders::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = fadersAutomata;
    return algorithm;
}

uint_fast8_t AutomataFaders::getColorToneMultiplier() {
    return 10;
}

std::unique_ptr<Array2D> AutomataFaders::generateUniqueArray(const size_t& size) {
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    *(array2d->at(size / 2, (size / 2 ) + 1)) = 1;
	*(array2d->at((size / 2) + 1, size / 2)) = 1;
	*(array2d->at(size / 2, size / 2)) = 1;
    return array2d;
}