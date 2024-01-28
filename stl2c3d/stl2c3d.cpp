#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <chrono>
#include <cmath>



struct Vertex {
  float x, y, z ;
};

struct Triangle {
  Vertex A, B, C, N; // index 0 is for the normal
};

struct TriangleVertices {
  size_t a, b, c ;
};

struct Connections {
  int q, w, e, r ;
  int n = -1 ; // vertex next Listing
  
};


std::string trim(const std::string& str) {
    const std::string whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


bool isRightHandRule(Triangle& triangle) { // not used
    // Calculate the cross product of two edges of the triangle
    float crossProductX = (triangle.B.x - triangle.A.x) * (triangle.C.y - triangle.A.y) -
                          (triangle.C.x - triangle.A.x) * (triangle.B.y - triangle.A.y);
    float crossProductY = (triangle.B.z - triangle.A.z) * (triangle.C.x - triangle.A.x) -
                          (triangle.C.z - triangle.A.z) * (triangle.B.x - triangle.A.x);
    float crossProductZ = (triangle.B.y - triangle.A.y) * (triangle.C.z - triangle.A.z) -
                          (triangle.C.y - triangle.A.y) * (triangle.B.z - triangle.A.z);

    // Calculate the dot product of the cross product with the normal
    float dotProduct = crossProductX * triangle.N.x + crossProductY * triangle.N.y + crossProductZ * triangle.N.z;

    // Check if the dot product is positive (right-hand rule)
    return dotProduct > 0.0f;
}
  
void processTriangle (Triangle& triangle, std::unordered_map<std::string, size_t>& uniqueVertices, std::vector<Vertex>& uniqueVertexList, std::vector<TriangleVertices>& triangleVerticesList ) {

  size_t currentIndex =  uniqueVertexList.size() ; 
  size_t vertexIndex[3] ;
  TriangleVertices triangleVertices; 
  Vertex vertices[3] ;
  vertices[0] = triangle.A ;
  vertices[1] = triangle.B ;
  vertices[2] = triangle.C ;
  for (uint32_t i=0; i<3; i++ ) {
    std::stringstream ss;
    ss << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z ;
    std::string vertexString = ss.str();
    if (uniqueVertices.find(vertexString) == uniqueVertices.end()) {
      uniqueVertices[vertexString] = currentIndex;
      vertexIndex[i] = currentIndex++ ; 
      uniqueVertexList.push_back(vertices[i]);
    } else {
      vertexIndex[i] =  uniqueVertices[vertexString] ;
    };
  };
  triangleVertices.a = vertexIndex[0] ;
  triangleVertices.b = vertexIndex[1] ;
  triangleVertices.c = vertexIndex[2] ;
  // std::cout << triangleVertices.a << ' ' << triangleVertices.b << ' ' << triangleVertices.c << std::endl ;  
  triangleVerticesList.push_back(triangleVertices) ;
  
};

  
void evaluateConnection ( const Connections& connections, std::vector<Connections>& connectionsList, std::unordered_map<std::string, size_t>& uniqueConnections, std::vector<int>& vertexFirstListing, std::vector<int>& vertexLastListing ) {

  size_t numconns = uniqueConnections.size()  ; 
  std::stringstream ss;
  std::string connectionsString ; 
  ss << connections.q << " " << connections.w ;
  connectionsString = ss.str();
  if (uniqueConnections.find(connectionsString) == uniqueConnections.end()) {
    connectionsList.push_back(connections) ;
    if ( vertexFirstListing[connections.q] == -1 ) {
      vertexFirstListing[connections.q] = numconns ;
      vertexLastListing[connections.q] = numconns ;   
    } else {
      connectionsList[ vertexLastListing[ connections.q] ].n = numconns ;  
      vertexLastListing[connections.q] = numconns ;
    }
    uniqueConnections[connectionsString] = numconns++;
  } else {
    if ( connectionsList[uniqueConnections[connectionsString]].e == -1 and connections.e != -1 ) {
      connectionsList[uniqueConnections[connectionsString]].e = connections.e ;
    } else if ( connectionsList[uniqueConnections[connectionsString]].r == -1 and connections.r != -1 ) { 
      connectionsList[uniqueConnections[connectionsString]].r = connections.r ;	   
    } else {
      if ( connectionsList[uniqueConnections[connectionsString]].e == connections.e and connections.e != -1 ) {
	std::cout << "Connections already established. Probably a duplicate triangle! Ignoring... \n" ;
      } else if ( connectionsList[uniqueConnections[connectionsString]].r == connections.r and connections.r != -1 ) {
	std::cout << "Connections already established. Probably a duplicate triangle! Ignoring... \n" ;
      } else {
	std::cerr << "Connections already established. Not a duplicate triangle! Error probably in sortConnections!  \n" ;
	std::cout << connectionsList[uniqueConnections[connectionsString]].q << ' ' 
		  << connectionsList[uniqueConnections[connectionsString]].w << ' ' 
		  << connectionsList[uniqueConnections[connectionsString]].e << ' ' 
		  << connectionsList[uniqueConnections[connectionsString]].r << std::endl ;
	std::cout << connections.q << ' ' << connections.w << ' ' << connections.e << ' ' << connections.r << std:: endl;
      };
    };
  };

};

void sortConnections ( const std::vector<TriangleVertices>& triangleVerticesList, std::vector<Connections>& connectionsList, std::vector<int>& vertexFirstListing, std::vector<int>& vertexLastListing ) {
  std::unordered_map<std::string, size_t> uniqueConnections ;
  Connections connections ;
  int a, b, c;
  size_t numconns ;
  for (const TriangleVertices& triangleVertices : triangleVerticesList) {
    vertexFirstListing.push_back ( -1 ) ; 
    a = triangleVertices.a ;
    b = triangleVertices.b ;
    c = triangleVertices.c ;
    if ( a < b and b < c ) {
      connections = { b, a,-1, c } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { c, a, b,-1} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { c, b,-1, a } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } else if ( c < a and a < b ) {
      connections = { a, c,-1, b } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { b, c, a,-1} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { b, a,-1, c } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } else if ( b < c and c < a ) {
      connections = { c, b, -1, a} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { a, b, c,-1} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { a, c,-1, b } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } else if ( a < c and c < b ) {
      connections = { c, a, b, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { b, a,-1, c} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { b, c, a, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } else if ( c < b and b < a ) {
      connections = { b, c, a, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { a, c,-1, b} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { a, b, c, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } else if ( b < a and a  < c ) {
      connections = { a, b, c, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { c, b,-1, a} ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
      connections = { c, a, b, -1 } ;
      evaluateConnection ( connections, connectionsList, uniqueConnections, vertexFirstListing, vertexLastListing ) ;
    } 	 
  }

}


void readAsciiSTL(std::ifstream& inputFile, std::vector<Vertex>& uniqueVertexList, std::vector<TriangleVertices>& triangleVerticesList) {

  std::string line;

  Vertex vertices[3];
  Triangle triangle;
  bool normalRead = false ;
  size_t triangIndex = 0;
  size_t normalCount = 0;
  size_t vertexCount = 0;
  size_t numTriangles = 0;
  size_t numUniqueVertices = 0;
  std::unordered_map<std::string, size_t> uniqueVertices ;

  // Process each line of the input file
  while (std::getline(inputFile, line)) {
    line = trim(line);
    if (line.empty())
      continue;
    
    std::stringstream ss(line);
    std::string keyword;
    ss >> keyword;
    
    Vertex vertex ;
    Vertex normal ; 
    
    if (keyword == "facet") {
      ss >> keyword; // Skip "normal"
      ss >> normal.x >> normal.y >> normal.z;
      normalRead = true ;
      triangle.N = normal ; 
    }
    else if (keyword == "vertex") {    
      ss >> vertex.x >> vertex.y >> vertex.z;
      vertices[vertexCount++] = vertex ; 
    }
    if (normalRead and vertexCount == 3 ) {
      triangle.A = vertices[0] ;
      triangle.B = vertices[1] ;
      triangle.C = vertices[2] ;
      processTriangle(triangle, uniqueVertices, uniqueVertexList, triangleVerticesList ) ;
      normalRead = false;
      vertexCount = 0;
      numTriangles++ ;
    }
	
    }

}
  
void readBinarySTL(std::ifstream& inputFile, std::vector<Vertex>& uniqueVertexList, std::vector<TriangleVertices>& triangleVerticesList) {

  Triangle triangle;
  size_t numUniqueVertices = 0;
  std::unordered_map<std::string, size_t> uniqueVertices ;
  
  // Skip the 80-byte header
  inputFile.seekg(80, std::ios::beg);
  
  // Read the number of triangles
  uint32_t numTriangles;
  inputFile.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));
  
    // Process each triangle
  for (uint32_t i = 0; i < numTriangles; ++i) {
    // Read the normal vector (3 floats)
    Vertex normal;
    inputFile.read(reinterpret_cast<char*>(&normal), sizeof(normal));
    triangle.N = normal ;
    // Read the vertices (3 * 3 floats)
    Vertex vertex;
    inputFile.read(reinterpret_cast<char*>(&vertex), sizeof(vertex));
    triangle.A = vertex ;
    inputFile.read(reinterpret_cast<char*>(&vertex), sizeof(vertex));
    triangle.B = vertex ;
    inputFile.read(reinterpret_cast<char*>(&vertex), sizeof(vertex));
    triangle.C = vertex ;
    
    processTriangle(triangle, uniqueVertices, uniqueVertexList, triangleVerticesList ) ;
      
    // Skip the attribute byte count
    inputFile.seekg(sizeof(uint16_t), std::ios::cur);
  }
}

void writeVTKTriFile(const std::string& fileName, const std::vector<Vertex>& uniqueVertexList, const std::vector<TriangleVertices>& triangleVerticesList) {

  
    std::ofstream outputFile(fileName);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << fileName << std::endl;
        return;
    }

    // Write the VTK header
    outputFile << "# vtk DataFile Version 4.2\n";
    outputFile << "Unique Vertices and Triangle Indices\n";
    outputFile << "ASCII\n";
    outputFile << "DATASET POLYDATA\n";

    // Write the vertices as POINTS
    outputFile << "POINTS " << uniqueVertexList.size() << " float\n";
    for (const Vertex& vertex : uniqueVertexList) {
        outputFile << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Write the triangles as POLYGONS
    outputFile << "POLYGONS " << triangleVerticesList.size() << " " << triangleVerticesList.size() * 4 << "\n";
    for (const TriangleVertices& triangle : triangleVerticesList) {
        outputFile << "3 " << triangle.a << " " << triangle.b << " " << triangle.c << "\n";
    }

    outputFile.close();

    std::cout << "Successfully written VTK file: " << fileName << std::endl;
  
}

void writeVTKQuadFile(const std::string& fileName, const std::vector<Vertex>& uniqueVertexList, const std::vector<Connections>& connectionsList) {

  
    std::ofstream outputFile(fileName);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << fileName << std::endl;
        return;
    }

    // Write the VTK header
    outputFile << "# vtk DataFile Version 4.2\n";
    outputFile << "Unique Vertices and Quad Indices\n";
    outputFile << "ASCII\n";
    outputFile << "DATASET POLYDATA\n";

    // Write the vertices as POINTS
    outputFile << "POINTS " << uniqueVertexList.size() << " float\n";
    for (const Vertex& vertex : uniqueVertexList) {
        outputFile << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Write the triangles as POLYGONS
    outputFile << "POLYGONS " << connectionsList.size() << " " << connectionsList.size() * 5 << "\n";
    for (const Connections& connection : connectionsList) {
        outputFile << "4 " << connection.q << " " << connection.e << " " << connection.w << " " << connection.r <<  "\n";
    }

    outputFile.close();

    std::cout << "Successfully written VTK file: " << fileName << std::endl;

}

void writeC3DFile(const std::string& fileName, const std::vector<Vertex>& uniqueVertexList, const std::vector<Connections>& connectionsList, const std::vector<int>& vertexFirstListing, std::vector<int>& vertexLastListing) {

  // open output file
    std::ofstream outputFile(fileName);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << fileName << std::endl;
        return ;
    };

  
  // write output file
    uint32_t numVertices = uniqueVertexList.size() ; 
    for (uint32_t i = 0; i < numVertices; ++i) {
      //for (const Vertex& uniqueVertex : uniqueVertexList) {
      Vertex uniqueVertex = uniqueVertexList[i] ;
      outputFile << uniqueVertex.x << " " << uniqueVertex.y << " " << uniqueVertex.z << " " << vertexFirstListing [i]<<  std::endl;
    }
    //for (const TriangleVertices& triangleVertices : triangleVerticesList) {
      //outputFile << triangleVertices.a << " " << triangleVertices.b << " " << triangleVertices.c << std::endl;
      //}
    for (const Connections& connection : connectionsList ) {
      outputFile << connection.q << " " << connection.w << " " << connection.e << " " <<  connection.r << " " <<  connection.n << std::endl;
    }

    outputFile.close();

    std::cout << "Successfully exported unique vertices to file: " << fileName << std::endl;


};


int main(int argc, char* argv[]) {
  // Start the timer
  auto start = std::chrono::high_resolution_clock::now();


  if (argc < 3 ) {
        std::cout << "Usage: " << argv[0] << " <input_file.stl> <output_file.stl>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];
    std::string option = "none";
    if (argc == 4 ) {
      option = argv[3];
    }


    // open STL file
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        return 1;
    }
    
    // check if ASCII
    std::string line;
    std::getline(inputFile, line);
    bool isAscii = (line.find("solid") != std::string::npos);
    inputFile.seekg(0, std::ios::beg); // Reset file position


    std::vector<Vertex> uniqueVertexList;
    std::vector<TriangleVertices> triangleVerticesList;

    if (isAscii) {
      readAsciiSTL(inputFile, uniqueVertexList, triangleVerticesList );
    } else {
      readBinarySTL(inputFile, uniqueVertexList, triangleVerticesList);
      // Check if end-of-file reached
    }

    inputFile.close();

    std::vector<int> vertexFirstListing ;
    std::vector<int> vertexLastListing ;
    for (uint32_t i=0; i< uniqueVertexList.size(); i++ ) {
      vertexFirstListing.push_back ( -1) ;
      vertexLastListing.push_back ( -1) ;
    }
    std::vector<Connections> connectionsList ;
    sortConnections ( triangleVerticesList, connectionsList, vertexFirstListing, vertexLastListing ) ;


    std::string outFileType = outputFileName.substr(outputFileName.length() - 4, outputFileName.length());
    if (outFileType == ".c3d") {
      writeC3DFile ( outputFileName, uniqueVertexList, connectionsList, vertexFirstListing, vertexLastListing ) ;
    } else if (outFileType == ".vtk" and option == "none") {
      writeVTKTriFile(outputFileName, uniqueVertexList, triangleVerticesList);
    } else if (outFileType == ".vtk" and option == "-quad") {
      writeVTKQuadFile(outputFileName, uniqueVertexList, connectionsList);
    } else {
      std::cout << "Error in output file specification. No output written! " << std::endl;
    }

    

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Print the duration
    std::cout << "Execution time: " << duration.count() << " seconds." << std::endl;
    
    return 0;
}


