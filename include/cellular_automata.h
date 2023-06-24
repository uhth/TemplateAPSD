#ifndef CELLULAR_AUTOMAtaTAS_H
#define CELLULAR_AUTOMAtaTAS_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include "../include/uint8_array2d.h"

class Automata {
	public:
	    virtual std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc() = 0;
        virtual std::unique_ptr<Array2D> generateUniqueArray(const size_t&) = 0;
	    virtual uint_fast8_t getColorToneMultiplier() = 0;
};

class AutomataFractal : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc() override;
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier() override;
};
class AutomataGoL : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
};
class AutomataForest : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
};
class AutomataEcoLibra : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
};
class AutomataFaders : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
};

#endif