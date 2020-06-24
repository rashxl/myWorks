#ifndef TENSION_H_
#define TENSION_H_
#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDataHandle.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MIntArray.h>
#include <maya/MRampAttribute.h>
#include <maya/MItMeshVertex.h>
#include <maya/MThreadUtils.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>

#include <omp.h>

#include <stddef.h>
#include <ctime>
#include <stdio.h>
#include <cmath>


class tension:public MPxNode{
public:
	tension() {
		inPtr = NULL;
		edges = 0;
		colorPerVertexDisplay = 0;
	}
	virtual ~tension() {
		if(inPtr != NULL){
			delete[] inPtr;
		}
	}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MObject inMesh;
	static MObject floatMap;
	static MObject doubleMap;
	static MObject resMesh;
	static MObject outMesh;
	static MObject env;
	static MTypeId id;
	static MObject tensionColorContrast;
	static MObject tensionColorRamp;
	static MObject drawColorPerVertex;
	static MObject colorDisplay;

private:
	MIntArray * inPtr;
	int edges;
	int colorPerVertexDisplay;

};
#endif
