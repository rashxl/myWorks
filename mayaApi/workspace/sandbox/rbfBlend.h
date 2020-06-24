/*
 * rbfBlend.h
 *
 *  Created on: Jul 6, 2017
 *      Author: rash
 */




#ifndef RBFBLEND_H_
#define RBFBLEND_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MArrayDataBuilder.h>

#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>

#include <maya/MDataHandle.h>
#include <maya/MDoubleArray.h>
#include <maya/MArrayDataHandle.h>

#include <maya/MVectorArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MThreadUtils.h>

#include "MatrixNN.h"

#include <omp.h>

#include <cmath>


class rbfBlend : public MPxNode{
public:
	rbfBlend();
	virtual ~rbfBlend();

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();


	MStatus readMatrixArray(MDataBlock &data, MDoubleArray &dArrWts, MVectorArray &vArrRead, MVector &vCur, MMatrixArray &poseMat, MMatrix &DrMat, unsigned int &uMat);
	MStatus interpWeights(MDataBlock &data, MDoubleArray &dArrWts, const MVectorArray &vArrRead, const MVector &vCur, MMatrixArray &poseMat, MMatrix &DrMat, const double &dRBFWidth) ;

	static MTypeId id;

	static MObject aPoses;		// pose compound
	static MObject aPoseXform;		// pose compound
	static MObject aDriver;		// curent driver

	static MObject aSigma;		// sigma func.

	static MObject comWeight;		// compound weight
	static MObject aWeight;		// weight
	static MObject aTypes;

	static MObject aAxis;		// axis

	static MObject aResult;		// result

	static MObject aMethod;
	static MObject aMode;

//	static MObject trans;
//	static MObject transX;
//	static MObject transY;
//	static MObject transZ;

	static MObject resPoses;		// resPose compound
	static MObject resWeight;		// result Pose weight
};

#endif // RBFBLEND_H_
