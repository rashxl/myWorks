/*
 * mirrorClusterCmd.h
 *
 *  Created on: Oct 21, 2016
 *      Author: rash
 */

#ifndef MIRRORCLUSTERCMD_H_
#define MIRRORCLUSTERCMD_H_


#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSet.h>
#include <maya/MItGeometry.h>
#include <maya/MFloatArray.h>
#include <maya/MWeight.h>
#include <maya/MIntArray.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshVertex.h>

#include <cmath>

class mirrorClusterCmd : public MPxCommand{

public:
	mirrorClusterCmd() {}
	virtual ~mirrorClusterCmd() {}
	virtual MStatus doIt(const MArgList& args);
	static MSyntax newSyntax();
	static void* creator();
	static MStatus mirrorOneCluster(MSelectionList selection, MString axis);
	static MStatus mirrorTwoCluster(MSelectionList selection, MString axis);
private:
	static const char *axisFlag;
	static const char *axisLongFlag;

};




#endif /* MIRRORCLUSTERCMD_H_ */
