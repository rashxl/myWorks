/*
 * mathNode.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: rash
 */




#include "mathNode.h"




MObject mathNode::PowVal;
MObject mathNode::PowBase;

MObject mathNode::LogVal;
MObject mathNode::LogBase;

MObject mathNode::ExpVal;

MObject mathNode::Num;
MObject mathNode::Denom;

MObject mathNode::absVal;

MObject mathNode::aSin;

MObject mathNode::fcurve;
MObject mathNode::inVal;

MObject mathNode::method;

MObject mathNode::result;



MStatus mathNode::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;

	if(plug == result){
		/// fcurve ramp
		MRampAttribute fcurveRamp(thisMObject(), fcurve, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		//######## get method ########################

		MDataHandle methodHandl = data.inputValue(method);
		MDataHandle outHandle = data.outputValue(result);

		if(methodHandl.asShort() == 0){

			//######## power compute ##################

			float power = data.inputValue(PowVal).asFloat();
			float exBase = data.inputValue(PowBase).asFloat();

			float resultPow = pow(exBase, power);


			outHandle.set(resultPow);
			outHandle.setClean();


		}else if(methodHandl.asShort() == 1){

			//######## logarithm compute ##################

			float val = data.inputValue(LogVal).asFloat();
			float base = data.inputValue(LogBase).asFloat();


			float resultLog = log(val)/log(base);

			outHandle.set(resultLog);
			outHandle.setClean();


		}else if(methodHandl.asShort() == 2){

			//######## exp compute ##################

			float exponent = data.inputValue(ExpVal).asFloat();

			float resultExp = exp(exponent);


			outHandle.set(resultExp);
			outHandle.setClean();

		}else if(methodHandl.asShort() == 3){

			//######## modulo compute ##################

			float ModNum = data.inputValue(Num).asFloat();
			float ModDenom = data.inputValue(Denom).asFloat();

			float resultMod = fmod(ModNum, ModDenom);


			outHandle.set(resultMod);
			outHandle.setClean();

		}else if(methodHandl.asShort() == 4){

			//######## abs compute ##################

			float absolute = data.inputValue(absVal).asFloat();

			float resultAbs = std::abs(absolute);


			outHandle.set(resultAbs);
			outHandle.setClean();

		}else if(methodHandl.asShort() == 5){

			//######## sin compute ##################

			float inValue = data.inputValue(aSin).asFloat();

			float resultVal;

			resultVal = sin(inValue);

			outHandle.set(resultVal);
			outHandle.setClean();



		}else if(methodHandl.asShort() == 6){

			//######## fcurve compute ##################

			float inValue = data.inputValue(inVal).asFloat();

			float resultVal;

			fcurveRamp.getValueAtPosition(inValue, resultVal);


			outHandle.set(resultVal);
			outHandle.setClean();

		}

		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

	}else{
		return MS::kUnknownParameter;
	}
	return MS::kSuccess;
}


void* mathNode::creator(){
	return new mathNode;
}


MStatus mathNode::initialize(){
	MStatus stat;

	MFnNumericAttribute FnNumAttr;
	MFnEnumAttribute enumAttr;
	MRampAttribute rAttr;


	// color display
	method = enumAttr.create( "method", "method", 0 );
	enumAttr.setStorable(true);
	enumAttr.setKeyable(true);
	enumAttr.addField("pow", 0) ;
	enumAttr.addField("log", 1) ;
	enumAttr.addField("exp", 2) ;
	enumAttr.addField("mod", 3) ;
	enumAttr.addField("abs", 4) ;
	enumAttr.addField("sin", 5) ;
	enumAttr.addField("fcrv", 6) ;
	stat = addAttribute(method);


	//############ power attribute ####################################
	PowVal = FnNumAttr.create("PowVal", "PowVal", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(PowVal);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	PowBase = FnNumAttr.create("PowBase", "PowBase", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(PowBase);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//############ logarithm attribute ####################################
	LogVal = FnNumAttr.create("LogVal", "LogVal", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(LogVal);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	LogBase = FnNumAttr.create("LogBase", "LogBase", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(LogBase);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//############ exponential func e~2.718 attribute ####################################
	ExpVal = FnNumAttr.create("ExpVal", "ExpVal", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(ExpVal);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ modulo attribute ####################################
	Num = FnNumAttr.create("Num", "Num", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(Num);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	Denom = FnNumAttr.create("Denom", "Denom", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(Denom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ absolute value attribute ####################################
	absVal = FnNumAttr.create("abs", "abs", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(absVal);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//############ sin attribute ####################################
	aSin = FnNumAttr.create("sin", "sin", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(aSin);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//############ curve function attribute ####################################
	inVal = FnNumAttr.create("inVal", "inVal", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(FnNumAttr.setKeyable(true));
	stat = addAttribute(inVal);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	fcurve = rAttr.createCurveRamp("fcurve", "fcrv");
	stat = addAttribute(fcurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//############ result attribute ####################################
	result = FnNumAttr.create("result", "result", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//########################## attr affects ##############################//
	stat = attributeAffects( LogVal, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( LogBase, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( PowVal, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( PowBase, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( ExpVal, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( Num, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( Denom, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( absVal, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( aSin, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inVal, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( fcurve, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( method, result);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	return MS::kSuccess;
}
