#include "../include/cellular_automata.h"
#include "../include/uint8_array2d.h"
#include "../include/graphics.h"
#include "../include/config.h"
#include "../include/mpi_wrap.h"

#include <iostream>
#include <chrono>
#include <thread>

bool handleEventMaster(EVENT,std::vector<std::unique_ptr<Automata>>&,GraphicsContext&,MpiWrapper&,std::unique_ptr<Array2D>&,std::unique_ptr<Array2D>&,size_t&);
bool handleEventProc(EVENT,std::vector<std::unique_ptr<Automata>>&,MpiWrapper&,std::unique_ptr<Array2D>&,size_t&);
void initAutomatas(std::vector<std::unique_ptr<Automata>>&,MpiWrapper&);

bool paused = false;

int main(int argc, char *argv[]) {
	// Mpi init
	MpiWrapper mpiWrapper(DIM,DIM);
	mpiWrapper.init(argc,argv);
	
	// Common Vars
	std::unique_ptr<Array2D> procArray2d = std::make_unique<Array2D>(mpiWrapper.num_cols, DIM);
	std::vector<std::unique_ptr<Automata>> automatas;
	size_t currentAutomata = 0;
	
	// Common Inits
	initAutomatas(automatas, mpiWrapper);


	/* PROCESS EXECUTION FLOW */
	if(mpiWrapper.my_id != 0) {
		mpiWrapper.recvInitialArray(procArray2d.get());
		//loop
		while(1) {
			EVENT event = mpiWrapper.syncEvents();
			if(!handleEventProc(event, automatas, mpiWrapper, procArray2d, currentAutomata)) break;
			if(paused) continue;
			mpiWrapper.ProcessAndSyncProcArray2d(procArray2d.get(), automatas[currentAutomata]->getAlgorithmFunc());
			mpiWrapper.sendProcArray(procArray2d.get());
			mpiWrapper.syncToBarrier();
		}
	}

	/* MASTER EXECUTION FLOW */
	if(mpiWrapper.my_id == 0) {
		long startTime = MPI_Wtime();
		GraphicsContext graphics(DIM, DIM);
		graphics.init();
		std::unique_ptr<Array2D> masterArray2d;
		handleEventMaster(EVENT::RESET, automatas, graphics, mpiWrapper, masterArray2d, procArray2d, currentAutomata);
		//loop
		while(1) {
			EVENT event = graphics.popFromEventQueue();
			mpiWrapper.syncEvents(event);
			if(!handleEventMaster(event, automatas, graphics, mpiWrapper, masterArray2d, procArray2d, currentAutomata)) break;
			//logic
			if(paused) continue;
			mpiWrapper.ProcessAndSyncProcArray2d(procArray2d.get(), automatas[currentAutomata]->getAlgorithmFunc());
			masterArray2d->clearArray();
			mpiWrapper.recvProcArray(masterArray2d.get());
			mpiWrapper.putMastersContribute(masterArray2d.get(), procArray2d.get());
			//graphics
			graphics.printOnScreen(masterArray2d.get());
			//sleep
			if(SLEEP_TIME > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
			mpiWrapper.syncToBarrier();
		}
		graphics.exit();
	long elapsedTime = MPI_Wtime() - startTime;
	std::cout << "Elapsed Time: " << elapsedTime << " seconds" << std::endl;
	}

	mpiWrapper.exit();

	return 0;
}

/* MASTER ONLY */
bool handleEventMaster(EVENT event, std::vector<std::unique_ptr<Automata>>& automatas, GraphicsContext& graphics, MpiWrapper& mpiWrapper, std::unique_ptr<Array2D>& masterArray2d, std::unique_ptr<Array2D>& procArray2d, size_t& currentAutomata) {
	if(mpiWrapper.my_id != 0) return true;
	switch(event) {
		case EVENT::EXIT:
			return false;
		case EVENT::PAUSE:
			paused = !paused;
			break;
		case EVENT::NONE:
			break;
		case EVENT::RESET:
			masterArray2d = automatas[currentAutomata]->generateUniqueArray(DIM);
			graphics.setColorToneMultiplier(automatas[currentAutomata]->getColorToneMultiplier());
			graphics.setColorMask(automatas[currentAutomata]->getMasquedValues());
			mpiWrapper.sendInitialArray(masterArray2d.get(), procArray2d.get());
			break;
		case EVENT::NEXT:
			currentAutomata = (currentAutomata + 1) % automatas.size();
			masterArray2d = automatas[currentAutomata]->generateUniqueArray(DIM);
			graphics.setColorToneMultiplier(automatas[currentAutomata]->getColorToneMultiplier());
			graphics.setColorMask(automatas[currentAutomata]->getMasquedValues());
			mpiWrapper.sendInitialArray(masterArray2d.get(), procArray2d.get());
			break;
		case EVENT::PREVIOUS:
			currentAutomata = (currentAutomata == 0) ? automatas.size() - 1 : currentAutomata - 1;
			masterArray2d = automatas[currentAutomata]->generateUniqueArray(DIM);
			graphics.setColorToneMultiplier(automatas[currentAutomata]->getColorToneMultiplier());
			graphics.setColorMask(automatas[currentAutomata]->getMasquedValues());
			mpiWrapper.sendInitialArray(masterArray2d.get(), procArray2d.get());
			break;
	};
	return true;
}

void initAutomatas(std::vector<std::unique_ptr<Automata>>& automatas, MpiWrapper& mpiWrapper) {
	
	automatas.push_back(std::make_unique<AutomataEcoLibra>());
	automatas.push_back(std::make_unique<AutomataGOL>());
	automatas.push_back(std::make_unique<AutomataForest>());
	automatas.push_back(std::make_unique<AutomataFaders>());
	automatas.push_back(std::make_unique<AutomataFractal>());
	automatas.push_back(std::make_unique<AutomataDLA>());

	if(mpiWrapper.num_procs == 1) return;
	auto it = automatas.begin();
	for(; it < automatas.end(); ++it) {
		if(!(*it)->canMultiProcess())
			automatas.erase(it);
	}

}


bool handleEventProc(EVENT event, std::vector<std::unique_ptr<Automata>>& automatas, MpiWrapper& mpiWrapper, std::unique_ptr<Array2D>& procArray2d, size_t& currentAutomata) {
	if(mpiWrapper.my_id == 0) return true;
	switch(event) {
		case EVENT::EXIT:
			return false;
		case EVENT::PAUSE:
			paused = !paused;
			break;
		case EVENT::NONE:
			break;
		case EVENT::RESET:
			mpiWrapper.recvInitialArray(procArray2d.get());
			break;
		case EVENT::NEXT:
			currentAutomata = (currentAutomata + 1) % automatas.size();
			mpiWrapper.recvInitialArray(procArray2d.get());
			break;
		case EVENT::PREVIOUS:
			currentAutomata = (currentAutomata == 0) ? automatas.size() - 1 : currentAutomata - 1;
			mpiWrapper.recvInitialArray(procArray2d.get());
			break;
	};
	return true;
}