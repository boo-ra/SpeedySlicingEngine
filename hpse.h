#include <iostream>
#include <string>

//void hpse();

	
struct Configuration {
  
  std::string geomFile = "./geom.stl";
  std::string outputFormat = "svg";
  std::string outputLocation = "./";
  std::string outputName ;
  int printAreaXDim = 192;
  int printAreaYDim = 108;
  int imageXDim = 1920;
  int imageYDim = 1080;
  float layerThickness = 0.3;
};
	
