#include <cassert>
#include <iostream>

#include <chrono>
#include <filesystem>
#include <string>

#include "parse_stl.h"
#include "write_files.h"
#include "slice.h"

namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
	
  std::cout << std::endl << "----------------- SpeedySlice v0.1 ----------------" << std::endl;
  std::cout << std::endl << "© Copyright 2024 M. Bugra Akin. All Rights Reserved." << std::endl << std::endl;
  
  // start timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  
  //set::json conf;
  Configuration conf;
  
  // check if file exists	
  std::string inputFile = conf.geomFile;
  if (argc >= 2) {
    inputFile = argv[1];
  } 
  
  // check if output format specified
  std::string output_format = conf.outputFormat;
  if (argc >= 3) {
    conf.layerThickness = std::stof(argv[2]);
  }
	// create folder for slices
	std::size_t posLastSlash = inputFile.find_last_of("/");
	std::size_t posLastDot = inputFile.find_last_of(".");
	std::string folder_name = inputFile.substr(0, posLastDot) + "_SpeedySlice";
	fs::create_directory(folder_name);
	conf.outputLocation = folder_name + "/";
	conf.outputName = inputFile.substr(posLastSlash+1,std::string::npos);
	std::cout << conf.outputName << std::endl;
  
  if (output_format != "svg" && output_format != "vtk")// || output_format != "png") 
  {
	  std::cout << "WARNING: Specified output format not recognized! Setting output to default (SVG)!" << std::endl;
	  output_format = "svg";
  }
  
  // initialize variables and containers 
  bool isbinary = stl::is_binary_stl(inputFile);
  std::vector<stl::vertex> vertices;
  stl::stl_data info;
  std::vector<stl::edge> edges;
  //std::cout << info.first_member[125000-1] << std::endl;
  
  // read stl and sort vertices
  if (isbinary) 
  {
	  stl::parse_binary_stl(inputFile, &info, &vertices, &edges, 0);
	  std::cout << "STL HEADER = " << info.name << std::endl;
	  //std::cout << "x bound: " << info.xmin <<' '<< info.xmax << std::endl;
	  //std::cout << "y bound: " << info.ymin <<' '<< info.ymax << std::endl;
	  //std::cout << "z bound: " << info.zmin <<' '<< info.zmax << std::endl;
	  //std::cout << "number of triangles = " << info.num_triangles << std::endl;
  	  info.nboxxo = fmin(fmax(1,info.num_triangles/10000)*10,1000);
	  //std::cout << "number of boxes = " << info.nboxxo << std::endl;
	  info.nboxxa = info.nboxxo * info.nboxxo * info.nboxxo;
      info.init_member_pointers();
      info.get_boxxdim();
	  stl::parse_binary_stl(inputFile, &info, &vertices, &edges, 1);
	  //std::cout << "number of vertices = " << info.num_vertices << std::endl;
	  //std::cout << "number of edges = " << info.num_edges << std::endl;
	  //for (auto t : vertices) {
	  //  std::cout << t << std::endl; }  
  } else {
	  stl::parse_ascii_stl(inputFile, &info, &vertices, &edges, 0);  
  }
  
  std::chrono::steady_clock::time_point time_parse = std::chrono::steady_clock::now();
  std::cout << "Preprocessing time = " << std::chrono::duration_cast<std::chrono::milliseconds>(time_parse - begin).count() << "[ms]" << std::endl;
  
  info.baseES = conf.layerThickness;
  scr::add_sort_info_z(&info, &vertices, &edges);
  float zrange = info.zmax - info.zmin;
  unsigned int num_slices = floor(zrange/info.baseES);
  //std::vector<scr::slice> slices[num_slices+1]; // works!!!! 
  std::cout << "start slicing..." << std::endl;
  float slat;
  std::vector<scr::slice> slices;  
  slat = info.zmin + (num_slices+1)*info.baseES;
  #pragma omp parallel private (slat,slices)
  {

  	  #pragma omp for nowait 
  	  for (unsigned int i = 1; i < num_slices+1; i++) 
  	  {
  		  //unsigned int i = floor((num_slices+1)/2);
		  slat = info.zmin + (i-1/2)*info.baseES; 
		  scr::slice_edges_zplane(slat, &info, &vertices, &edges, &slices);
		  if (slices.size() > 3 ) 
		  {
			  if (output_format == "svg") 
			  {
				wri::write_contours_svg(conf,i, &info, &slices);
			  }
			  if (output_format == "vtk") 
			  {
				wri::write_contours_vtk(inputFile, i, &info, &slices);
			  }
			  //slices.clear();
			  //slices.shrink_to_fit();
			  std::vector<scr::slice>().swap(slices);
		  }
	  }
	}

  
  std::chrono::steady_clock::time_point time_slice = std::chrono::steady_clock::now();
  std::cout << "Slicing time = " << std::chrono::duration_cast<std::chrono::milliseconds>(time_slice - time_parse).count() << "[ms]" << std::endl;
  // write output
  //wri::write_vtk_file(stl_file_name, &info, &vertices, &edges);
  
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "total runtime = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  //std::cout << "duration = " << std::chrono::duration_cast<std::chrono::minutes>(end - begin).count() << "[min]" << std::endl;

}

