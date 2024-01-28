#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include "omp.h"

#include "parse_stl.h"

namespace stl {

  std::ostream& operator<<(std::ostream& out, const point p) {
    out << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;
    return out;
  }

  std::ostream& operator<<(std::ostream& out, const vertex& t) {
    out << "---- TRIANGLE ----" << std::endl;
    out << t.normal << std::endl;
    out << t.vp << std::endl;
    //out << t.v2 << std::endl;
    //out << t.v3 << std::endl;
    return out;
  }
  
  float parse_float(std::ifstream& s) {
    char f_buf[sizeof(float)];
    s.read(f_buf, 4);
    float* fptr = (float*) f_buf;
    return *fptr;
  }

  point parse_point(std::ifstream& s) {
    float x = parse_float(s);
    float y = parse_float(s);
    float z = parse_float(s);
    return point(x, y, z);
  }
  
  void clean_vertices(stl_data *info, std::vector<vertex> *vertices, std::vector<edge> *edges, triangle tri) {
	 // check if any two points of the triangle are same
	 for (unsigned int i = 0; i < 3; i++) {
		 float qsidel = fabs(tri.v[i].x - tri.v[ (i+1)%3 ].x) + fabs(tri.v[i].y - tri.v[ (i+1)%3 ].y) + fabs(tri.v[i].z - tri.v[ (i+1)%3 ].z); 
		 if (qsidel == 0.0) { return; } }
	 // check for vertex recurrency if triangle has 3 individual points
	 tri.get_boxno(info);
	 int matched  = 0;  
	 // parallelization overhead seems to be more than the gain
	 //#pragma omp parallel //num_threads(3)
	 //{
	 //#pragma omp for nowait reduction(+:matched)
	 //#pragma omp parallel for reduction(+:matched)
	 for (unsigned int i = 0; i < 3; i++) {
	 	tri.matched[i]= 0;
	 	//tri.index[i]= -1;
		int j = info->first_member[tri.boxno[i]];
		while (j < info->num_vertices && j != -1) {
			vertex ve = (*vertices)[j]; 
			float qsidel = fabs(tri.v[i].x - ve.vp.x) + fabs(tri.v[i].y - ve.vp.y) + fabs(tri.v[i].z - ve.vp.z);
			if (qsidel == 0.0) { 
				tri.index[i] = j; 
				tri.matched[i] = 1; 
				matched++;
				j=-1; 
			} else {
			j = ve.next_member; 
				} 
			}
	 	} 
	 //}
	 // add vertices to list
	 for (unsigned int i = 0; i < 3; i++) {
		if (tri.matched[i] == 0) { 
			// add sorting information
			if ( info->first_member[tri.boxno[i]]  == -1) {
				info->first_member[tri.boxno[i]] = info->num_vertices;
				info->last_member[tri.boxno[i]] = info->num_vertices;
			} else {
				int lmi = info->last_member[tri.boxno[i]];
				(*vertices)[lmi].next_member = info->num_vertices; 
				info->last_member[tri.boxno[i]] = info->num_vertices;
			}
			// add vertex 
			vertices->push_back(vertex(tri.normal,tri.v[i],-1,-1,-1));
			tri.index[i] = info->num_vertices;
			info->num_vertices += 1; 
		}
		else { // adjust normal 
			vertex ve = (*vertices)[tri.index[i]];
			point new_direction;
			new_direction.x = tri.normal.x + ve.normal.x;
			new_direction.y = tri.normal.y + ve.normal.y;
			new_direction.z = tri.normal.z + ve.normal.z;
			float new_dir_len = sqrt ( pow(new_direction.x,2) + pow(new_direction.y,2) + pow(new_direction.z,2) );
			(*vertices)[tri.index[i]].normal.x = new_direction.x / new_dir_len;
			(*vertices)[tri.index[i]].normal.y = new_direction.y / new_dir_len;
			(*vertices)[tri.index[i]].normal.z = new_direction.z / new_dir_len;
		}
	}
		
	// add edges to list
	for (int i = 2; i > -1; i--) {
			unsigned int j = (i +1) % 3;
			unsigned int k = (i +1 +1) % 3;
			unsigned int li = std::max(tri.index[j],tri.index[k]);
			unsigned int lr = std::min(tri.index[j],tri.index[k]);
			unsigned int connfound = 0;
			if ( (matched == 2 ) || matched == 3) { 
				int si = (*vertices)[li].first_edge;
				int ei = (*vertices)[li].last_edge+1;
				int j = si;
				while (j<ei && j != -1 && (*edges)[j].evpi1 == li) { // check if edge exists
					if ((*edges)[j].evpi2 == lr) { //connection already exists - write other triangle point
						connfound = 1;
						if ( (*edges)[j].enpi2 != -1) { 
							std::cout << "ERROR: overwriting existing triangle point!!! " << matched << std::endl; 
							exit (EXIT_FAILURE);
							}   
						if ((*edges)[j].enpi1 != tri.index[i] && li == (*edges)[j].evpi1 ) {
							(*edges)[j].enpi2 = tri.index[i];
							}
						j = -1;
					
					} else { 
						//j++;
						int jp = j; 
						j = (*edges)[jp].next_edge; 
					}
					
				} 
				if (connfound == 0) { // write new edge
					if ( (*vertices)[li].first_edge == -1 ) {
					(*vertices)[li].first_edge = info->num_edges;
					(*vertices)[li].last_edge = info->num_edges;
					} else { 
					int previous_edge = (*vertices)[li].last_edge;
					(*edges)[previous_edge].next_edge = info->num_edges;;
					(*vertices)[li].last_edge = info->num_edges;
					}
					edges->push_back(edge(li,lr,tri.index[i],-1,-1,-1));
					info->num_edges += 1; 
				}
			} else { // write new edge
				if ( (*vertices)[li].first_edge == -1 ) {
					(*vertices)[li].first_edge = info->num_edges;
					(*vertices)[li].last_edge = info->num_edges;
				} else { 
					int previous_edge = (*vertices)[li].last_edge;
					(*edges)[previous_edge].next_edge = info->num_edges;
					(*vertices)[li].last_edge = info->num_edges;
					}
				edges->push_back(edge(li,lr,tri.index[i],-1,-1,-1));
				info->num_edges += 1; 
			 	}
			}
		}
  
  

  void parse_binary_stl(const std::string& stl_path, stl_data *info, std::vector<vertex> *vertices,  std::vector<edge> *edges, unsigned int cid) {
	if (cid == 0) { 
		std::cout << "read binary stl..." << std::endl;
	}// else if (cid == 1) {
	//	std::cout << "sorting vertices..." << std::endl;
	//}
    std::ifstream stl_file(stl_path.c_str(), std::ios::in | std::ios::binary);

    char header_info[80] = "";
    char n_triangles[4];
    stl_file.read(header_info, 80);
    stl_file.read(n_triangles, 4);
    std::string h(header_info);
    info->name = header_info;
    unsigned int* r = (unsigned int*) n_triangles;
    info->num_triangles = *r;
    info->num_vertices = 0;
    info->num_edges = 0;
    // get bounding box
    if (cid == 0) {
	      float xmin,xmax,ymin,ymax,zmin,zmax;
    	  triangle tri;
	      tri.normal = parse_point(stl_file);
		  tri.v[0] = parse_point(stl_file);
		  tri.v[1] = parse_point(stl_file);
		  tri.v[2] = parse_point(stl_file);
		  stl_file.read(tri.attr, 2);
		  tri.get_tri_bbox();
		  xmin = tri.xmin;
		  ymin = tri.ymin;
		  zmin = tri.zmin;
		  xmax = tri.xmax;
		  ymax = tri.ymax;
		  zmax = tri.zmax; 	
		for (unsigned int i = 1; i < info->num_triangles; i++) {
		  tri.normal = parse_point(stl_file);
		  tri.v[0] = parse_point(stl_file);
		  tri.v[1] = parse_point(stl_file);
		  tri.v[2] = parse_point(stl_file);
		  stl_file.read(tri.attr, 2);
		  tri.get_tri_bbox();
		  xmin = fmin(xmin,tri.xmin);
		  ymin = fmin(ymin,tri.ymin);
		  zmin = fmin(zmin,tri.zmin);
		  xmax = fmax(xmax,tri.xmax);
		  ymax = fmax(ymax,tri.ymax);
		  zmax = fmax(zmax,tri.zmax);
		  }
		info->xmin = xmin;
		info->xmax = xmax;
		info->ymin = ymin;  
		info->ymax = ymax;
		info->zmin = zmin;
		info->zmax = zmax; 
    } else if (cid == 1) {
	    for (unsigned int i = 0; i < info->num_triangles; i++) {
		  unsigned int numTridev10 = ceil(info->num_triangles/10);
		  if (i % numTridev10 == 0) 
		  {	
			  //std::cout << " Preprocessing... " << i << " of " << info->num_triangles << " triangles! continuing..." << std::endl;
			  int progress = (float)(i)/info->num_triangles*100;
			  std::cout << " Preprocessing... " << progress << " % complete" << std::endl;
		  }
    	  triangle tri;
		  tri.normal = parse_point(stl_file);
		  tri.v[0] = parse_point(stl_file);
		  tri.v[1] = parse_point(stl_file);
		  tri.v[2] = parse_point(stl_file);
		  stl_file.read(tri.attr, 2);
		  clean_vertices(info, vertices, edges, tri); 
		  }
	std::cout << " Preprocessing... done!" << std::endl;
  	info->free_members();
	}
  }
  
  // parse_ascii_stl is incomplete
  void parse_ascii_stl(const std::string& stl_path, stl_data *info, std::vector<vertex> *vertices,  std::vector<edge> *edges, unsigned int cid) {
      std::cout << "read ascii stl... this routine isn't ready yet" << std::endl;
	  std::ifstream stl_file(stl_path.c_str());
	  std::string str;
	  std::string key = "normal";
	  while (stl_file) {
		std::getline(stl_file, str);
		if (str.find(key) != std::string::npos) {
			// normals
      		//std::cout << str << std::endl;
			std::getline(stl_file, str);
			std::getline(stl_file, str);
			// vertex 1
      		//std::cout << str << std::endl;
			std::getline(stl_file, str);
			// vertex 2
      		//std::cout << str << std::endl;
			std::getline(stl_file, str);
			// vertex 3
      		//std::cout << str << std::endl;
	  		}
		}
	}
  
  bool is_binary_stl(const std::string& stl_path) {
	std::ifstream stl_file(stl_path.c_str(), std::ios::in | std::ios::binary);
    if (!stl_file) {
      std::cout << "ERROR: COULD NOT READ FILE" << std::endl;
      assert(false);
    }
    
    bool bbinary = true;
	std::string key = "solid";
	std::string word = "facet";

	// get header chunk
    char header_info[80] = "";
    stl_file.read(header_info, 80);
    std::string h(header_info);
    
    // check for key word :) solid and facet
    // if found file is ASCII
    if (h.find(key) != std::string::npos) {
		if (h.find(word) != std::string::npos) {
    		//std::cout << "Looks like file is ASCII!" << std::endl;
    		bbinary = false;
		}
	}
	
    return bbinary;
    
    }

}
