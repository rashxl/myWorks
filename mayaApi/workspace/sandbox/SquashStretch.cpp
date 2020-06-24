/*
 * SquashStretch.cpp
 *
 *  Created on: Apr 7, 2017
 *      Author: rash
 */

#include "SquashStretch.h"


MObject squashStretch::root;
MObject squashStretch::target;
MObject squashStretch::factor;
MObject squashStretch::squash;
MObject squashStretch::stretch;
MObject squashStretch::bend;
MObject squashStretch::compute;
MObject squashStretch::StrScale;
MObject squashStretch::SqhScale;


#define PI 3.14159265

squashStretch::squashStretch(){

}

squashStretch::~squashStretch(){

}

void * squashStretch::creator(){

	return new squashStretch();
}

MObject& squashStretch::accessoryAttribute ()const{
	return target;
}


//###### post construction initial ramp attributes ! important it not good choose!
//void postConstructor_initialise_ramp_curve( MObject node, MObject ramp, int index, float position, float value, int interpolation);
//
//
//void squashStretch::postConstructor(){
//
//
//	postConstructor_initialise_ramp_curve( squashStretch::thisMObject(), squash, 0, 0.0f, 1.0f, 1 );
//	postConstructor_initialise_ramp_curve( squashStretch::thisMObject(), stretch, 0, 0.0f, 1.0f, 1 );
//	postConstructor_initialise_ramp_curve( squashStretch::thisMObject(), bend, 0, 0.0f, 1.0f, 1 );
//
//
//
//}
//
//
//
//void postConstructor_initialise_ramp_curve( MObject node, MObject ramp, int index, float position, float value, int interpolation)
//{
//	MStatus stat;
//
//	MPlug rampPlug( node, ramp );
//	MGlobal::displayInfo("postConstructor_initialise_ramp_curve##");
//
//	MPlug elementPlug = rampPlug.elementByLogicalIndex( index, &stat );
//	MPlug positionPlug = elementPlug.child(0, &stat);
//	stat = positionPlug.setFloat(position);
//
//	MPlug valuePlug = elementPlug.child(1);
//	stat = valuePlug.setFloat(value);
//
//	MPlug interpPlug = elementPlug.child(2);
//	interpPlug.setInt(interpolation);
//
//}




MStatus squashStretch::accessoryNodeSetup(MDagModifier& mDagMod){

	////// create spaceLocator ///////
	MStatus stat;
	MObject oRoot = mDagMod.createNode("locator", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oTarget = mDagMod.createNode("locator", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = mDagMod.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// Root world Matrix attribute ////
	MFnDagNode fnRootLoc(oRoot, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aRootTransAttr = fnRootLoc.attribute("worldMatrix", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	//// connect world mat of root to node
	stat = mDagMod.connect(oRoot, aRootTransAttr, thisMObject(), root);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// Target world Matrix attribute ////
	MFnDagNode fnTargetLoc(oTarget, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aTargetTransAttr = fnTargetLoc.attribute("worldMatrix", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	//// connect world mat of root to node
	stat = mDagMod.connect(oTarget, aTargetTransAttr, thisMObject(), target);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	/*
	/// getting position max of vtx
	MGlobal::displayInfo(MString("here!"));
	MFnDependencyNode FnDepNode(thisMObject());
	MPlug plElemNodeArray, plElemNode;
	plElemNodeArray = FnDepNode.findPlug("outputGeometry", true, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	plElemNode = plElemNodeArray.elementByPhysicalIndex(0);
	MGlobal::displayInfo(MString("here2!"));
	MObject moMesh = plElemNode.asMObject();
	MGlobal::displayInfo(MString("here3!"));
	MFnMesh fnMesh(moMesh);
	MPointArray points;
	stat = fnMesh.getPoints(points);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	double min = 0.0001;
	for(int i=0; i<points.length(); ++i){
		if(min<points[i].y){
			min = points[i].y;
		}
	}
	stat = MGlobal::executeCommand("setAttr "+fnTargetLoc.name()+".translateY "+(min+(min/3))+";");
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	*/


	//// rename
	mDagMod.renameNode(oRoot, "loc_Root_ssb#");
	mDagMod.renameNode(oTarget, "loc_Target_ssb#");

	mDagMod.doIt();

	squashStretch::firstCall = true;

	return stat;

}


MStatus squashStretch::initialize(){

	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;
	MRampAttribute rAttr;
	MFnMatrixAttribute matAttr;


	////// root Mat attribute ////////
	root = matAttr.create("rootMat", "rmat");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	stat = addAttribute(root);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// target Mat attribute ////////
	target = matAttr.create("targetMat", "tmat");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	stat = addAttribute(target);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// factor attribute ////////
	factor = attNum.create("factor", "factor", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0.0));
	stat = addAttribute(factor);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// scale stretch attribute ////////
	StrScale = attNum.create("stretch_scale", "StrScale", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(StrScale);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// scale squash attribute ////////
	SqhScale = attNum.create("squash_scale", "SqhScale", MFnNumericData::kDouble, 2.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(SqhScale);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// factor attribute ////////
	compute = attNum.create("compute", "compute", MFnNumericData::kBoolean, 0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	stat = addAttribute(compute);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// squash attribute ////////
	squash = rAttr.createCurveRamp("squash", "squ");
	stat = addAttribute(squash);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// stretch attribute ////////
	stretch = rAttr.createCurveRamp("stretch", "str");
	stat = addAttribute(stretch);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// bend attribute ////////
	bend = rAttr.createCurveRamp("bend", "bnd");
	stat = addAttribute(bend);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// effects ////////
	stat = attributeAffects(root, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(target, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(factor, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(squash, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(stretch, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(bend, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(compute, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(StrScale, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(SqhScale, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///// Make deformer weights paintable ////////
	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer squashStretch weights;");


	return MS::kSuccess;
}


MStatus squashStretch::deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix& localToWorldMat, unsigned int indexGeo){

	MStatus stat;


	bool com = block.inputValue(compute).asBool();

	//######################### ramp #############################
	/// stretch
	MRampAttribute stretchRamp(thisMObject(), stretch, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	/// squash
	MRampAttribute squashRamp(thisMObject(), squash, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	/// squash
	MRampAttribute bendRamp(thisMObject(), bend, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	if(firstCall){


		MFloatArray StrPos, SqhPos, BndPos; // position
		MFloatArray StrVal, SqhVal, BndVal; // values
		MIntArray StrInter, SqhInter, BndInter; // interpolations

		/// stretch ///
		StrPos.append(0.0);StrPos.append(1.0);StrPos.append(0.5);
		StrVal.append(1.0);StrVal.append(1.0);StrVal.append(0.0);
		StrInter.append(MRampAttribute::kSmooth);StrInter.append(MRampAttribute::kSmooth);StrInter.append(MRampAttribute::kSmooth);
		stretchRamp.addEntries(StrPos, StrVal, StrInter);


		/// squash ///
		SqhPos.append(0.15);SqhPos.append(0.195);SqhPos.append(0.25);
		SqhPos.append(0.33);SqhPos.append(0.4);SqhPos.append(0.5);
		SqhPos.append(0.6);SqhPos.append(0.66);SqhPos.append(0.75);
		SqhPos.append(0.805);SqhPos.append(0.85);
		SqhVal.append(0.0);SqhVal.append(0.32);SqhVal.append(0.64);
		SqhVal.append(0.87);SqhVal.append(0.97);SqhVal.append(1.0);
		SqhVal.append(0.97);SqhVal.append(0.87);SqhVal.append(0.64);
		SqhVal.append(0.32);SqhVal.append(0.0);
		SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);
		SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);
		SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);
		SqhInter.append(MRampAttribute::kLinear);SqhInter.append(MRampAttribute::kLinear);
		squashRamp.addEntries(SqhPos, SqhVal, SqhInter);

		/// stretch ///
		BndPos.append(0.0);BndPos.append(1.0);
		BndVal.append(0.0);BndVal.append(1.0);
		BndInter.append(MRampAttribute::kSmooth);BndInter.append(MRampAttribute::kSmooth);
		bendRamp.addEntries(BndPos, BndVal, BndInter);

	}


	if(com){

		//######################### matrix#############################
		//// get world matrix from root /////
		MDataHandle matRootHandle = block.inputValue(root);
		MMatrix matRoot = matRootHandle.asMatrix();

		//// get world matrix from target /////
		MDataHandle matTargetHandle = block.inputValue(target);
		MMatrix matTarget = matTargetHandle.asMatrix();

		matTarget = matTarget * matRoot.inverse();
		MVector targetVector;
		targetVector.x = matTarget[3][0];
		targetVector.y = matTarget[3][1];
		targetVector.z = matTarget[3][2];


		//// initialize  ///
		if(firstCall){

			fixdisY = targetVector.y;

			firstCall = false;

		}

		double targetYdis = targetVector.y / fixdisY;

		targetYdis = std::abs(targetYdis);
		double targetInv = targetYdis - 1.0;
		double t = std::abs(targetInv)*block.inputValue(factor).asDouble();

		//// get attribute envelope ////
		float env = block.inputValue(envelope).asFloat();
		if(env == 0.0) return MS::kSuccess;


		MPointArray allPoints; allPoints.clear();

		for(; !itGeo.isDone(); itGeo.next()){

			/// get vertex weight
			float weight = weightValue(block, indexGeo, itGeo.index());

			MPoint pt = itGeo.position();

			pt *= localToWorldMat;
			pt *= matRoot.inverse();

			float vtxY = std::abs((float)pt.y);
			// rescale pos Y //
			vtxY = 0.0 + (((vtxY - 0.0) / (std::abs((float)fixdisY) - 0.0))*(1.0 - 0.0));

			/// stretch
			if(targetInv > 0){

				float strVal;
				stretchRamp.getValueAtPosition(vtxY, strVal);

				//// x ////
				float ptStrX = strVal * pt.x;
				pt.x = (pt.x+t*((ptStrX*block.inputValue(StrScale).asDouble()) - pt.x))*weight+(1-weight)*pt.x;

				//// y ////
				float ptStrZ = strVal * pt.z;
				pt.z = (pt.z+t*((ptStrZ*block.inputValue(StrScale).asDouble()) - pt.z))*weight+(1-weight)*pt.z;

			}/// squash
			else{

				float sqhVal;
				squashRamp.getValueAtPosition(vtxY, sqhVal);

				//// x ////
				float ptSqhX = sqhVal * pt.x;
				pt.x = (pt.x+t*((ptSqhX*block.inputValue(SqhScale).asDouble()) - pt.x))*weight+(1-weight)*pt.x;

				//// y ////
				float ptSqhZ = sqhVal * pt.z;
				pt.z = (pt.z+t*((ptSqhZ*block.inputValue(SqhScale).asDouble()) - pt.z))*weight+(1-weight)*pt.z;
			}

			pt.y= pt.y * targetYdis;


			//####### bend ###########
			float reSclval = 0.0 + (((pt.y - 0.0) / ((float)targetVector.y - 0.0))*(1.0 - 0.0));
			float bndVal;
			bendRamp.getValueAtPosition(reSclval, bndVal);

			//// bend Z axis
			double arcTz = -atan(targetVector.x / targetVector.y);
			double anglZ = bndVal*arcTz;
			double arcTx = atan(targetVector.z / targetVector.y);
			double anglX = bndVal*arcTx;
			MVector vecA(pt), vecB;
			vecB = vecA.rotateBy(MVector::kZaxis, anglZ);
			vecA = vecB.rotateBy(MVector::kXaxis, anglX);
			pt = pt*(1-weight)+vecA*weight;

			//// bend X axis
//			//double arcT = -atan(targetVector.x / targetVector.y) * 180 /PI;
//			double arcTx = atan(targetVector.z / targetVector.y);
//			double anglX = bndVal*arcTx;
//			MQuaternion qRotX;
//			MVector vecX(0,0,1);
//			qRotX.setAxisAngle(vecX, anglX);

//			MVector vecPoint;
//			vecPoint.x = pt.x;
//			vecPoint.y = pt.y;
//			vecPoint.z = pt.z;
//			vecPoint.rotateBy(qRotZ);
//			pt.x = vecPoint.x;
//			pt.y = vecPoint.y;
//			pt.z = vecPoint.z;


			pt *= matRoot;
			pt *= localToWorldMat.inverse();


			allPoints.append(pt);

		}

		itGeo.setAllPositions(allPoints);
	}

	return MS::kSuccess;
}
