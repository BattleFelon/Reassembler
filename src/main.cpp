#include <iostream>
#include <sstream>
#include <mutex>

#include "SettingsParser.h"
#include "ShipBuilder.h"
#include "Ship.h"
#include "TournamentManager.h"
#include "LogParser.h"
#include "Fleet.h"
#include "Mutator.h"
#include "FleetBuilder.h"
#include "FleetMutator.h"

int main()
{

	SettingsParser SP;
	SP.loadFromFile("config.txt");
	int mode;
	SP.get("program_mode", mode);

	Mutator M;
	FleetMutator FM;

	switch (mode){
		case 1:
			M.init();
			M.singleTargetMutator();
			break;
		case 2:
			M.init();
			M.poolMutator();
			break;
		case 3:
			M.init();
			M.seededMutator();
			break;
		case 4:
			FM.init();
			FM.bruteForceFleetBeater();
			break;
		case 5:
			FM.init();
			FM.geneticFleetBeater();
			break;
		default :
			std::cout << "Mode must be 1 - 3\n";
	}

    return EXIT_SUCCESS;
}
