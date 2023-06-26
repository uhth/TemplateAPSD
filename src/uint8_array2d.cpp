#include "../include/uint8_array2d.h"
#include <stdexcept>

void Array2D::callAlgFuncOnEveryElement(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func) {
    for(size_t row = 0; row < width; ++row) {
        for(size_t col = 0; col < height; ++col) {
            *at(row,col) = func(this,row,col,width,height);
        }
    }
}

void Array2D::callAlgFuncOnEveryElementUseAux(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func) {
    for(size_t row = 0; row < width; ++row) {
        for(size_t col = 0; col < height; ++col) {
            auxArray[(row * width) + col] = func(this,row,col,width,height);
        }
    }
}

void Array2D::callAlgFuncOnRangeUseAux(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> func, Array2DRange& array2dRange) {
    for(int row = array2dRange.startRow; row < array2dRange.endRow; ++row) {
        for(int col = array2dRange.startCol; col < array2dRange.endCol; ++col) {
            auxArray[(row * width) + col] = func(this,row,col,array2dRange.rowBoundary,array2dRange.colBoundary);
        }
    }
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

void Array2D::cpyArray(Array2D* src, size_t l_offset, size_t r_offset) {
    if(src == nullptr)
        throw std::runtime_error("Src Array is nullptr");
    if( src->height != height || ((src->width - l_offset - r_offset) > width)) 
        throw std::runtime_error("Src Array must be same height of dest array, and width + offset must be < of dest width");
    for(size_t row = 0; row < src->height; ++row)
        for(size_t col = 0 + l_offset; col < width - r_offset; ++col)
            *at(row,col) = *src->at(row,col);
}