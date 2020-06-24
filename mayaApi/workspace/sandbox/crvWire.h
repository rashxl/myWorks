/*
 * wireDef.h
 *
 *  Created on: Aug 1, 2017
 *      Author: rash
 */

#ifndef CRVWIRE_H_
#define CRVWIRE_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDataHandle.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MRampAttribute.h>
#include <maya/MItMeshVertex.h>
#include <maya/MThreadUtils.h>
#include <maya/MFnMeshData.h>


#include <omp.h>

#include <stddef.h>
#include <ctime>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <math.h>

class crvWire:public MPxNode{
public:
	crvWire(){}
	virtual ~crvWire() {}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MObject mesh;
	static MObject curve;
	static MObject scale;
	static MObject num;
	static MObject outMesh;
	static MObject env;
	static MTypeId id;
	static MObject waveFallOff;
	static MObject scaleFallOff;
	static MObject offset;

	static MObject shift;
	static MObject amount;
	static MObject len;


//private:
	//MIntArray * inPtr;
};





#endif /* CRVWIRE_H_ */
