/*
 * meshClone.h
 *
 *  Created on: Jan 09, 2017
 *      Author: rash
 */

#ifndef MESHCLONE_H_
#define MESHCLONE_H_

#include <iostream>
#include <algorithm>
#include <cmath>

#include <maya/MPxNode.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>
#include <maya/MGlobal.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>


using namespace std;

class meshClone : public MPxNode{
public:
	meshClone(){}
	virtual ~meshClone(){}
	virtual MStatus compute(const MPlug &plug, MDataBlock &data);
	static void* creator();
	static MStatus initialize();
	double quaternionDot(MQuaternion a, MQuaternion b);
	MQuaternion quaternionSlerp(MQuaternion a, MQuaternion b, double t);

	static MTypeId id;
	static MObject inCurv;
	static MObject rootA;
	static MObject rootB;
	static MObject outMesh;
	static MObject inMesh;
	static MObject count;
	static MObject crvStart;
	static MObject crvEnd;
	static MObject offset;
};


#endif
