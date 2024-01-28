//#include <vector>
//#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>

#include "write_files.h"
//#include "lodepng.h"



namespace wri {
	
void write_vtk_file(const std::string& stl_path, stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges) 
	{
		std::string vtk_file_name = stl_path.c_str();
		unsigned int name_length = vtk_file_name.length();
		vtk_file_name.replace(name_length-4, name_length, ".vtk"); 
		// create and open vtk text file
		std::ofstream vtk_file(vtk_file_name);
		std::cout << "writing vtk output to: " << vtk_file_name << std::endl;
		// write header
		vtk_file << "# vtk DataFile Version 4.2" << std::endl;
		vtk_file << "surface" << std::endl;
		vtk_file << "ASCII" << std::endl;
		vtk_file << "DATASET POLYDATA" << std::endl;
		vtk_file << "POINTS " << info->num_vertices << " float" << std::endl;
		for (unsigned int i = 0; i < info->num_vertices; i++) {
			stl::vertex ve = (*vertices)[i];
			// vtk_file << ve.vp.x << "\t" << ve.vp.y << "\t" << ve.vp.z << std::endl; }
		        // check normals
			vtk_file << ve.vp.x - ve.normal.x << "\t" << ve.vp.y - ve.normal.y << "\t" << ve.vp.z - ve.normal.z<< std::endl; }
		// write edges 
		vtk_file << "POLYGONS " << info->num_edges << " " << info->num_edges*3 << std::endl;
		for (unsigned int i = 0; i < info->num_edges; i++) {
			stl::edge ed = (*edges)[i];
			vtk_file << "2\t" << ed.evpi1 << "\t" << ed.evpi2 << std::endl; } 
			//vtk_file << "2\t" << ed.evpi1 << "\t" << ed.evpi2 << "\t" << ed.enpi1 << "\t" << ed.enpi2 << std::endl; } 
		// write triangles
		/*vtk_file << "POLYGONS " << info->num_edges << " " << info->num_edges*4 << std::endl;
		for (unsigned int i = 0; i < info->num_edges; i++) {
			stl::edge ed = (*edges)[i];
			vtk_file << "3\t" << ed.edgei1 << "\t" << ed.edgei2 << "\t" << ed.neghi1 << std::endl; }*/
		// close vtk file
		vtk_file.close();
	}
	

	void write_contours_vtk(const std::string& stl_path, unsigned int sno, stl::stl_data *info, std::vector<scr::slice> *slices) 
	{
		std::string vtk_file_name = stl_path.c_str();
		std::string name_append = "_contour";
		name_append += std::to_string(sno); 
		name_append += ".vtk"; 
		unsigned int name_length = vtk_file_name.length();
		vtk_file_name.replace(name_length-4, name_length, name_append); 
		// create and open vtk text file
		std::ofstream vtk_file(vtk_file_name);
		//std::cout << "writing vtk output to: " << vtk_file_name << std::endl;
		// write header
		vtk_file << "# vtk DataFile Version 4.2" << std::endl;
		vtk_file << "surface" << std::endl;
		vtk_file << "ASCII" << std::endl;
		vtk_file << "DATASET POLYDATA" << std::endl;
		vtk_file << "POINTS " << slices->size() << " float" << std::endl;
		for (unsigned int i = 0; i < slices->size(); i++) {
			scr::slice sc = (*slices)[i];
			vtk_file << sc.vp.x << "\t" << sc.vp.y << "\t" << sc.vp.z << std::endl; }
		vtk_file.close();
	}
	
	void write_contours_svg(const Configuration& conf, unsigned int sno, stl::stl_data *info, std::vector<scr::slice> *slices) 
	{
		//std::string svg_file_name = stl_path.c_str();
		//std::__fs::filesystem::create_directory("./_test_directory");
		
		std::size_t posLastDot = conf.outputName.find_last_of(".");
		std::string svg_file_name = conf.outputLocation + conf.outputName.substr(0,posLastDot) + "_Speedy_" + std::to_string(sno) + ".svg";
		/*std::string name_append = "_contour";
		std::__fs::filesystem::create_directory(name_append);
		name_append += std::to_string(sno); 
		name_append += ".svg"; 
		unsigned int name_length = svg_file_name.length();
		svg_file_name.replace(name_length-4, name_length, name_append); */
		// create and open vtk text file
		std::ofstream svg_file(svg_file_name);
		// initialize strings for contours
		std::string contours, ccontour;
		unsigned int ncontour = 0;
		std::vector<unsigned int> contourStringLength;
		std::vector<unsigned int>::iterator iti;
		std::vector<float> contourMinMax;
		std::vector<float>::iterator itf;
		// write header
		int image_width = conf.imageXDim, image_height = conf.imageYDim;
		int image_width_halve = image_width / 2, image_height_halve = image_height / 2;
		float width_xdim_halve = conf.printAreaXDim/2, height_ydim_halve = conf.printAreaYDim/2;
		svg_file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
		svg_file << "<svg style=\"background-color:black\"  width=\"" << image_width << "\" height=\"" << image_height 
			<< "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
		//svg_file << "<polyline points=\"" ;
		ccontour.append("<polyline points=\"");
		scr::slice sc0 = (*slices)[0];
		float xmin_x = sc0.vp.x;
		float xmin_y = sc0.vp.y;
		float xmax_x = sc0.vp.x;
		int xmin_norm_x = 0;
		if ( sc0.normal.x > 0 )
		  { xmin_norm_x = 1; }
		else if ( sc0.normal.x < 0 )
		  { xmin_norm_x = -1; }
		int patch_begin = 0;
		for (unsigned int i = 0; i < slices->size(); i++) 
		{
			scr::slice sc = (*slices)[i];
			if (sc.vp.x < xmin_x)
			{
			  xmin_x = sc.vp.x;
			  xmin_y = sc.vp.y;
			  xmin_norm_x = 0;
			  if ( sc.normal.x > 0 )
			    { xmin_norm_x = 1; }
			  else if ( sc.normal.x < 0 )
			    { xmin_norm_x = -1; }
			}
			if (sc.vp.x > xmax_x)
			{
			  xmax_x = sc.vp.x;
			}
			ccontour.append(std::to_string(int(round( image_width_halve + sc.vp.x / width_xdim_halve * image_width_halve )))); //make integer before string !!!!
			ccontour.append( ","); 
			ccontour.append(std::to_string(int(round( image_height_halve   - sc.vp.y / height_ydim_halve * image_height_halve )))); //make integer before string !!!!
			ccontour.append(" "); 
			if ( i != patch_begin  && sc.vp.x == sc0.vp.x && sc.vp.y == sc0.vp.y ) // || i == slices->size()-1 //&& i != slices->size()-1
			{
				//svg_file << "\" stroke=\"white\" stroke-width=\"0\" " ;
				ccontour.append("\" stroke=\"white\" stroke-width=\"0\" " );
				if (xmin_norm_x < 0)
				  { ccontour.append("fill=\"white\" /> \n"); }
				else if (xmin_norm_x > 0)
				  { ccontour.append( "fill=\"black\" /> \n"); }
				else 
				  {
				    ccontour.append("fill=\"white\" /> \n"); 
				    std::cout << "WARNING: Normal direction is not clear! Assuming fill! Add y-direction check!!" << std::endl;
				  }
				// simply add current contour to contours
				if (ncontour == 0) 
				{
					contours.append(ccontour);
					contourStringLength.push_back(ccontour.length());
					contourMinMax.push_back(xmin_x);
					contourMinMax.push_back(xmax_x);
				}
				// find correct location of contour
				else
				{
					unsigned int writeLoc = ncontour;
					for ( int nc = ncontour-1; nc >= 0; nc--) 
					{
						if (xmin_x < contourMinMax[nc*2] && xmax_x > contourMinMax[nc*2+1])
						{ 
							writeLoc = nc;
						}
					}
					unsigned int insertLoc = 0;
					//writeLoc = 0;
					for (unsigned int j = 0; j < writeLoc; j++) 
					{ 
						insertLoc += contourStringLength[j];
					}
					contours.insert(insertLoc, ccontour);
					iti = contourStringLength.begin()+writeLoc;
					unsigned int cconLen= ccontour.length();
					contourStringLength.insert(iti,cconLen);
					itf = contourMinMax.begin()+(writeLoc)*2;
					contourMinMax.insert(itf,xmin_x);
					itf = contourMinMax.begin()+(writeLoc)*2+1;
					contourMinMax.insert(itf,xmax_x);
					
				}
				ncontour += 1;
				ccontour = "<polyline points=\"" ;
				patch_begin = i +1;
				sc0 = (*slices)[patch_begin];
		 		xmin_x = sc0.vp.x;
				xmin_y = sc0.vp.y;
		 		xmax_x = sc0.vp.x;
			}
			
		}
		svg_file << contours;
		svg_file << "</svg>" << std::endl;
		svg_file.close();
	}


	
}

