#include "../include/cellular_automata.h"
#include "../include/config.h"

#include <stdexcept>
#include <iostream>
#include <cmath>


/* COMMON */
bool yesOrNo(float probabilityOfYes) {
    return std::rand() % 5000 < (probabilityOfYes);
}

/* AUTOMATA */
std::unordered_map<uint_fast8_t, bool> Automata::getMasquedValues() {
    std::unordered_map<uint_fast8_t, bool> map;
    map[0] = true;
    return map;
};

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
    uint_fast8_t self = *(array2d->at(row, col));
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

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataGOL::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = golAlgorithm;
    return algorithm;
}


std::unique_ptr<Array2D> AutomataGOL::generateUniqueArray(const size_t& size) {
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

uint_fast8_t AutomataGOL::getColorToneMultiplier() {
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
uint_fast8_t ecolibraAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
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
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = ecolibraAlgorithm;
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
uint_fast8_t fadersAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
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
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = fadersAlgorithm;
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

/* DIFFUSION-LIMITED AGGREGATION */
/*
(i) Select an integer q which is one of 2, 4, 8 or 64. Cells may be in any of the states 1 through q. Cells are either "fixed" or "mobile".
(ii) Select a grid size s.
(iii) Select integers k1 (range 5 through 100) and k2 (range 1 through minimum of s and 250), interpreted respectively as specifying the initial percentage concentration of cells on the grid and the number of seed cells.
(iv) Set up the initial state of the system by placing (a) k2 "seed" cells either at random locations on the grid or at certain predetermined positions and (b) the remaining cells (whose number is determined by k1 and the grid size) at random empty locations. The seed cells are all fixed and the other cells are all mobile. The seed cells are all in state q (and are usually colored white) and the other cells are all in state 1 (and are invisible).
(v) The system evolves as a succession of "steps", each of which consists of a succession of s2 "substeps".
(vi) A "substep" consists of the following process:
(a) Select a random location. If there is no mobile cell at this location then this substep is completed.
(b) If one of the neighboring locations has a fixed cell then this mobile cell changes to a fixed cell, and is assigned a state in the range 2 through q and an appropriate color (this completes this substep).
(c) If no neighboring location has a fixed cell then this mobile cell moves at random to one of the empty neighboring locations (if any).
(d) If the new location of the cell is beyond the circular region centered on the central location with radius s/2 then the cell ceases to exist.
(f) This completes a substep.
*/

uint_fast8_t Q = 64;

uint_fast8_t DLAAlgorithm(Array2D *array2d, size_t row, size_t col, size_t rowMax, size_t colMax) {
    if(*array2d->at(row,col) != 1) return *array2d->at(row,col);
    int row_k, col_l, fix_neigh = 0;
    for( int k = -1; k <= 1; k++ ) { //rows 0 -1 +1 row
        if(fix_neigh) break;
        row_k = (row == 0 && k == - 1) ? rowMax - 1 : (row + k >= rowMax) ? 0 : row + k;
        for( int l = -1; l <= 1; l++ ) { //columns 0 -1 +1 col
            col_l = (col == 0 && l == -1) ? colMax - 1 : (col + l >= colMax) ? 0 : col + l;
            if(!k && !l) continue;
            if (*(array2d->at(row_k, col_l)) > 1) {
                fix_neigh = 1;
                break;
            } 
        }
    }
    if(fix_neigh) {
        uint_fast8_t newValue = (std::rand() % Q) + 2;
        return newValue;
    }
    else {
        int newRow, newCol;
        int iRowMax = rowMax, iColMax = colMax;
        do {
            size_t moveLeftRight = std::rand() % 2;
            size_t moveUpDown = std::rand() % 2;
            moveLeftRight = (!moveLeftRight) ? -1 : moveLeftRight;
            moveUpDown = (!moveUpDown) ? -1 : moveUpDown;
            newRow = row + moveLeftRight;
            newCol = col + moveUpDown;
        }
        while(!(newRow > 0 && newCol > 0 && newRow < iRowMax && newCol < iColMax && array2d->at(newRow, newCol) != 0 && !array2d->getOverrideMap().count(Coords2D{.row = newRow, .col = newCol})));

        int sqrd_dist = pow(((rowMax * 0.5) - newRow), 2) + pow(((colMax * 0.5) - newCol), 2);
        int sqrd_radius = pow(rowMax, 2);

        if(sqrd_dist <= (sqrd_radius * 0.2f)) {
            array2d->getOverrideMap()[Coords2D{.row = newRow, .col = newCol}] = 1;
        }
        return 0;
    }
    return *array2d->at(row, col);
}

uint_fast8_t AutomataDLA::getColorToneMultiplier() {
    return 1;
}

std::unique_ptr<Array2D> AutomataDLA::generateUniqueArray(const size_t& size) {
    //percentage of cells
    size_t k1 = 70;
    std::unique_ptr<Array2D> array2d = std::make_unique<Array2D>(size, size);
    array2d->clearArray();
    *(array2d->at(size >> 1, size >> 1)) = Q;
    //num of mobile cells
    size_t num_m_cells = pow(size,2) * k1 * 0.01f;
    for(size_t i = 0; i < num_m_cells; ++i) {
        size_t row = std::rand() % size;
        size_t col = std::rand() % size;
        if(row == (size >> 1) || col == (size >> 1) + 1) continue;
        *array2d->at(row, col) = 1;
    }
    return array2d;
}

std::function<uint_fast8_t(Array2D  *array2d, size_t row, size_t col, size_t rowMax, size_t colMax)> AutomataDLA::getAlgorithmFunc() {
    std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> algorithm = DLAAlgorithm;
    return algorithm;
}

std::unordered_map<uint_fast8_t, bool> AutomataDLA::getMasquedValues() {
    std::unordered_map<uint_fast8_t, bool> map;
    map[0] = true;
    map[1] = true;
    return map;
}