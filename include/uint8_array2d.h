#ifndef UINT8_ARRAY2D
#define UINT8_ARRAY2D

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <functional>
#include <memory>

/* ARRAY2D */
/* WRAPS A ONE DIMENSIONAL ARRAY FOR IT TO BE USED AS A BI-DIMENSIONAL ARRAY */

struct Array2DRange {
    int startRow;
    int endRow;
    int startCol;
    int endCol;
    int rowBoundary;
    int colBoundary;
};

class Array2D {
    private:
        Array2D() {} // Beware of this, because it doesn't allocate memory on it's own

    protected:
        size_t width;
        size_t height;
        size_t size;
        uint_fast8_t* array;     // ACTUAL 1D ARRAY
        uint_fast8_t* auxArray;  // AUXILIARY 1D ARRAY

    public:
        /* ACCESS METHODS */
        inline uint_fast8_t* at( size_t index ) { return &array[index]; }
        inline uint_fast8_t* at( size_t row, size_t col ) { return at( ( row * width ) + col ); }
        inline uint_fast8_t* row( size_t index ) { return &array[ index * width ]; }
        inline uint_fast8_t* col( size_t index ) { return &array[ index ]; }

        /* CONSTRUCTORS - DESTRUCTOR - OPERATORS */
        Array2D(const Array2D& array2d) : width(array2d.width), height(array2d.height), size(array2d.height * array2d.width) // Copy Constructor
        { 
            array = new uint_fast8_t[size];
            auxArray = new uint_fast8_t[size];
            std::copy(array2d.array, array2d.array + array2d.size, array);
        }
        Array2D(const size_t& width, const size_t& height) : width(width), height(height), size(height * width) // Standard Constructor
        {
            array = new uint_fast8_t[size];
            auxArray = new uint_fast8_t[size];
        }
        ~Array2D() { // Destructor
            delete[] array;
            delete[] auxArray;
        }
        Array2D& operator=(const Array2D& array2d)
        {
            delete[] array;
            delete[] auxArray;
            width = array2d.width;
            height = array2d.height;
            size = array2d.size;
            array = new uint_fast8_t[size];
            auxArray = new uint_fast8_t[size];
            std::copy(array2d.array, array2d.array + array2d.size, array);
            return *this;
        }
        
        /* DATA MANIPULATION */
        void clearAll();
        void swap();
        inline void clearArray() {std::fill(array, array + size, 0);}
        inline void clearAuxArray() {std::fill(auxArray, auxArray + size, 0);}
        inline uint_fast8_t* getArray() {return array;};
        inline size_t getSize() {return size;}
        inline size_t getWidth() {return width;}
        inline size_t getHeight() {return height;}
        void cpyArray(Array2D*,size_t=0, size_t=0);
        void callAlgFuncOnEveryElement(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)>);
        void callAlgFuncOnEveryElementUseAux(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)>);
        void callAlgFuncOnRangeUseAux(std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)>, Array2DRange&);

};



#endif