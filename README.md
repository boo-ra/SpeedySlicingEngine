# SpeedySlicingEngine
A high performance slicing engine
Copyright © 2024 M. Bugra Akin
Method description: meshnomo.com
Patent pending https://worldwide.espacenet.com/patent/search?q=pn%3DWO2023102583A1

# Compile commands
cmake . \
make \
Use clean.sh in case you need to rebuild \
To compile stl2c3d run build.sh

# Usage
./SpeedySlice (use default settings in hpse.h) \
./SpeedySlice othergeom.stl (slice othergeom.stl, other settings from hpse.h) \
./SpeedySlice othergoem.stl 0.2 (set slicing distance to 0.2, other settings from hpse.h) \
./stl2c3d inputfile.stl outputfile.c3d \
./stl2c3d inputfile.stl outputfile.vtk 

Output: SVG files into new folder, created with geometry file name, in working directory

# stl3c3d	
This is a converter to the native c3d (compact 3d) format. 
The slicing engine does this currently internally and can't yet read c3d. 
The intention is to use stl2c3d later on.

# Now issues/bugs
bug: The inside color of a contour (SVG file) is seldomly chosen wrong. Selection criteria needs improvement. If you don't see the contour this is probably the reason. 
issue: Tested only on Ubuntu. Might eventually cause a crash on other systems.
