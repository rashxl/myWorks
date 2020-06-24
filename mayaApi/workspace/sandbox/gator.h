/*
 * gator.h
 *
 *  Created on: May 29, 2017
 *      Author: rash
 */

#ifndef GATOR_H_
#define GATOR_H_

#include <regex>
#include <regex.h>
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
#include <maya/MItMeshVertex.h>
#include <maya/MFnTransform.h>
#include <maya/MThreadUtils.h>

//struct weight{
//public:
//	float w;
//	int id;
//	MPoint mPos;
//
//};

class gator : public MPxCommand{
private:
	MPointArray vertexs;
public:
	gator();
	virtual ~gator();
	virtual MStatus doIt(const MArgList& args);
	static MSyntax newSyntax();
	static void* creator();
	float getNewWeights( MPointOnMesh& info, MObject mesh, MFloatArray w, MIntArray idComp );
};





#endif /* GATOR_H_ */
