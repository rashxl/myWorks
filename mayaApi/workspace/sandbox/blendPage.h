/*
 * blendPage.h
 *
 *  Created on: Apr 12, 2017
 *      Author: rash
 */

#ifndef BLENDPAGE_H_
#define BLENDPAGE_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MFnMesh.h>
#include <maya/MDoubleArray.h>
#include <maya/MQuaternion.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MThreadUtils.h>

#include <omp.h>

#include <cmath>



class blendPage : public MPxNode{
public:
	blendPage()
	{
		ptrvtxArrayMid = NULL;
		ptrvtxArrayR = NULL;
		ptrvtxArrayL = NULL;
		rLen = NULL;
		lLen = NULL;
		rAngl = NULL;
		lAngl = NULL;
	}
	virtual ~blendPage(){
		if(ptrvtxArrayMid != NULL){
			delete [] ptrvtxArrayMid, ptrvtxArrayR, ptrvtxArrayL, rLen, lLen, rAngl, lAngl;
		}
	}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId id;

	static MObject inMesh;
	static MObject rMesh;
	static MObject lMesh;
	static MObject rPage;
	static MObject lPage;
	static MObject mainSlider;
	static MObject pageSliders;
	static MObject outMesh;
	static MObject idPage;

private:

	MPointArray *ptrvtxArrayMid;
	MPointArray *ptrvtxArrayR;
	MPointArray *ptrvtxArrayL;
	MDoubleArray *rLen;
	MDoubleArray *lLen;
	MDoubleArray *rAngl;
	MDoubleArray *lAngl;
	MIntArray *idPgs;
};



#endif /* BLENDPAGE_H_ */
