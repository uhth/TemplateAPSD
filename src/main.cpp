
#include "../include/cellular_automata.h"
#include "../include/uint8_array2d.h"
#include "../include/graphics.h"
#include "../include/config.h"

#include <iostream>
#include <chrono>
#include <thread>

bool handleEvent(EVENT,std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void initAutomatas(std::vector<std::unique_ptr<Automata>>&);
void switchToNextAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void switchToPreviousAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void resetAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);

int main() {
	//vars
	std::vector<std::unique_ptr<Automata>> automatas;
	std::unique_ptr<Array2D> array2d;
	GraphicsContext graphics(DIM, DIM);
	size_t currentAutomata = 0;
	//inits
	initAutomatas(automatas);
	graphics.init();
	resetAutomata(automatas, graphics, array2d, currentAutomata);

	while(handleEvent(graphics.popFromEventQueue(), automatas, graphics, array2d, currentAutomata)) {
		//logic
		array2d->callAlgFuncOnEveryElementUseAux(automatas[currentAutomata]->getAlgorithmFunc());
		//graphics
		graphics.printOnScreen(array2d.get());
		//sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
	}
	graphics.exit();
	return 0;
}

bool handleEvent(EVENT event, std::vector<std::unique_ptr<Automata>>& automatas, GraphicsContext& graphics, std::unique_ptr<Array2D>& array2d, size_t& currentAutomata) {
	switch(event) {
		case EVENT::EXIT:
			return false;
		case EVENT::NONE:
			break;
		case EVENT::RESET:
			resetAutomata(automatas, graphics, array2d, currentAutomata);
			break;
		case EVENT::NEXT:
			switchToNextAutomata(automatas, graphics, array2d, currentAutomata);
			break;
		case EVENT::PREVIOUS:
			switchToNextAutomata(automatas, graphics, array2d, currentAutomata);
			break;
	};
	return true;
}

void initAutomatas(std::vector<std::unique_ptr<Automata>>& automatas) {
	automatas.push_back(std::make_unique<AutomataEcoLibra>());
	automatas.push_back(std::make_unique<AutomataForest>());
	automatas.push_back(std::make_unique<AutomataGoL>());
	automatas.push_back(std::make_unique<AutomataFaders>());
	automatas.push_back(std::make_unique<AutomataFractal>());
}

void switchToNextAutomata(std::vector<std::unique_ptr<Automata>>& automatas, GraphicsContext& graphics, std::unique_ptr<Array2D>& array2d, size_t& currentAutomata) {
	currentAutomata = (currentAutomata + 1) % automatas.size();
	resetAutomata(automatas, graphics, array2d, currentAutomata);
}

void switchToPreviousAutomata(std::vector<std::unique_ptr<Automata>>& automatas, GraphicsContext& graphics, std::unique_ptr<Array2D>& array2d, size_t& currentAutomata) {
	currentAutomata = (currentAutomata - 1 >= 0) ? currentAutomata - 1 : automatas.size() - 1;
	resetAutomata(automatas, graphics, array2d, currentAutomata);
}

void resetAutomata(std::vector<std::unique_ptr<Automata>>& automatas, GraphicsContext& graphics, std::unique_ptr<Array2D>& array2d, size_t& currentAutomata) {
	array2d = automatas[currentAutomata]->generateUniqueArray(DIM);
	graphics.setColorToneMultiplier(automatas[currentAutomata]->getColorToneMultiplier());
}