#include "pop.h"

MObject pop::inPoly;
MObject pop::aPose;
MObject pop::aPoseX;
MObject pop::aPoseY;
MObject pop::aPoseZ;
MObject pop::vtx;



void *pop::creator(){
	return new pop;
}


MStatus pop::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;

	MGlobal::displayInfo("### pop");

	if(plug == aPose){

		//// position ////
		int nPosition = data.inputValue(vtx).asInt();

		/// inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inPoly, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();

		MFnMesh fnMesh(moMesh);

		/// get world space of position
		MPoint pPosition;
		fnMesh.getPoint(nPosition, pPosition, MSpace::kWorld);

		/// set constrain translate attribute
		MDataHandle hConTranslate = data.outputValue(aPose);
		hConTranslate.set3Double(pPosition.x,pPosition.y,pPosition.z);

	}else{
		return MS::kUnknownParameter;
	}


	return stat;
}

MStatus pop::initialize(){
	MStatus stat;

	MFnTypedAttribute MfnTypeAttr;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;

	inPoly = MfnTypeAttr.create("inPoly", "inP", MFnData::kMesh, &stat);
    CHECK_MSTATUS ( MfnTypeAttr.setStorable( true ) );
	stat = addAttribute(inPoly);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aPosition attribute ////////
	vtx = nAttr.create("position", "pos", MFnNumericData::kInt, 0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setMin(0));
	stat = addAttribute(vtx);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	aPoseX = nAttr.create("conTranslateX", "ctx", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aPoseY = nAttr.create("conTranslateY", "cty", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aPoseZ = nAttr.create("conTranslateZ", "ctz", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aPose = cAttr.create("conTranslate", "cot", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(cAttr.addChild(aPoseX));
	CHECK_MSTATUS(cAttr.addChild(aPoseY));
	CHECK_MSTATUS(cAttr.addChild(aPoseZ));
	CHECK_MSTATUS(cAttr.setKeyable(false));
	CHECK_MSTATUS(cAttr.setWritable(false));
	CHECK_MSTATUS(cAttr.setStorable(false));
	stat = addAttribute(aPose);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inPoly, aPose );
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( vtx, aPose );
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;
}
