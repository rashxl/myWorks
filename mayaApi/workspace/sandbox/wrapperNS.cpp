/*
 * wrapperNS.cpp
 *
 *  Created on: Apr 4, 2017
 *      Author: rash
 */

/// descriptions: this is script get closes POINT on the restSurface from each vertex mesh and then from founded point on surface take NORMAL and
/// compute distance between vertex and founded point, at the end add to the array MVectorArray for UVs and MFloatArray for distance.
/// After that find from founded UVs(array) the point on the sourceSurface and take POSITION and NORMAL, then multiply distance and normal, and at the end plus founded new point and normal


#include "wrapperNS.h"

float noZero(double value);
float dotP(MVector A, MVector B);

MObject wrapperNS::inRestSurf;
MObject wrapperNS::inSourceSurf;
MObject wrapperNS::cache;
MObject wrapperNS::inMesh;
MObject wrapperNS::outMesh;



void * wrapperNS::creator(){
	return new wrapperNS();
}


MStatus wrapperNS::initialize(){

	MStatus stat;
	MFnTypedAttribute mfnTypeAttr;
	MFnNumericAttribute mfnnumAttr;

	//##### rest surface input
	inRestSurf = mfnTypeAttr.create("inRestSurf", "inRSurf", MFnNurbsSurfaceData::kNurbsSurface, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    CHECK_MSTATUS ( mfnTypeAttr.setStorable( true ) );
	stat = addAttribute(inRestSurf);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//##### source surface input
	inSourceSurf = mfnTypeAttr.create("inSourceSurf", "inSSurf", MFnNurbsSurfaceData::kNurbsSurface, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    CHECK_MSTATUS ( mfnTypeAttr.setStorable( true ) );
	stat = addAttribute(inSourceSurf);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//##### mesh input
	inMesh = mfnTypeAttr.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    CHECK_MSTATUS ( mfnTypeAttr.setStorable( true ) );
	stat = addAttribute(inMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//##### mesh output
	outMesh = mfnTypeAttr.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    CHECK_MSTATUS ( mfnTypeAttr.setStorable( true ) );
	stat = addAttribute(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inSourceSurf, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	return MS::kSuccess;
}


MStatus wrapperNS::compute(const MPlug &plug, MDataBlock &data){
	MStatus stat;

	MThreadUtils::syncNumOpenMPThreads();

	#ifndef _DEBUG
	#ifdef _OPENMP
	omp_set_num_threads(omp_get_num_procs());
	#endif
	#endif


	if(plug == outMesh){

		// get inRest surface //
		MDataHandle inRestSurfHandle = data.inputValue(inRestSurf, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject inRestSurfObject = inRestSurfHandle.asNurbsSurface();
		MFnNurbsSurface SurfFnRest(inRestSurfObject);

		// get inRest surface //
		MDataHandle inSourceSurfHandle = data.inputValue(inSourceSurf, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject inSourceSurfObject = inSourceSurfHandle.asNurbsSurface();
		MFnNurbsSurface SurfFnSource(inSourceSurfObject);

		/// get inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();
		MFnMesh fnMesh(moMesh);

		/// outMesh ///
		MDataHandle outPolyDataHandle = data.outputValue(outMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		outPolyDataHandle.copy(inPolyDataHandle);


		MPointArray idVertex;

		fnMesh.getPoints(idVertex);

		int idLen = idVertex.length();


		/// this code caching once

		if(PntVecUVs == NULL){

			PntVecUVs = new MVectorArray[idLen];
			PntDisVec = new MFloatArray[idLen];

			for (int i =0; i<idLen; ++i){
				double u,v;
				MVector vec;
				/// find closes point from mesh  and UVs
				MPoint closPt = SurfFnRest.closestPoint(idVertex[i], false, &u, &v);
				vec.x = u;
				vec.y = v;
				vec.z =0;
				PntVecUVs[i].append(vec);
				vec = closPt - idVertex[i];
				MVector vecNorm = SurfFnRest.normal(u,v);
				PntDisVec[i].append(dotP(vec, vecNorm));

			}
		}

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		/// find new point from founded UVs
		for (int i =0; i<idLen; ++i){

			MPoint SrcPtSrf;
			SurfFnSource.getPointAtParam(PntVecUVs[i][0].x,PntVecUVs[i][0].y, SrcPtSrf);
			MVector vecNorm = SurfFnSource.normal(PntVecUVs[i][0].x,PntVecUVs[i][0].y);
			vecNorm *= -PntDisVec[i][0];
			idVertex[i] = SrcPtSrf + vecNorm;

		}

		/*

		MVectorArray UVs; UVs.clear();
		MFloatArray dis; dis.clear();

		for (int i =0; i<idVertex.length(); ++i){
			double u,v;
			MVector vec;
			MPoint closPt = SurfFnRest.closestPoint(idVertex[i], false, &u, &v);
			vec.x = u;
			vec.y = v;
			vec.z =0;
			UVs.append(vec);

			vec = closPt - idVertex[i];
			MVector vecNorm = SurfFnRest.normal(u,v);
			dis.append(dotP(vec, vecNorm));

		}


		for (int i =0; i<UVs.length(); ++i){

			MPoint SrcPtSrf;
			SurfFnSource.getPointAtParam(UVs[i].x,UVs[i].y, SrcPtSrf);
			MVector vecNorm = SurfFnSource.normal(UVs[i].x,UVs[i].y);
			vecNorm *= dis[i];
			idVertex[i] = SrcPtSrf + vecNorm;

		}
		*/

		fnMesh.setPoints(idVertex);

		outPolyDataHandle.setClean();

		data.setClean(plug);


	}else{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}
