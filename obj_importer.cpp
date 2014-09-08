/*
 *  Read routine for Wavefront OBJ
 
 *  Amiras HxSurface can only display faces as triangles (not quadrilaterals for example),
 *  so .obj files must have appropriate faces otherwise surfaces rendered in Amira with the SurfaceView module
 *  look false!
 *  Nevertheless this reader will not throw an error on this and will import every object file.
 *  This .obj reader can only read polygonal data - no free-form geometry (e.g. bezier curves ...)
 
 */

//C++ standard headers:
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

//amira headers:
#include <mclib/McStringTokenizer.h>
#include <hxcore/HxData.h>
#include <hxcore/HxMessage.h>

#include <hxsurface/HxSurface.h>
#include <hxsurface/HxSurfaceField.h>

//my headers:
#include "Obj_ImporterAPI.h"

OBJ_IMPORTER_API
int obj_importer(const char* filename)
{
    
    unsigned int i;//for counter in for loops
    unsigned int aSize;//saves size of triangeles or points

    if (filename) {
        
        theMsg->printf(" Wavefront .obj file loader (author: Bernhard Rosensteiner, brosensteiner@gmail.com)");
        
        cObj myObject(filename);//this object handles the complete reading procedure of an .obj file -> see class definition
        
        theMsg->printf("%s has %d points and %d faces", filename, myObject.vertices.size(), myObject.faces.size());
        
        //allocate new HxSurface object
        HxSurface *aSurface = new HxSurface;
        aSurface->addMaterial("Inside", 0);
        aSurface->addMaterial("Outside", 1);
        
        HxSurface::Patch* aPatch = new HxSurface::Patch;
        aSurface->patches.append(aPatch);
        aPatch->innerRegion = 0;
        aPatch->outerRegion = 1;
        
        aSurface->points.resize(myObject.vertices.size());
        aSurface->triangles.resize(myObject.faces.size());
        
        for (i=0, aSize = myObject.vertices.size(); i < aSize; i++) { // read vertices in aSurface
            
            McVec3f& aPoint = aSurface->points[i];
            aPoint[0] = myObject.vertices[i].v[0];
            aPoint[1] = myObject.vertices[i].v[1];
            aPoint[2] = myObject.vertices[i].v[2];
        }
        
        for (i=0, aSize = myObject.faces.size(); i < aSize; i++) { // read faces in aSurface
            
            Surface::Triangle& aTriangle = aSurface->triangles[i];
            aTriangle.points[0] = myObject.faces[i].vertex[0];
            aTriangle.points[1] = myObject.faces[i].vertex[1];
            aTriangle.points[2] = myObject.faces[i].vertex[2];
            
        }
        
        aPatch->triangles.resize(myObject.faces.size());
        for (i=0, aSize = myObject.faces.size(); i < aSize; i++) {
            aPatch->triangles[i] = i; // add all triangles to one patch
        }
        
        aSurface->recompute();//recompute surface information, when not done Amira crashes to 90% when one wants to connect a SurfaceView module
        
        if (aSurface) {
            HxData::registerData(aSurface, filename);
            aSurface->createAllEdges(1);//fill edges arrays
            aSurface->updateInfo();
            aSurface->compute();
        }
        else
            theMsg->printf("could not register surface from %s file!", filename);
        
        // Fix the load command of all created objects
        /*
         McString loadCmd;
        loadCmd.printf("set TMPIO [load -obj %s]\n"
                       "lindex $TMPIO 0", filename);
        aSurface->setLoadCmd(loadCmd,1);
        */
    }
    

    return 1;
}
