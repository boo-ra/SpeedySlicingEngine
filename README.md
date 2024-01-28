# SpeedySlicingEngine
A high performance slicing engine
Copyright M. Bugra Akin

# Compile commands
cmake . 
make
Use clean.sh in case you need to rebuild. 

# Usage
./SpeedySlice (use default settings in hpse.h)
./SpeedySlice othergeom.stl (slice othergeom.stl, other settings from hpse.h)
./SpeedySlice othergoem.stl 0.2 (set slicing distance to 0.2, other settings from hpse.h)

Output: SVG files into new folder, created with geometry file name, in working directory

# stl3c3d	
This is a converter to the native c3d (compact 3d) format. 
The slicing engine does this currently internally and can't yet read c3d. 
The intention is to use stl2c3d later on.
