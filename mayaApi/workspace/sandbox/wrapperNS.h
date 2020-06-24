/*
 * wrapperNS.h
 *
 *  Created on: Apr 4, 2017
 *      Author: rash
 */

#ifndef WRAPPERNS_H_
#define WRAPPERNS_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MThreadUtils.h>

#include <omp.h>

class wrapperNS : public MPxNode{
public:
	wrapperNS(){

		PntVecUVs = NULL;
		PntDisVec = NULL;

	}
	virtual ~wrapperNS(){

		if(PntVecUVs != NULL){
			delete[] PntVecUVs;
		}

		if(PntDisVec != NULL){
			delete[] PntDisVec;
		}
	}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId id;
	static MObject cache;
	static MObject inRestSurf;
	static MObject inSourceSurf;
	static MObject inMesh;
	static MObject outMesh;

private:

	MVectorArray * PntVecUVs;
	MFloatArray * PntDisVec;


};



#endif /* WRAPPERNS_H_ */
