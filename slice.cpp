//#include <string>
//#include <vector>
//#include <cmath>
#include <iostream>
#include "slice.h"

namespace scr {
	

void add_sort_info_z(stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges) {
	// add sorting info
	info->nboxxa = info->nboxxo+1;
	info->init_member_pointers();
	//unsigned int edge_checked[info->num_edges];
	//std::cout << "sorting info..." << std::endl;
	for (unsigned int i = 0; i < info->num_edges; i++) {
	  stl::edge cedge = (*edges)[i];
	  cedge.get_boxxnoz(info,vertices);
	  if (info->first_member[cedge.boxno]==-1) {
		  info->first_member[cedge.boxno]= i;
		  info->last_member[cedge.boxno]= i;
	  } else {
		  int lmi = info->last_member[cedge.boxno];
		  (*edges)[lmi].next_member = i; 
		  info->last_member[cedge.boxno] = i;
	  }
	}
}

void slice_edges_zplane(float slat, stl::stl_data *info, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges, std::vector<scr::slice> *slices) {
		
	// create variable to check if edge has been processed
	unsigned int *edge_checked;
	edge_checked  = new unsigned int[info->num_edges]; 
	for (unsigned int i = 0; i < info->num_edges; i++) {
	  edge_checked[i]=0; 
	 }
	

	int slat_boxno = int(fmin(info->nboxxo-1,floor((slat - info->zmin)/info->boxxdimz)));
	int i = info->first_member[slat_boxno];
	unsigned checked_extraboxx = 0;
	int ledgep = -1; //last edge part (one vertex on last edge which is a neighbour to the current edge)
	int start_chain_edgei = -1;
	int start_chain_slicei = -1;
	if (i == -1) {
		i = info->first_member[info->nboxxo];
		checked_extraboxx = 1;
	}
	// search connections
	stl::point p1, p2, p3, p4;
	stl::point n1, n2;
	unsigned int splane=2; // z-plane
	unsigned int npoic= 0; // number of intersections or number of points on contour
	unsigned int npocp= 0; // number of contours on plane
	while (i < info->num_edges && i != -1) { 
		stl::edge cedge = (*edges)[i];
		stl::vertex v1 = (*vertices)[cedge.evpi1];
		stl::vertex v2 = (*vertices)[cedge.evpi2];
		stl::vertex v3 = (*vertices)[cedge.enpi1];
		stl::vertex v4 = (*vertices)[cedge.enpi2];
		if (edge_checked[i] == 0){
			edge_checked[i] = 1;
			p1 = v1.vp;
			p2 = v2.vp;
			p3 = v3.vp;
			p4 = v4.vp;
			n1 = v1.normal;
			n2 = v2.normal;
			if ( (slat < p1.z && slat < p2.z) || (slat > p1.z && slat > p2.z) ) {
				i = cedge.next_member;
				ledgep = -1;
			} else {
				stl::point newp;
				stl::point newn;
				if (slat == p1.z && slat == p2.z) { 
					//std::cout << "just checking: if " << npocp <<std::endl;
					if (slat != p3.z || slat != p4.z || ledgep != -1) { 
						if ( ledgep == -1 ) {
							// start chain
							int edge_checked_expanded=0;
							if (npocp > 0){ // make sure none of the neighbouring edges have been processed
								unsigned int ne[4];
								ne[0] =  get_edge_index(cedge.evpi1, cedge.enpi1, vertices, edges);
								ne[1] =  get_edge_index(cedge.evpi1, cedge.enpi2, vertices, edges);
								ne[2] =  get_edge_index(cedge.evpi2, cedge.enpi1, vertices, edges);
								ne[3] =  get_edge_index(cedge.evpi2, cedge.enpi2, vertices, edges);
								for ( unsigned int k = 0; k < 4; k++ ) {
									edge_checked_expanded += edge_checked[ne[k]]; 
								} 
							}
							
							if (edge_checked_expanded == 0 ) {
								if (p3.z != slat && p4.z == slat ){
									ledgep = cedge.enpi2;
								} else if (p3.z == slat && p4.z != slat ){
									ledgep = cedge.enpi1;
								} 
								start_chain_edgei = i; 
								start_chain_slicei = npoic;
								slices->push_back(slice(n1,p1));
								npoic++;
								//slices->push_back(slice(n2,p2));
								//npoic++;
								get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
							} else {
							edge_checked[i] = 1;
							i = cedge.next_member; 
							}
						} else {
							float qsidel1 = fabs( (*slices)[npoic-1].vp.x - p1.x) + fabs( (*slices)[npoic-1].vp.y - p1.y) + fabs( (*slices)[npoic-1].vp.z - p1.z);
							float qsidel2 = fabs( (*slices)[npoic-1].vp.x - p2.x) + fabs( (*slices)[npoic-1].vp.y - p2.y) + fabs( (*slices)[npoic-1].vp.z - p2.z);
							if (qsidel1 == 0.0 && qsidel2 != 0.0) {
								slices->push_back(slice(n2,p2));
								npoic++; 
								get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
							} else if (qsidel1 != 0.0 && qsidel2 == 0.0) {
								slices->push_back(slice(n1,p1));
								npoic++; 
								get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
							} else {
								std::cout << " slice.cpp: 1: you shoudn't see this " << std::endl;
							}
						}
					// std::cout << npoic << " " << i << std::endl;
					} 
				} else if (slat == p1.z && slat != p2.z ) {
					if (ledgep != -1) {
						float qsidel = fabs( (*slices)[npoic-1].vp.x - p1.x) + fabs( (*slices)[npoic-1].vp.y - p1.y) + fabs( (*slices)[npoic-1].vp.z - p1.z);
						if (qsidel != 0.0 ) {
							slices->push_back(slice(n1,p1));
							npoic++;
						}
						get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
					} else {
						edge_checked[i] = 0;
						i = cedge.next_member; 
					}
				} else if (slat != p1.z && slat == p2.z) {
					if (ledgep != -1) {
						float qsidel = fabs( (*slices)[npoic-1].vp.x - p2.x) + fabs( (*slices)[npoic-1].vp.y - p2.y) + fabs( (*slices)[npoic-1].vp.z - p2.z);
						if (qsidel != 0.0 ) {
							slices->push_back(slice(n2,p2));
							npoic++;
						}
						get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
					} else {
						edge_checked[i] = 0;
						i = cedge.next_member; 
					}
				}  else {
					if (ledgep == -1) {
						// start chain
						start_chain_edgei = i; 
						start_chain_slicei = npoic;
					}
					float r = (slat - p1.z) / (p2.z - p1.z);
					newp.z = slat;
					newp.y = p1.y + r * (p2.y - p1.y);
					newp.x = p1.x + r * (p2.x - p1.x);
					newn.x = n1.x + r * (n2.x - n1.x);
					newn.y = n1.y + r * (n2.y - n1.y);
					newn.z = n1.z + r * (n2.z - n1.z);
					slices->push_back(slice(newn,newp));
					npoic++;
					get_index_of_next_edge(slat, &i, &ledgep, cedge, vertices, edges, splane);
				}
			}
		} else { // edge already checked; move on to next in bucket
			i = cedge.next_member; 
			ledgep = -1;
			if (start_chain_edgei != -1) { // go back to index which initiated chain reaction
				i = start_chain_edgei;
				start_chain_edgei = -1;
				//std::cout << "number of points on contour " << npoic <<std::endl;
				//if (npoic > 2) {
				stl::point p = (*slices)[start_chain_slicei].vp;
				stl::point n = (*slices)[start_chain_slicei].normal;
				slices->push_back(slice(n,p));
				ledgep = -1;
				npocp++;
				npoic++;
				//break;
			} 
		}
		if ( i == -1 && checked_extraboxx == 0) { // moved this {} up one level --- didn' test it though
			i = info->first_member[info->nboxxo];
			checked_extraboxx = 1;
			start_chain_edgei = -1;
			ledgep = -1;
		}
	}
	delete[] edge_checked;
}


void get_index_of_next_edge(float slat, int *i, int *ledgep, stl::edge cedge, std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges, unsigned int splane) {
	stl::vertex v1 = (*vertices)[cedge.evpi1];
	stl::vertex v2 = (*vertices)[cedge.evpi2];
	stl::vertex v3 = (*vertices)[cedge.enpi1];
	//stl::point p1, p2, p3;
	int j;
	int match_index;
	float p1xyz, p2xyz, p3xyz;
	if (splane == 0) { // set these also for other axis
		p1xyz = v1.vp.x;
		p2xyz = v2.vp.x;
		p3xyz = v3.vp.x;
	} else if (splane == 1 ) { // set these also for other axis
		p1xyz = v1.vp.y;
		p2xyz = v2.vp.y;
		p3xyz = v3.vp.y;
	} else if (splane == 2 ) { // set these also for other axis
		p1xyz = v1.vp.z;
		p2xyz = v2.vp.z;
		p3xyz = v3.vp.z;
	}
	if ( ledgep[0] == -1 || ledgep[0] == cedge.enpi2 ) {
		 if ( (slat < p1xyz && slat >= p3xyz) || (slat <= p3xyz && slat > p1xyz) ) {
			ledgep[0] = cedge.evpi2;
			if (cedge.enpi1 > cedge.evpi1) {
				j = v3.first_edge;
				match_index = cedge.evpi1;
			} else {
				j = v1.first_edge;
				match_index = cedge.enpi1;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( (slat < p2xyz && slat >= p3xyz) || (slat <= p3xyz && slat > p2xyz) )  { // "if" not really necessary) 
			ledgep[0] = cedge.evpi1;
			if (cedge.enpi1 > cedge.evpi2) {
				j = v3.first_edge;
				match_index = cedge.evpi2;
			} else {
				j = v2.first_edge;
				match_index = cedge.enpi1;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( slat == p1xyz ) {
			ledgep[0] = cedge.evpi2;
			if (cedge.enpi1 > cedge.evpi1) {
				j = v3.first_edge;
				match_index = cedge.evpi1;
			} else {
				j = v1.first_edge;
				match_index = cedge.enpi1;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( slat == p2xyz )  { 
			ledgep[0] = cedge.evpi1;
			if (cedge.enpi1 > cedge.evpi2) {
				j = v3.first_edge;
				match_index = cedge.evpi2;
			} else {
				j = v2.first_edge;
				match_index = cedge.enpi1;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else {
			std::cout << " ledgep " << ledgep[0] << " enp1 " << cedge.enpi1 << " enp2 " << cedge.enpi2 << std::endl;
			std::cout << " slice.cpp: 2: you shouldn't see this " << std::endl;
			i[0]=-1;
		}
	} else if (ledgep[0] == cedge.enpi1 ) {
		v3 = (*vertices)[cedge.enpi2];
		if (splane == 0) { // set these also for other axis
			p3xyz = v3.vp.x;
		} else if (splane == 1) { // set these also for other axis
			p3xyz = v3.vp.y;
		} else if (splane == 2) { // set these also for other axis
			p3xyz = v3.vp.z;
		}
		if ( (slat < p1xyz && slat >= p3xyz) || (slat <= p3xyz && slat > p1xyz) ) {
			ledgep[0] = cedge.evpi2;
			if (cedge.enpi2 > cedge.evpi1) {
				j = v3.first_edge;
				match_index = cedge.evpi1;
			} else {
				j = v1.first_edge;
				match_index = cedge.enpi2;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( (slat < p2xyz && slat >= p3xyz) || (slat <= p3xyz && slat > p2xyz) )  { // "if" not really necessary) 
			ledgep[0] = cedge.evpi1;
			if (cedge.enpi2 > cedge.evpi2) {
				j = v3.first_edge;
				match_index = cedge.evpi2;
			} else {
				j = v2.first_edge;
				match_index = cedge.enpi2;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( slat == p1xyz ) {
			ledgep[0] = cedge.evpi2;
			if (cedge.enpi2 > cedge.evpi1) {
				j = v3.first_edge;
				match_index = cedge.evpi1;
			} else {
				j = v1.first_edge;
				match_index = cedge.enpi2;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else if ( slat == p2xyz )  {
			ledgep[0] = cedge.evpi1;
			if (cedge.enpi2 > cedge.evpi2) {
				j = v3.first_edge;
				match_index = cedge.evpi2;
			} else {
				j = v2.first_edge;
				match_index = cedge.enpi2;
			}
			while (j != -1 ) {
				if ( (*edges)[j].evpi2 == match_index ) {
					i[0] = j; 
					j = -1;
				} else {
					j = (*edges)[j].next_edge;
				}
			}
		} else {
			std::cout << " ledgep " << ledgep[0] << " enp1 " << cedge.enpi1 << " enp2 " << cedge.enpi2 << std::endl;
			std::cout << " slice.cpp: 3: you shouldn't see this " << std::endl;
			i[0]=-1;
		}
	} else {
		std::cout << " ledgep " << ledgep[0] << " enp1 " << cedge.enpi1 << " enp2 " << cedge.enpi2 << std::endl;
		std::cout << " slice.cpp: 4: you shouldn't see this " << std::endl;
		i[0]=-1;
	}	
}

  int get_edge_index(unsigned int vertex1, unsigned int vertex2,std::vector<stl::vertex> *vertices, std::vector<stl::edge> *edges) {
		unsigned int ri;
		int v1, v2; // e1 > e2
		if (vertex1 > vertex2) {
			v1 = vertex1;
			v2 = vertex2;
		} else {
			v1 = vertex2;
			v2 = vertex1;
		}
		int i = (*vertices)[v1].first_edge;
		while (i != -1 ) {
			if ( (*edges)[i].evpi2 == v2 ) {
				ri = i;
				i=-1;
			} else {
				i = (*edges)[i].next_edge;
			}
		}
	  return ri;
  }

}
