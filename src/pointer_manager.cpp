#include "pointer_manager.h"


template <typename T> 
void pointer_manager::initPointer(SCStudyInterfaceRef sc, T* pointerVar, int pNum) {

	if (pointerVar == NULL) {
		pointerVar = new T;
		if (pointerVar != NULL) {
			sc.SetPersistentPointer(pNum, pointerVar);
		}
		else {
			sc.AddMessageToLog("mem allocation error: inputs", 1);
		}
	}

}


template <typename T>
bool pointer_manager::nullPointerCheck(SCStudyInterfaceRef sc, T* pointerVar) {

	if (pointerVar == NULL) {
		sc.AddMessageToLog("null pointer found", 0);
		return 1;
	}
	else {
		return 0;
	}
}


template <typename T>
void pointer_manager::releasePointer(SCStudyInterfaceRef sc, T* pointerVar, int pNum) {

	if (pointerVar != NULL) {
		delete pointerVar;
		sc.SetPersistentPointer(pNum, NULL);
	}
}


int pointer_manager::process_pointers(SCStudyInterfaceRef sc, TradeList* cv, EquityCurve* eq) {


	initPointer(sc, cv, 0);
	initPointer(sc, eq, 1);


	if (sc.LastCallToFunction) {

		releasePointer(sc, cv, 0);
		releasePointer(sc, eq, 1);
	
		return -1;
	}


	if (nullPointerCheck(sc, cv) 
	||  nullPointerCheck(sc, eq)) {
		return -1;
	}
	else {
		return 0;
	}

}