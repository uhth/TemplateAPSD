#include "../include/cellular_automata.h"
#include "../include/uint8_array2d.h"
#include "../include/graphics.h"
#include "../include/config.h"
#include "../include/mpi_wrap.h"

#include <iostream>
#include <chrono>
#include <thread>

bool handleEvent(EVENT,std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void initAutomatas(std::vector<std::unique_ptr<Automata>>&);
void switchToNextAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void switchToPreviousAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);
void resetAutomata(std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,std::unique_ptr<Array2D>&,size_t&);

int main(int argc, char *argv[]) {
	// Mpi init
	MpiWrapper mpiWrapper(DIM,DIM);
	mpiWrapper.init(argc,argv);
	
	// Common Vars
	std::unique_ptr<Array2D> procArray2d = std::make_unique<Array2D>(mpiWrapper.num_cols, DIM);
	std::vector<std::unique_ptr<Automata>> automatas;
	size_t currentAutomata = 0;
	
	// Common Inits
	initAutomatas(automatas);

	/* PROCESS EXECUTION FLOW */
	if(mpiWrapper.my_id != 0) {
		mpiWrapper.recvInitialArray(procArray2d.get());
	}
	
	/* MASTER EXECUTION FLOW */
	if(mpiWrapper.my_id == 0) {
		GraphicsContext graphics(DIM, DIM);
		graphics.init();
		std::unique_ptr<Array2D> masterArray2d;
		resetAutomata(automatas, graphics, masterArray2d, currentAutomata);
		mpiWrapper.sendInitialArray(masterArray2d.get(), procArray2d.get());
		//loop
		while(handleEvent(graphics.popFromEventQueue(), automatas, graphics, masterArray2d, currentAutomata)) {
			//logic
			masterArray2d->callAlgFuncOnEveryElementUseAux(automatas[currentAutomata]->getAlgorithmFunc());
			//graphics
			graphics.printOnScreen(masterArray2d.get());
			//sleep
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
		}
		graphics.exit();
	}

	mpiWrapper.exit();

	return 0;
}

/* MASTER ONLY */
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