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
#include "Display.h"

int main()
{
	Display disp;
	disp.run();

    return EXIT_SUCCESS;
}
