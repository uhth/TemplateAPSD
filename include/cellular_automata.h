#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <map>
#include "../include/uint8_array2d.h"

class Automata;

class Automata {
	public:
	    virtual std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc() = 0;
        virtual std::unique_ptr<Array2D> generateUniqueArray(const size_t&) = 0;
	    virtual uint_fast8_t getColorToneMultiplier() = 0;
        virtual std::unordered_map<uint_fast8_t, bool> getMasquedValues();
        inline virtual bool canMultiProcess() { return true; }
        virtual std::string name() = 0;
};

class AutomataFractal : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc() override;
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier() override;
        std::string name() override { return "Fractal"; }
};
class AutomataGOL : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::string name() override { return "GOL"; }
};
class AutomataForest : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::string name() override { return "Forest"; }
};
class AutomataEcoLibra : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::string name() override { return "EcoLibra"; }
};
class AutomataFaders : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::string name() override { return "Faders"; }
};
class AutomataRainzha : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::string name() override { return "Rainzha"; }
};
class AutomataDLA : public Automata {
    public:
        std::function<uint_fast8_t(Array2D*,size_t,size_t,size_t,size_t)> getAlgorithmFunc();
        std::unique_ptr<Array2D> generateUniqueArray(const size_t&) override;
        uint_fast8_t getColorToneMultiplier();
        std::unordered_map<uint_fast8_t, bool> getMasquedValues() override;
        inline bool canMultiProcess() override { return false; }
        std::string name() override { return "DLA"; }
};


#endif