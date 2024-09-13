#ifndef POINTERMANAGER_H
#define POINTERMANAGER_H

#include "sierrachart.h"
#include "TradeList.h"
#include "EquityCurve.h"

// TO DO: Old code, refactor, possibly better as a namespace?


class pointer_manager
{

public:

	pointer_manager() {}

	template <typename T>  
	void initPointer(SCStudyInterfaceRef sc, T* pointerVar, int pNum);

	template <typename T>
	bool nullPointerCheck(SCStudyInterfaceRef sc, T* pointerVar);

	template <typename T>
	void releasePointer(SCStudyInterfaceRef sc, T* pointerVar, int pNum);

	int process_pointers(SCStudyInterfaceRef sc, TradeList* cv, EquityCurve* eq);




};

#endif