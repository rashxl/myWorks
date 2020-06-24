/*
 * softClamp.cpp
 *
 *  Created on: Apr 18, 2017
 *      Author: rash
 */


#include "softClamp.h"
#ifndef PI
#define PI 3.14159265
#endif




// soft clamp
MObject softClamp::value;
MObject softClamp::limitA;
MObject softClamp::limitASoft;
MObject softClamp::limitB;
MObject softClamp::limitBSoft;
// soft max
MObject softClamp::valMax;
MObject softClamp::limMax;
MObject softClamp::softMax;
// soft min
MObject softClamp::valMin;
MObject softClamp::limMin;
MObject softClamp::softMin;

// clamp
MObject softClamp::valClamp;
MObject softClamp::limACalmp;
MObject softClamp::limBCalmp;

MObject softClamp::method;

MObject softClamp::result;




/// initialize math functions ////
float softnessMax(float val, float max, float softness);
float softnessMin(float val, float max, float softness);
float clamping(float val, float max, float min);

void * softClamp::creator(){
	return new softClamp;
}


MStatus softClamp::initialize(){
	MStatus stat;

	MFnNumericAttribute FnNumAttr;
	MFnEnumAttribute enumAttr;
	MRampAttribute rAttr;


	// method display
	method = enumAttr.create( "method", "method", 0 );
	enumAttr.setStorable(true);
	enumAttr.setKeyable(true);
	enumAttr.addField("SoftMax", 0) ;
	enumAttr.addField("SoftMin", 1) ;
	enumAttr.addField("clamp", 2) ;
	enumAttr.addField("SoftClamp", 3) ;
	stat = addAttribute(method);


	//############ soft max attribute ####################################
	valMax = FnNumAttr.create("valMax", "valMax", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(valMax);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limMax = FnNumAttr.create("limMax", "limMax", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limMax);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	softMax = FnNumAttr.create("softMax", "softMax", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(softMax);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ soft min attribute ####################################
	valMin = FnNumAttr.create("valMin", "valMin", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(valMin);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limMin = FnNumAttr.create("limMin", "limMin", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limMin);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	softMin = FnNumAttr.create("softMin", "softMin", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(softMin);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ clamp attribute ####################################
	valClamp = FnNumAttr.create("valClamp", "valClamp", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(valClamp);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limBCalmp = FnNumAttr.create("MinCalmp", "MinCalmp", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limBCalmp);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limACalmp = FnNumAttr.create("MaxCalmp", "MaxCalmp", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limACalmp);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ soft Clamp attribute ####################################
	value = FnNumAttr.create("value", "value", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(value);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limitA = FnNumAttr.create("limitA", "limitA", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limitA);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limitASoft = FnNumAttr.create("limitASoft", "limitASoft", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limitASoft);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limitB = FnNumAttr.create("limitB", "limitB", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limitB);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	limitBSoft = FnNumAttr.create("limitBSoft", "limitBSoft", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(limitBSoft);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ result attribute ####################################
	result = FnNumAttr.create("result", "result", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//########################## attr affects ##############################//
	stat = attributeAffects( value, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limitA, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limitASoft, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limitB, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limitBSoft, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( valMax, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limMax, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( softMax, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( valMin, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limMin, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( softMin, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( valClamp, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limACalmp, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( limBCalmp, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( method, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;
}



MStatus softClamp::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;


	if(plug == result){

		//######## get method ########################

		MDataHandle methodHandl = data.inputValue(method);
		MDataHandle outHandle = data.outputValue(result);

		if(methodHandl.asShort() == 0){
			//######## soft max compute ##################
			float inValMax = data.inputValue(valMax).asFloat();
			float inLimMax = data.inputValue(limMax).asFloat();
			float inSoftMax = data.inputValue(softMax).asFloat();

			float res = softnessMax(inValMax, inLimMax, inSoftMax);

			outHandle.set(res);
			outHandle.setClean();


		}else if(methodHandl.asShort() == 1){

			//######## soft min compute ##################

			float inValMin = data.inputValue(valMin).asFloat();
			float inLimMin = data.inputValue(limMin).asFloat();
			float inSoftMin = data.inputValue(softMin).asFloat();

			float res = softnessMin(inValMin, inLimMin, inSoftMin);
			outHandle.set(res);
			outHandle.setClean();

		}else if(methodHandl.asShort() == 2){

			//######## clamp compute ##################

			float inValCalmp = data.inputValue(valClamp).asFloat();
			float inLimMax = data.inputValue(limACalmp).asFloat();
			float inLimMin = data.inputValue(limBCalmp).asFloat();

			float resultClamp = clamping(inValCalmp, inLimMax, inLimMin);

			outHandle.set(resultClamp);
			outHandle.setClean();

		}else if(methodHandl.asShort() == 3){

			//######## soft clamp compute ##################

			float inValSoftClmp = data.inputValue(value).asFloat();
			float inLimA = data.inputValue(limitA).asFloat();
			float inLimASoft = data.inputValue(limitASoft).asFloat();
			float inLimB = data.inputValue(limitB).asFloat();
			float inLimBSoft = data.inputValue(limitBSoft).asFloat();
			float result;

			if(inLimB > inLimA){

				float softMinRes = softnessMin(inValSoftClmp, inLimA, inLimASoft);
				float softMaxRe = softnessMax(softMinRes,inLimB, inLimBSoft );
				result = clamping(softMaxRe, inLimB, inLimA);

			}else{

				float softMinRes = softnessMin(inValSoftClmp, inLimB, inLimBSoft);
				float softMaxRe = softnessMax(softMinRes,inLimA, inLimASoft );
				result = clamping(softMaxRe, inLimB, inLimA);
			}

			outHandle.set(result);
			outHandle.setClean();
		}

		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

	}else{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}
