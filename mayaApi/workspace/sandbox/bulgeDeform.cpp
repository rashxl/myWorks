/*
 * bulgeDeform.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: rash
 */




#include "bulgeDeform.h"


MObject budleDeform::aBugleAmount;
MTypeId budleDeform::id(0x003300120);
MObject budleDeform::matrix;
MObject budleDeform::startDist;
MObject budleDeform::endDist;
MObject budleDeform::aMagnitude;
MObject budleDeform::aMean;
MObject budleDeform::aVariance;
MObject budleDeform::aCurveRamp;

budleDeform::budleDeform(){

}


budleDeform::~budleDeform(){

}

void* budleDeform::creator(){
	return new budleDeform;
}


MStatus budleDeform::initialize(){
	MStatus stat;

	aCurveRamp = MRampAttribute::createCurveRamp("curveRamp", "cur");
	addAttribute(aCurveRamp);
	attributeAffects(aCurveRamp, outputGeom);

	MFnNumericAttribute nAttr;
	aBugleAmount = nAttr.create("amount", "amount", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	addAttribute(aBugleAmount);
	attributeAffects(aBugleAmount, outputGeom);

	aMagnitude = nAttr.create("magnitude", "magnitude", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	addAttribute(aMagnitude);
	attributeAffects(aMagnitude, outputGeom);

	aMean = nAttr.create("Mean", "Mean", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	addAttribute(aMean);
	attributeAffects(aMean, outputGeom);

	aVariance = nAttr.create("Variance", "Variance", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	addAttribute(aVariance);
	attributeAffects(aVariance, outputGeom);



	MFnMatrixAttribute matAttr;

	matrix = matAttr.create("deforSpace", "ds");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);


	MFnUnitAttribute unitFn;

	startDist = unitFn.create("startDist", "sd", MFnUnitAttribute::kDistance);
	unitFn.setDefault(MDistance(0.0, MDistance::uiUnit()));
	unitFn.setMin(MDistance(0.0, MDistance::uiUnit()));
	unitFn.setKeyable(true);


	endDist = unitFn.create("endDist", "ed", MFnUnitAttribute::kDistance);
	unitFn.setDefault(MDistance(3.0, MDistance::uiUnit()));
	unitFn.setMin(MDistance(0.0, MDistance::uiUnit()));
	unitFn.setKeyable(true);

	addAttribute( matrix);
	addAttribute(startDist);
	addAttribute(endDist);

	attributeAffects(startDist, outputGeom);
	attributeAffects(endDist, outputGeom);
	attributeAffects(matrix, outputGeom);


	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer bulgeMesh weights;");
	return MS::kSuccess;
}

MStatus budleDeform::accessoryNodeSetup( MDagModifier& cmd){

	MStatus stat;
	MObject LocObj = cmd.createNode("locator", MObject::kNullObj);
	cmd.renameNode(LocObj, "HandlebugleLoc");

	MFnDependencyNode locFn(LocObj);
	MObject attrMat = locFn.attribute("matrix");
	stat = cmd.connect(LocObj, attrMat, thisMObject(), matrix);
	return stat;
}

MObject &budleDeform::accessoryAttribute()const{
	return matrix;
}

MStatus budleDeform::deform(MDataBlock& block, MItGeometry& itgeo, const MMatrix& mat, unsigned int multiIndex){
	MStatus stat;

	MArrayDataHandle hInputs = block.outputArrayValue(input, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = hInputs.jumpToElement(multiIndex);
	MDataHandle hInputElement = hInputs.outputValue(&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oInputGeom = hInputElement.child(inputGeom).asMesh();

	MFnMesh fnMesh(oInputGeom, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MFloatVectorArray normals;
	fnMesh.getVertexNormals(false, normals);

	MDataHandle matData = block.inputValue(matrix);
	MMatrix matr = matData.asMatrix();
	MMatrix invMat = matr.inverse();
	MMatrix worldToLocalMatrix = mat.inverse();

	MDataHandle startDistHnd = block.inputValue(startDist);
	float startDistd = startDistHnd.asFloat();

	MDataHandle endDistHnd = block.inputValue(endDist);
	float endDistd = endDistHnd.asFloat();


	float Magnitude = block.inputValue(aMagnitude, &stat).asFloat();
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	float Mean = block.inputValue(aMean, &stat).asFloat();
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	float Variance = block.inputValue(aVariance, &stat).asFloat();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	float bugleAmount = block.inputValue(aBugleAmount).asFloat();
	float env = block.inputValue(envelope).asFloat();
	MPoint point;
	float w;


	MObject oThis = thisMObject();
	MRampAttribute curveAttribute(oThis, aCurveRamp, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	for(; !itgeo.isDone(); itgeo.next()){

		w = weightValue(block, multiIndex, itgeo.index());

		if( w == 0){
			continue;
		}

		point = itgeo.position();
		point *= mat;
		point *= invMat;

		if (MVector(point).length() < 1){
			float poimtPos = 1 - MVector(point).length(), curveRampValue;

			curveAttribute.getValueAtPosition(poimtPos, curveRampValue, &stat);

			point += normals[itgeo.index()] * curveRampValue * w * env * bugleAmount;
		}

		point *= matr;
		point *= worldToLocalMatrix;

		itgeo.setPosition(point);
	}


	return MS::kSuccess;

}
