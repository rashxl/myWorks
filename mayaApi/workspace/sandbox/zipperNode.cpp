#include "zipperNode.h"


MObject zipperNode::amount;
MObject zipperNode::scale;
MObject zipperNode::bias;
MObject zipperNode::zip;
MObject zipperNode::inCurve;
MObject zipperNode::rCurveRamp;
MObject zipperNode::gCurveRamp;
MObject zipperNode::outCurve;


void * zipperNode::creator(){
	return new zipperNode;
}

//zipper plug-in
MStatus zipperNode::initialize(){

	MStatus stat;
	MFnNumericAttribute fnNumAttr;
	MFnTypedAttribute MfnTypeAttr;

	rCurveRamp = MRampAttribute::createCurveRamp("curveRampScale", "curveRampScale");
	addAttribute(rCurveRamp);

	gCurveRamp = MRampAttribute::createCurveRamp("curveRampSpeed", "curveRampSpeed");
	addAttribute(gCurveRamp);


	amount = fnNumAttr.create("amount", "amount",  MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	fnNumAttr.setDefault(1.0);
	fnNumAttr.setMin(0.0);
	fnNumAttr.setMax(1.0);
	stat = addAttribute(amount);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	scale = fnNumAttr.create("scale", "scale",  MFnNumericData::kInt, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	fnNumAttr.setDefault(2);
	fnNumAttr.setMin(0);
	fnNumAttr.setMax(100);
	stat = addAttribute(scale);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	bias = fnNumAttr.create("bias", "bias",  MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	fnNumAttr.setDefault(0.5);
	fnNumAttr.setMin(0);
	fnNumAttr.setMax(1.0);
	stat = addAttribute(bias);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	zip = fnNumAttr.create("zip", "zip",  MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	fnNumAttr.setMin(0);
	fnNumAttr.setMax(2.0);
	stat = addAttribute(zip);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	inCurve = MfnTypeAttr.create("inCurve", "inCurve", MFnNurbsCurveData::kNurbsCurve, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    //CHECK_MSTATUS ( MfnTypeAttr.setReadable( false ) );
    //CHECK_MSTATUS ( MfnTypeAttr.setWritable( true ) );
    CHECK_MSTATUS ( MfnTypeAttr.setStorable( true ) );
	stat = addAttribute(inCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	outCurve = MfnTypeAttr.create("outCurves", "outCurves", MFnNurbsCurveData::kNurbsCurve, &stat);
    CHECK_MSTATUS ( MfnTypeAttr.setArray( true ) );
	//CHECK_MSTATUS ( MfnTypeAttr.setStorable( false ) );
    //CHECK_MSTATUS ( MfnTypeAttr.setReadable( true ) );
    //CHECK_MSTATUS ( MfnTypeAttr.setWritable( false ) );
	CHECK_MSTATUS ( MfnTypeAttr.setStorable( true ) )
	CHECK_MSTATUS ( MfnTypeAttr.setUsesArrayDataBuilder( true ) );
    stat = addAttribute( outCurve );
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( amount, outCurve );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( scale, outCurve );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( inCurve, outCurve );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	attributeAffects(rCurveRamp, outCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	attributeAffects(bias, outCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	attributeAffects(gCurveRamp, outCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	attributeAffects(zip, outCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return stat;

}

MStatus zipperNode::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;

	if(plug == outCurve){
		MDataHandle amountHandle = data.inputValue(amount, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		float rAmount = amountHandle.asFloat();

		int rScale  = data.inputValue(scale, &stat).asInt();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MDataHandle biasHandle = data.inputValue(bias, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		float rBias = biasHandle.asFloat();

		MDataHandle zipHandle = data.inputValue(zip, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		float rZip = zipHandle.asFloat();

		MDataHandle inCurveHandle = data.inputValue(inCurve, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject inCrvObject = inCurveHandle.asNurbsCurve();

		MArrayDataHandle outputArrayCurvesHandle = data.outputValue(outCurve, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		MFnNurbsCurve CurveFn(inCrvObject);
		MPointArray cvs, cvsMid, CurveUpperCVs, CurveLowerCVs;
		CurveFn.getCVs(cvs, MSpace::kWorld);


		MObject oThis = thisMObject();
		MRampAttribute crvAttribute(oThis, rCurveRamp, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MRampAttribute crvSpeddAttribute(oThis, gCurveRamp, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MArrayDataBuilder CurvesBuilder(outCurve, 2, &stat);

		float len = (float)cvs.length(), ValueRamp;
		float step = 1.0f/(len - 1.0);
		float u = (len - 1) * rZip;
		int ulower = (int)floor(u);
		int uTop = (int)ceil(u);
		MPoint mPoint;
		MPointArray cvsPoints;
		cvsPoints.setLength(len);
		float y;
		float sideY;

		for (unsigned int num = 0; num < 2; num++){

			MDataHandle outCuveHandle = CurvesBuilder.addElement(num);

			MFnNurbsCurveData newCurveDataFn;

			MObject newCurveData = newCurveDataFn.create();

			MFnNurbsCurve newCurveFn;

			newCurveFn.copy(inCrvObject, newCurveData);

			newCurveFn.setObject(newCurveData);

			for (unsigned int i =0; i < cvs.length(); i++)
			{

					float d;

					crvAttribute.getValueAtPosition(i*step, d, &stat);

					if(rZip < d){

						y = 0;

					}else{

						y = 0.5*((rZip - d)*((1-0.5)/((d+step)-d)+0.5)*rZip);


					}
					crvSpeddAttribute.getValueAtPosition(y, sideY, &stat);

					if(num > 0){
						mPoint.y = (cvs[i].y - (rScale*(sideY*rBias)));
					}else{
						mPoint.y = (cvs[i].y + (rScale*(sideY*(1-rBias))));
					}

					//if(rZip < d){

						//mPoint.y = cvs[i].y;

					//}else{

						//y = 0.5*((rZip - d)*((1-0.5)/((d+step)-d)+0.5)*rZip);
						//crvSpeddAttribute.getValueAtPosition(y, sideY, &stat);
						//mPoint.y = mPoint.y * sideY *rAmount;
					//}

					mPoint.x = cvs[i].x;
					mPoint.z = cvs[i].z;
					cvsPoints[i] = mPoint;


			}

			newCurveFn.setCVs(cvsPoints);
			newCurveFn.updateCurve();

			stat = outCuveHandle.set(newCurveData);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

		}

		stat = outputArrayCurvesHandle.set(CurvesBuilder);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		stat = outputArrayCurvesHandle.setAllClean();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		data.setClean(plug);

	}
	else {
		return MS::kUnknownParameter;
	}

	return stat;
}
