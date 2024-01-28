#ifndef WRITE_FILES_H
#define WRITE_FILES_H

#include "parse_stl.h"
#include "slice.h"
#include "hpse.h"

namespace wri {
  
void write_vtk_file(const std::string& stl_path, stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges);

void write_contours_vtk(const std::string& stl_path, unsigned int sno, stl::stl_data *info, std::vector<scr::slice> *slices);

//void write_contours_svg(const std::string& stl_path, unsigned int sno, stl::stl_data *info, std::vector<scr::slice> *slices);
void write_contours_svg(const Configuration& conf, unsigned int sno, stl::stl_data *info, std::vector<scr::slice> *slices);

}

#endif
