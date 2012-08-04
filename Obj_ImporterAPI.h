/***************************************************************
 *
 * DLL export/import definitions for Windows
 *
 ***************************************************************/

#ifndef OBJ_IMPORTER_API_H
#define OBJ_IMPORTER_API_H

#ifdef HX_OS_WIN
    #ifdef OBJ_IMPORTER_EXPORTS
        #define OBJ_IMPORTER_API __declspec(dllexport)
    #else
        #define OBJ_IMPORTER_API __declspec(dllimport)
    #endif
#else
    #define OBJ_IMPORTER_API
#endif

//C++ standard headers:
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

//amira headers
#include <hxcore/HxMessage.h>


//structures for vertex and face suited for .obj files
struct vertex {
    std::vector<float> v;
    void normalize() {
        float magnitude = 0.0f;
        for (unsigned int i = 0; i < v.size(); i++)
            magnitude += pow(v[i], 2.0f);
        magnitude = sqrt(magnitude);
        for (unsigned int i = 0; i < v.size(); i++)
            v[i] /= magnitude;
    }
    vertex operator-(vertex v2) {
        vertex v3;
        if (v.size() != v2.v.size()) {
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
        } else {
            for (unsigned int i = 0; i < v.size(); i++)
                v3.v.push_back(v[i] - v2.v[i]);
                }
        return v3;
    }
    vertex cross(vertex v2) {
        vertex v3;
        if (v.size() != 3 || v2.v.size() != 3) {
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
        } else {
            v3.v.push_back(v[1]*v2.v[2]-v[2]*v2.v[1]);
            v3.v.push_back(v[2]*v2.v[0]-v[0]*v2.v[2]);
            v3.v.push_back(v[0]*v2.v[1]-v[1]*v2.v[0]);
        }
        return v3;
    }
};

struct face {
    std::vector<int> vertex;
    std::vector<int> texture;
    std::vector<int> normal;
};

//object which collects all vertex/faces... information
// it also takes care of the whole file opening and closing procedure ...
class cObj {
    
private:
protected:
public:
    cObj(const char* filename);
    ~cObj() {
        //nothing to destruct
    };
    
    std::vector<vertex> vertices;
    std::vector<vertex> texcoords;
    std::vector<vertex> normals;
    std::vector<vertex> parameters;
    std::vector<face> faces;

};

//ctor
cObj::cObj(const char * filename) {
    
    std::ifstream ifs(filename, std::ifstream::in);
    std::string line, key;
    
    //filetest and amira console output
    if (!ifs.good()) {
        theMsg->ioError(filename);
        //return 0;
    }
    
    while (ifs.good() && std::getline(ifs, line)) {
        key = "";
        std::stringstream stringStrm(line);
        stringStrm >> key >> std::ws;
        
        if (key == "v") { // vertex
            vertex v; float x;
            while (!stringStrm.eof()) {
                stringStrm >> x >> std::ws;
                v.v.push_back(x);
            }
            vertices.push_back(v);
        } else if (key == "vp") { // parameter
            vertex v; float x;
            while (!stringStrm.eof()) {
                stringStrm >> x >> std::ws;
                v.v.push_back(x);
            }
            parameters.push_back(v);
        } else if (key == "vt") { // texture coordinate
            vertex v; float x;
            while (!stringStrm.eof()) {
                stringStrm >> x >> std::ws;
                v.v.push_back(x);
            }
            texcoords.push_back(v);
        } else if (key == "vn") { // normal
            vertex v; float x;
            while (!stringStrm.eof()) {
                stringStrm >> x >> std::ws;
                v.v.push_back(x);
            }
            v.normalize();
            normals.push_back(v);
        } else if (key == "f") { // face
            face f; int v, t, n;
            while (!stringStrm.eof()) {
                stringStrm >> v >> std::ws;
                f.vertex.push_back(v-1);
                if (stringStrm.peek() == '/') {
                    stringStrm.get();
                    if (stringStrm.peek() == '/') {
                        stringStrm.get();
                        stringStrm >> n >> std::ws;
                        f.normal.push_back(n-1);
                    } else {
                        stringStrm >> t >> std::ws;
                        f.texture.push_back(t-1);
                        if (stringStrm.peek() == '/') {
                            stringStrm.get();
                            stringStrm >> n >> std::ws;
                            f.normal.push_back(n-1);
                        }
                    }
                }
            }
            faces.push_back(f);
        } else {
            //nothing at this point
        }
    }
    
    ifs.close();
    
}


#endif // OBJ_IMPORTER_API_H
