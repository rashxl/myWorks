/*
 * mirrorClusterCmd_B.h
 *
 *  Created on: Jul 3, 2017
 *      Author: rash
 */

#ifndef CLUSTERCMD_B_H_
#define CLUSTERCMD_B_H_


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


class ClusterCmd_B : public MPxCommand{

public:
	ClusterCmd_B() {}
	virtual ~ClusterCmd_B() {}
	virtual MStatus doIt(const MArgList& args);
	static MSyntax newSyntax();
	static void* creator();
	static MStatus mirrorOneCluster(MSelectionList selection, MString axis);
	static MStatus mirrorTwoCluster(MSelectionList selection, MString axis);
private:
	static const char *axisFlag;
	static const char *axisLongFlag;

};




#endif /* CLUSTERCMD_B_H_ */
