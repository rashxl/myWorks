/*
 * colideP.h
 *
 *  Created on: Apr 21, 2017
 *      Author: rash
 */

#ifndef COLIDEP_H_
#define COLIDEP_H_


#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMesh.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <maya/MEulerRotation.h>


#include <omp.h>

#include <cmath>


class colideP : public MPxNode{
public:
	colideP(){}
	virtual ~colideP(){}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId id;
	static MObject inMesh;
	static MObject inSurfMesh;
	static MObject WM;
	static MObject Point;
	static MObject Ctrl;
	static MObject depth;
	static MObject scale;

	static MObject  translate;
	static MObject  translateX;
	static MObject  translateY;
	static MObject  translateZ;

	static MObject  rotate;
	static MObject  rotateX;
	static MObject  rotateY;
	static MObject  rotateZ;

};




#endif /* COLIDEP_H_ */
