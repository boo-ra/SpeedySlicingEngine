#ifndef SLICE_H
#define SLICE_H

#include "parse_stl.h"

namespace scr {
	
	struct slice {
		
		stl::point normal;
		stl::point vp;
		
		slice(stl::point normalp, stl::point vpc) :
		normal(normalp), vp(vpc) {}
		
	};
	
	struct sliced {
		
		float x, y, z, nx, ny, nz; 
		
		void fee_mem_slice() 
		{
			delete[] &x;
			delete[] &y;
			delete[] &z;
			delete[] &nx;
			delete[] &ny;
			delete[] &nz;
		}
		
		sliced(float xa, float ya, float za, float nxa, float nya, float nza) :
		x(xa), y(ya), z(za), nx(nxa), ny(nya), nz(nza) {}

	};
	
	struct slice_info {
		
		unsigned int num_vertices;
		unsigned int num_contours;
		unsigned int si, ei;
		
	};
	
	
	void add_sort_info_z(stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges);
	
	void slice_edges_zplane(float slat, stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges, std::vector<scr::slice> *slices);
	
	void get_index_of_next_edge(float slat, int *i, int *ledgep, stl::edge cedge, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges, unsigned int splane);
	
	int get_edge_index(unsigned int vertex1, unsigned int vertex2,std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges);
}

#endif
