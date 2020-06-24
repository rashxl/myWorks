/*
 * blendPage.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: rash
 */

#include "blendPage.h"
#ifndef PI
#define PI 3.14159265 // PI
#endif

//############ math func ##################3

float getAngle(MVector A, MVector B);
float clamping(float val, float max, float min);
float rescale(float value, float oldMin, float oldMax, float newMin, float newMax);

MTypeId blendPage::id(0x0127654);
MObject blendPage::inMesh;
MObject blendPage::rMesh;
MObject blendPage::lMesh;
MObject blendPage::rPage;
MObject blendPage::lPage;
MObject blendPage::mainSlider;
MObject blendPage::pageSliders;
MObject blendPage::outMesh;
MObject blendPage::idPage;


void * blendPage::creator(){
	return new blendPage;
}



MStatus blendPage::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;

	/// multthread
	MThreadUtils::syncNumOpenMPThreads();

	#ifndef _DEBUG
	#ifdef _OPENMP
	omp_set_num_threads(omp_get_num_procs());
	#endif
	#endif

	if(plug == outMesh ){

	/// inMesh ///
	MDataHandle inMeshDataHandle = data.inputValue(inMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject MOinMesh = inMeshDataHandle.asMesh();
	MFnMesh fnInMesh(MOinMesh);

	/// rMesh ///
	MDataHandle rMeshDataHandle = data.inputValue(rMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject MOrMesh = rMeshDataHandle.asMesh();
	MFnMesh fnrMesh(MOrMesh);

	/// lMesh ///
	MDataHandle lMeshDataHandle = data.inputValue(lMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject MOlMesh = lMeshDataHandle.asMesh();
	MFnMesh fnlMesh(MOlMesh);

	/// outMesh ///
	MDataHandle MoutMeshHandle = data.outputValue(outMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MoutMeshHandle.copy(inMeshDataHandle);
	MObject moOutMesh = MoutMeshHandle.asMesh();
	MFnMesh fnOutMesh(moOutMesh);


	MPointArray pts;
	fnInMesh.getPoints(pts, MSpace::kObject);
	int lenVtx = pts.length();


	MArrayDataHandle pagesArray = data.inputValue(pageSliders);


	int idPages = data.inputValue(idPage).asInt();

	if(idPages == 0 || idPages != pagesArray.elementCount() ){
		MGlobal::displayWarning(" #idPages a number, must be it the same numbers of pages slider#");
		MGlobal::displayWarning(MString("number sliders is ")+pagesArray.elementCount());
		return MS::kSuccess;
	}

	if(ptrvtxArrayMid == NULL){

		ptrvtxArrayMid =  new MPointArray[lenVtx];
		ptrvtxArrayL =  new MPointArray[lenVtx];
		ptrvtxArrayR =  new MPointArray[lenVtx];

		rLen =  new MDoubleArray[lenVtx];
		lLen =  new MDoubleArray[lenVtx];

		rAngl =  new MDoubleArray[lenVtx];
		lAngl =  new MDoubleArray[lenVtx];

		idPgs = new MIntArray[lenVtx];

		fnInMesh.getPoints(ptrvtxArrayMid[0], MSpace::kObject);
		fnrMesh.getPoints(ptrvtxArrayL[0], MSpace::kObject);
		fnlMesh.getPoints(ptrvtxArrayR[0], MSpace::kObject);

		/// get min max value
		double maxV = ptrvtxArrayMid[0][0].x;
		double minV = ptrvtxArrayMid[0][0].x;

		for(int i = 1; i<lenVtx; i++){
			if(ptrvtxArrayMid[0][i].x>maxV){
				maxV = ptrvtxArrayMid[0][i].x;
			}
			if(ptrvtxArrayMid[0][i].x<minV){
				minV = ptrvtxArrayMid[0][i].x;
			}
		}

		MGlobal::displayInfo(MString("maxVal ")+maxV);
		MGlobal::displayInfo(MString("minVal ")+minV);

		for (int i=0; i<lenVtx; ++i){
			MVector vecinMesh(ptrvtxArrayMid[0][i]);
			MVector vecrMesh(ptrvtxArrayR[0][i]);
			MVector veclMesh(ptrvtxArrayL[0][i]);

			MVector Vr(vecrMesh.x, vecrMesh.y, 0);
			MVector Vl(veclMesh.x, veclMesh.y, 0);
			MVector Vm(vecinMesh.x, vecinMesh.y, 0);

			rLen[0].append(Vr.length()/Vm.length());
			lLen[0].append(Vl.length()/Vm.length());


			rAngl[0].append(getAngle(Vr, Vm));
			lAngl[0].append(getAngle(Vl, Vm));


			//id pages
			int resIdpages = (int)rescale(vecinMesh.x, minV, maxV, 0, idPages);
			idPgs[0].append(resIdpages);


		}

	}



	// rPage //
	float rPagef = data.inputValue(rPage).asFloat();
	// lPage //
	float lPagef = data.inputValue(lPage).asFloat();


	float newRpage = rescale(lPagef, -90, 0, 1.0, 0.0);
	float newLpage = rescale(rPagef, 0.0, 90.0, 0.0, -1.0);

	MPointArray newVtx;

	#ifndef _DEBUG
	#ifdef _OPENMP
	#pragma omp parallel for private(newRpage,newLpage)
	#endif
	#endif

	for (int i=0; i<lenVtx; i++){

		pagesArray.jumpToElement(idPgs[0][i]);
		// pages
		float val = pagesArray.inputValue().asFloat();
		// slider //
		float slider = data.inputValue(mainSlider).asFloat() + val;

		float clampval = clamping(slider, newRpage, newLpage);


		MVector Vec(ptrvtxArrayMid[0][i].x,ptrvtxArrayMid[0][i].y, 0), vecRot, qVec(0,0,-1);

		if(clampval > 0){
			MQuaternion quat(lAngl[0][i]*clampval, qVec);
			vecRot = Vec.rotateBy(quat);
			vecRot *= (1-clampval)*1 + clampval* lLen[0][i];
		}else{
			MQuaternion quat(rAngl[0][i]*clampval, qVec);
			vecRot = Vec.rotateBy(quat);
			vecRot *= (1-std::abs(clampval))*1 + std::abs(clampval)* rLen[0][i];
		}


		vecRot.z = ptrvtxArrayMid[0][i].z;

		newVtx.append(vecRot);

	}

	fnOutMesh.setPoints(newVtx);

	}else{
		return MS::kUnknownParameter;
	}

	return stat;
}


MStatus blendPage::initialize(){

	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;

	//// id pages ////
	idPage = attNum.create("idPage", "idPage", MFnNumericData::kInt,  0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	attNum.setKeyable(true);
	stat = addAttribute(idPage);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute inMesh ////
	inMesh = attrType.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(inMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute rMesh ////
	rMesh = attrType.create("rMesh", "rMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(rMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute lMesh ////
	lMesh = attrType.create("lMesh", "lMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(lMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute outMesh ////
	outMesh = attrType.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// mainSlider
	mainSlider = attNum.create( "mainSlider", "mainSlider",	MFnNumericData::kFloat, 0.0, &stat );
	attNum.setKeyable(true);
	stat = addAttribute (mainSlider);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// right Page
	rPage = attNum.create( "rPage", "rPage",	MFnNumericData::kFloat, 0.0, &stat );
	attNum.setKeyable(true);
	stat = addAttribute (rPage);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// left Page
	lPage = attNum.create( "lPage", "lPage",	MFnNumericData::kFloat, 0.0, &stat );
	attNum.setKeyable(true);
	stat = addAttribute (lPage);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	pageSliders = attNum.create("pageSliders", "pageSliders", 	MFnNumericData::kFloat,  0.0, &stat);
    CHECK_MSTATUS ( attNum.setArray( true ) );
	CHECK_MSTATUS ( attNum.setStorable( true ) )
	//CHECK_MSTATUS ( attNum.setUsesArrayDataBuilder( true ) );
    stat = addAttribute( pageSliders );
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// affects ////
	stat = attributeAffects( mainSlider, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( pageSliders, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( rPage, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( lPage, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return stat;

}
