cd Drones
cscript.exe start_simulated_drones.vbs \bin
cd ..
cd Scene
SET VREP_HOME=..\V-REP
copy .\Plugins\*.dll "%VREP_HOME%"
start_coverage_sim.bat
