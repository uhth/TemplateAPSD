#include "../include/uint8_array2d.h"

void Array2D::callAlgFuncOnEveryElement(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func) {
    for(size_t row = 0; row < width; ++row) {
        for(size_t col = 0; col < height; ++col) {
            *at(row,col) = func(this,row,col,width,height);
        }
    }
}

void Array2D::callAlgFuncOnEveryElementUseAux(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func) {
    clearAuxArray();
    for(size_t row = 0; row < width; ++row) {
        for(size_t col = 0; col < height; ++col) {
            auxArray[(row * width) + col] = func(this,row,col,width,height);
        }
    }
    swap();
}

void Array2D::clearAll() {
    clearArray();
    clearAuxArray();
}

void Array2D::swap() {
    uint_fast8_t *temp = array;
    array = auxArray;
    auxArray = temp;
}