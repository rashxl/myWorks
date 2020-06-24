/*
 * crvWire.cpp
 *
 *  Created on: Aug 1, 2017
 *      Author: rash
 */

/*
 * curveWave.cpp
 *
 *  Created on: Apr 2, 2017
 *      Author: rash
 */

#include "crvWire.h"
#define min
#define max

MObject crvWire::mesh;
MObject crvWire::curve;
MObject crvWire::scale;
MObject crvWire::outMesh;
MObject crvWire::env;
MObject crvWire::waveFallOff;
MObject crvWire::scaleFallOff;
MObject crvWire::num;
MObject crvWire::offset;
MObject crvWire::shift;
MObject crvWire::amount;
MObject crvWire::len;

MTypeId crvWire::id(0x00330628);

void * crvWire::creator(){

	return new crvWire;
}


MStatus crvWire::initialize(){


	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;

	/////// wave ramp /////////
	waveFallOff = MRampAttribute::createCurveRamp("waveFalloff", "waveFalloff");
	addAttribute(waveFallOff);

	/////// scale ramp /////////
	scaleFallOff = MRampAttribute::createCurveRamp("scaleFalloff", "scaleFalloff");
	addAttribute(scaleFallOff);

	////// input curve attribute ////////
	curve = attrType.create("inCurve", "inCurve", MFnNurbsCurveData::kNurbsCurve, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
    CHECK_MSTATUS ( attrType.setStorable( true ) );
	stat = addAttribute(curve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute inMesh ////
	mesh = attrType.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(mesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// shift attribute ////////
	len = attNum.create("length", "length", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(1.0));
	stat = addAttribute(len);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// shift attribute ////////
	shift = attNum.create("shift", "shift", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0.0));
	stat = addAttribute(shift);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// shift attribute ////////
	amount = attNum.create("amount", "amount", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0.0));
	stat = addAttribute(amount);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// env attribute ////////
	env = attNum.create("env", "env", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0.0));
	CHECK_MSTATUS(attNum.setMax(1.0));
	stat = addAttribute(env);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// env attribute ////////
	offset = attNum.create("offset", "offset", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
//	CHECK_MSTATUS(attNum.setMin(0.0));
//	CHECK_MSTATUS(attNum.setMax(1.0));
	stat = addAttribute(offset);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// scale ////
	scale = attNum.create("scale", "scale", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(scale);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// number ////
	num = attNum.create("number", "number", MFnNumericData::kInt, 2.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attNum.setMin(2);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(num);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// attribute outMesh ////
	outMesh = attrType.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(false));
	stat = addAttribute(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( offset, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( waveFallOff, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( scaleFallOff, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( mesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( curve, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( env, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( num, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( scale, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( shift, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( amount, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( len, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;


}

MStatus crvWire::compute(const MPlug& plug, MDataBlock & data){
	MStatus stat;

	if(plug == outMesh){

		MThreadUtils::syncNumOpenMPThreads();

		#ifndef _DEBUG
		#ifdef _OPENMP
		omp_set_num_threads(omp_get_num_procs());
		#endif
		#endif

		//// initialize ramp ///
		MRampAttribute curveAttribute(thisMObject(), waveFallOff, &stat);
		MRampAttribute scaleAttribute(thisMObject(), scaleFallOff, &stat);

		// get curve //
		MDataHandle inCurveHandle = data.inputValue(curve, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject inCrvObject = inCurveHandle.asNurbsCurve();
		MFnNurbsCurve CurveFn(inCrvObject);

		/// get mesh ///
		MDataHandle inPolyDataHandle = data.inputValue(mesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();
		MFnMesh fnInMesh(moMesh);

		/// copy mesh
        MFnMeshData meshDataFn;
        MFnMesh inputMesh(moMesh);
        MObject newMeshObj = meshDataFn.create();
        MFnMesh newMeshFn;
        newMeshFn.copy( moMesh, newMeshObj );

		/// get noraml
		MFloatVectorArray normals;
		fnInMesh.getVertexNormals(false, normals);
		/// get points
		MPointArray pts;
		stat = fnInMesh.getPoints(pts);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get scale //
		double scaleFactor = data.inputValue(scale, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get offset //
		double offsetFactor = data.inputValue(offset, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get shift //
		double shiftFactor = data.inputValue(shift, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get amount //
		double amountFactor = data.inputValue(amount, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get amount //
		double lenFactor = data.inputValue(len, &stat).asDouble();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// get number//
		int number = data.inputValue(num, &stat).asInt();

		/// env ///
		MDataHandle inEnv = data.inputValue(env, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double envDouble = inEnv.asDouble();

		/// out mesh ///
		MDataHandle outPolyDataHandle = data.outputValue(outMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		double lenCrv = CurveFn.length();
		double incLen = lenCrv/(double(number)-1.0);

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for (int i=0; i<number; ++i){

			double dis = fmod(fmod((incLen*amountFactor) * i + offsetFactor, lenCrv-0.1), lenCrv);
	        //double param = max( min( CurveFn.findParamFromLength( dist ), CurveFn.findParamFromLength( lenCrv ) ), 0.0);


			//double dis = fmod(i*incLen+offsetFactor, lenCrv), param;
			double param = CurveFn.findParamFromLength(dis);
			MPoint ptcrv;
			CurveFn.getPointAtParam(param+shiftFactor, ptcrv, MSpace::kObject);

			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			#endif
			for (int j=0; j<pts.length(); ++j){
				MVector vec = ptcrv - pts[j];
				if(vec.length() < scaleFactor){
					float pos = vec.length()/scaleFactor, val;
					curveAttribute.getValueAtPosition(pos, val, &stat);
					CHECK_MSTATUS_AND_RETURN_IT(stat);
					pts[j] += normals[j]*val*lenFactor*envDouble;
				}
			}

		}
		stat = newMeshFn.setPoints(pts);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		outPolyDataHandle.set(newMeshObj);
		data.setClean(plug);

	}else{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}




