#include "rampNode.h"


MObject rampNode::inDoubleArray;
MObject rampNode::inFloatArray;
MObject rampNode::inFloat;
MObject rampNode::aRamp;
MObject rampNode::oldMin;
MObject rampNode::oldMax;
MObject rampNode::Min;
MObject rampNode::Max;
MObject rampNode::outValue;
MObject rampNode::outFloat;





void * rampNode::creator(){
	return new rampNode;
}


MStatus rampNode::initialize(){

	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;

	//// in Array float ////
	inFloatArray = attrType.create("floatMap", "floatMap", MFnData::kFloatArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(inFloatArray);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// in Array double ////
	inDoubleArray = attrType.create("doubleMap", "doubleMap", MFnData::kDoubleArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(inDoubleArray);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// curve ramp ///
	aRamp = MRampAttribute::createCurveRamp("Ramp", "Ramp");
	stat = addAttribute(aRamp);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// oldMin attribute ////////
	oldMin = attNum.create("oldMin", "oldMin", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(oldMin);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// oldMax attribute ////////
	oldMax = attNum.create("oldMax", "oldMax", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(oldMax);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// Min attribute ////////
	Min = attNum.create("Min", "Min", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(Min);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// Max attribute ////////
	Max = attNum.create("Max", "Max", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(Max);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// out Array double ////
	outValue = attrType.create("outValDbl", "outValDbl", MFnData::kDoubleArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// out Array double ////
	outFloat = attrType.create("outValFl", "outValFl", MFnData::kFloatArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inDoubleArray, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aRamp, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( oldMin, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( oldMax, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( Min, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( Max, outValue);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inDoubleArray, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aRamp, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( oldMin, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( oldMax, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( Min, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( Max, outFloat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;
}


MStatus rampNode::compute(const MPlug & plug, MDataBlock & data){
	MStatus stat;

	if(plug == outValue){

		//// initialize ramp ///
		MRampAttribute curveAttribute(thisMObject(), aRamp, &stat);

		//// in double array attr ///
		MObject MdblArray = data.inputValue(inDoubleArray, &stat).data();
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MFnDoubleArrayData fnDoubleArray(MdblArray, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MDoubleArray inMDoubleArray = fnDoubleArray.array();
		MDoubleArray outMDoubleArray;
		MFloatArray outMFloatArray;

		double inOldMin = data.inputValue(oldMin, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double inOldMax = data.inputValue(oldMax, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double inMin = data.inputValue(Min, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double inMax = data.inputValue(Max, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		int len = inMDoubleArray.length();

		for (int i = 0; i < len; i++){


			double val = inMin + (((inMDoubleArray[i] - inOldMin) / (inOldMax - inOldMin))*(inMax - inMin));
			outMDoubleArray.append(val);
			outMFloatArray.append(float(val));

			/*
			float valRamp;
			float inRamp = float(inMDoubleArray[i]);
			curveAttribute.getValueAtPosition(inRamp, valRamp);
			inMDoubleArray[i] = valRamp;
			*/

		}


		//// out double array attr ///
		MDataHandle outMDoubleMapHandle = data.outputValue(outValue, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		//// out double array attr ///
		MDataHandle outMapFlHandle = data.outputValue(outFloat, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		MFnDoubleArrayData fnOutArrayData;

		MFnFloatArrayData fnOutFlArrayData;

		MObject OutMObjDoubleArray = fnOutArrayData.create(outMDoubleArray, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MObject OutMObjFloatArray = fnOutFlArrayData.create(outMFloatArray, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		stat = outMDoubleMapHandle.set(OutMObjDoubleArray);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		stat = outMapFlHandle.set(OutMObjFloatArray);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


	}else{
		//MGlobal::displayInfo("#### FAILD Ramp!!!");
		return MS::kUnknownParameter;
	}
	return MS::kSuccess;
}
