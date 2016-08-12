#include <iostream>
#include <sstream>

#ifdef __linux__
#include <unistd.h>
#endif // __linux__

#include "TournamentManager.h"

TournamentManager::TournamentManager()
{

}

TournamentManager::~TournamentManager()
{

}

void TournamentManager::startArena(Ship& ship_1, Ship& ship_2)
{
    //This mess calls reassembly correctly. Damn this was hard

    std::ostringstream command;
    command <<"\"\"";
    command << exe_path;
    command << "\\ReassemblyRelease.exe";
    command << "\" ";
    //Command line args
    command << "--HeadlessMode=1 --NetworkEnable=0 --LoadSuperFast=1 --SteamEnable=0 --TimestampLog=0 --EnableDevBindings=1 --SandboxScript=\"arena \'";
    command << ships_path << "\\" << ship_1.getShipName() << ".lua\' \'" << ships_path << "\\" << ship_2.getShipName() << ".lua\'\"\"";

    system(command.str().c_str());

}

void TournamentManager::startString(std::string ship_1, std::string ship_2)
{

	//This mess calls reassembly correctly. Damn this was hard
	#ifdef _WIN32
	std::ostringstream command;
	command << "\"\"";
	command << exe_path;
	command << "\\ReassemblyRelease.exe";
	command << "\" ";
	//Command line args
	command << "--HeadlessMode=1 --NetworkEnable=0 --LoadSuperFast=1 --SteamEnable=0 --TimestampLog=0 --EnableDevBindings=1 --SandboxScript=\"arena \'";
	command << ships_path << "\\" << ship_1 << ".lua\' \'" << ships_path << "\\" << ship_2 << ".lua\'\"\"";

	system(command.str().c_str());
	#endif // _WIN32
	#ifdef __linux__
	//std::cout << ship_1 << " " << ship_2 << "\n";
	#endif // __linux__
}

void TournamentManager::startStringThreaded(std::string ship_1, std::string ship_2,int *delay)
{
	clock_t timer = clock();
	//This mess calls reassembly correctly. Damn this was hard

	//Log naming
	static int log_number = 0;
	log_number++;

	std::string log_name = "--LogFileName=\"data/log_";
	log_name.append(std::to_string(log_number));
	log_name.append(".txt\"");

	//This ugly bastard
	std::ostringstream command;
	command << "\"\"";
	command << exe_path;
	command << "\\ReassemblyRelease.exe";
	command << "\" ";
	//Command line args
	command << "--HeadlessMode=1 --NetworkEnable=0 --LoadSuperFast=1 --SteamEnable=0 --TimestampLog=0 " << log_name << " --EnableDevBindings=1 --SandboxScript=\"arena \'";
	command << ships_path << "\\" << ship_1 << ".lua\' \'" << ships_path << "\\" << ship_2 << ".lua\'\"\"";

	system(command.str().c_str());

	*delay = clock() - timer;
}