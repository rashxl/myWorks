/*
 * PointOnMesh.h
 *
 *  Created on: Oct 3, 2016
 *      Author: rash
 */

#ifndef POINTONMESH_H_
#define POINTONMESH_H_

#include <regex>
//#include <regex.h>
#include <array>
#include <iostream>
#include <iomanip>

#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MMeshIntersector.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MObject.h>
#include <maya/MSelectionList.h>
#include <maya/MArgList.h>
#include <maya/MFloatPoint.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MPointArray.h>
#include <maya/MMeshIntersector.h>
#include <maya/MMatrix.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnGeometryFilter.h>
#include <maya/MFnSet.h>
#include <maya/MItGeometry.h>
#include <maya/MWeight.h>
#include <maya/MIntArray.h>

struct weights{
public:
	float w;
	int id;
	MPoint mPos;

};

class PointOnMeshCmd : public MPxCommand{
private:
	MPointArray vertexs;
public:
	PointOnMeshCmd();
	virtual ~PointOnMeshCmd();
	virtual MStatus doIt(const MArgList& args);
	static MSyntax newSyntax();
	static void* creator();
	float getNewWeights( MPointOnMesh& info, MObject mesh, MFloatArray w, MIntArray idComp );
};



#endif /* POINTONMESH_H_ */


