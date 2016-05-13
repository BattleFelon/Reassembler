# Reassembler
Reassembly Genetic Algorithm for ship creation

Early alpha build of the program. This can randomly generate an importable ship into Reassembly. It has a GUI and depends on the following. 

SFML 2.3.2 

SFGUI 0.2.3
#Installation
Linux

Go into the Reassembler Directory

cmake -G "Unix Makefiles"   --- Or whatever you wanna build to. Code::Blocks. Whatever

make

./Reassembler

For windows just use cmake-gui. Have not tested this on windows

#About
So far all data for the block is stored in "data/block_data.csv". The block data contains the bounding vertex position information, the attachment information, including the normal angle, and other block data. 

Perfromance: The program can generate a usable ship file of 9000 P value in less than a second from init to writing in release

Here is an example of what I am thinking, but in wind turbine form

https://www.youtube.com/watch?v=YZUNRmwoijw

#TODO/Project Needs
Command line Call:
I need to figure out how to call the tournament command from terminal. I have tried almost everything in order to get it to work.

Current call-------

 ~/.steam/bin32/steam-runtime/run.sh /home/robv/.steam/steamapps/common/Reassembly/linux/./ReassemblySteam32 --HeadlessMode 1 --NetworkEnable 0 --LoadSuperFast 1 --SteamEnable 0 --TimeStampLog 0 --SandboxScript "bracket 8_ship1 8_ship2"
 
I cannot get the sandbox script arg to recgonize things after the first word. For instance.


--SandboxScript "help ship" // Brings up only help


--SandboxScript "ship 8_blarg" // Lists known ships

Also, when in headless mode there are no recognized ships. I need help from the dev as to how I can make this call work. Once that is done we can work on calling tournaments, getting results, and eveolving the ships.

Block Defs:
I need more assitacne in defining the block geometry. There are so many scales, shapes, and mirrored versions that it will take a lot of time for one person to compile accurate definitions. The current definitions appear to be perfectly accurate, and I havent seen aritifacts when imported. 

Genetic Mixing:
I have several ideas for this, but I can't move foreward without tournament calling. I think a good approach would be to break up the ship into random "chunks" of varrying sizes. The best performers from a population could have random chunks sampled from it. A set of chunks from one parent and another set from another could be added together until a new ship is formed. It would be possible to figure out which chunks were most responsible for a win, and an organization of specific blocks could be identified as superior. 

Thruster Adding:
I need to figure out what will work for thruster blocks. Thruster do not work as attachments for other blocks, which makes them unique. I will probably have a value for is_thruster. Just simple and works. Low on the TODO list
