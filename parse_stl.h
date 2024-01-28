#ifndef PARSE_STL_H
#define PARSE_STL_H

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace stl {

  struct point {
    float x;
    float y;
    float z;

    point() : x(0), y(0), z(0) {}
    point(float xp, float yp, float zp) : x(xp), y(yp), z(zp) {}
  };
  
  struct stl_data {
    std::string name;
    unsigned int num_triangles;
    unsigned int num_vertices;
    unsigned int num_edges;
    float xmin,xmax,ymin,ymax,zmin,zmax;
    unsigned int nboxxo, nboxxa;  
	float boxxdimx, boxxdimy, boxxdimz;
	float baseES;
	int *first_member, *last_member;
    void init_member_pointers() {
	    //nboxxa = nboxxo * nboxxo * nboxxo;
	    first_member  = new int[nboxxa]; 
	    last_member  = new int[nboxxa]; 
	    for (unsigned int i = 0; i < nboxxa; i++) {
		    first_member[i] = -1;
		    last_member[i]  = -1;
		    }
		}
	void get_boxxdim() {
		boxxdimx = (xmax - xmin) / nboxxo;
		boxxdimy = (ymax - ymin) / nboxxo;
		boxxdimz = (zmax - zmin) / nboxxo;
		}
		
	void free_members() {
		delete[] last_member;
		delete[] first_member;
	}
  };
  
  
    class triangle {
	public:
		point normal;
		point v[3];
		unsigned int matched[3];
		unsigned int index[3];
		char attr[2]; 
	    float xmin,xmax,ymin,ymax,zmin,zmax;
		void get_tri_bbox() {
		  xmin = fmin(fmin(v[0].x, v[1].x),v[2].x);
		  xmax = fmax(fmax(v[0].x, v[1].x),v[2].x);
		  ymin = fmin(fmin(v[0].y, v[1].y),v[2].y);
		  ymax = fmax(fmax(v[0].y, v[1].y),v[2].y);
		  zmin = fmin(fmin(v[0].z, v[1].z),v[2].z);
		  zmax = fmax(fmax(v[0].z, v[1].z),v[2].z);
		  }
		unsigned int boxno[3];
	    void get_boxno(stl_data *info) {
	    for (unsigned int i=0; i < 3; i++) {
		    boxno[i] =  int(fmin(info->nboxxo-1,floor((v[i].x - info->xmin)/info->boxxdimx))) + 
	      		  		int(fmin(info->nboxxo-1,floor((v[i].y - info->ymin)/info->boxxdimy)))*info->nboxxo + 
	      		 		int(fmin(info->nboxxo-1,floor((v[i].z - info->zmin)/info->boxxdimz)))*info->nboxxo*info->nboxxo;    
	      	}
	    }
  };
  
    
  struct vertex {
    point normal;
    point vp;
    int next_member;
    int first_edge = -1;
    int last_edge= -1; // last_edge is probably useless
    
    vertex(point normalp, point vpc, int nm, int fe, int le):
    normal(normalp), vp(vpc), next_member(nm), first_edge(fe), last_edge(le) {}
    
  };
  
  std::ostream& operator<<(std::ostream& out, const vertex& t);

  
    struct edge {
	  unsigned int evpi1, evpi2;
	  int enpi1, enpi2;
	  int next_edge; //of point
	  int next_member; // in bucket
	  unsigned int boxno;
	  
	  void get_boxxnox(stl_data *info, std::vector<stl::vertex> *vertices) {
		  unsigned int boxno0 = int(fmin(info->nboxxo-1,floor(((*vertices)[evpi1].vp.x - info->xmin)/info->boxxdimx)));
		  unsigned int boxno1 = int(fmin(info->nboxxo-1,floor(((*vertices)[evpi2].vp.x - info->xmin)/info->boxxdimx))); 
		  if (boxno0 == boxno1) {
			  boxno = boxno0;
		  } else {
			  boxno = info->nboxxo;
		  }
	  }
	  
	  void get_boxxnoz(stl_data *info, std::vector<stl::vertex> *vertices) {
		  unsigned int boxno0 = int(fmin(info->nboxxo-1,floor(((*vertices)[evpi1].vp.z - info->zmin)/info->boxxdimz)));
		  unsigned int boxno1 = int(fmin(info->nboxxo-1,floor(((*vertices)[evpi2].vp.z - info->zmin)/info->boxxdimz))); 
		  if (boxno0 == boxno1) {
			  boxno = boxno0;
		  } else {
			  boxno = info->nboxxo;
		  }
	  }
	  
	  
	  edge( unsigned int v1, unsigned int v2, int n1, int n2, int ne, int nm):
	  evpi1(v1), evpi2(v2), enpi1(n1), enpi2(n2), next_edge(ne), next_member(nm) {}  
  };
  
  
  
  

  void parse_binary_stl(const std::string& stl_path, stl_data *info, std::vector<vertex> *vertices, std::vector<edge> *edges,  unsigned int cid);
  
  void parse_ascii_stl(const std::string& stl_path, stl_data *info, std::vector<vertex> *vertices, std::vector<edge> *edges, unsigned int cid);
  
  void clean_vertices(stl_data *info, std::vector<vertex> *vertices, std::vector<edge> *edges, triangle tri);
  
  bool is_binary_stl (const std::string& stl_path);
  

}

#endif
