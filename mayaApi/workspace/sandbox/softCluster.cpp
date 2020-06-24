/*
 * softCluster.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: rash
 */


#include "softCluster.h"


MObject softCluster::aFalloff;
MObject softCluster::aMatrix;
MObject softCluster::bindMatrix;
MObject softCluster::bindUpdate;
MObject softCluster::push;

softCluster::softCluster(){
}

softCluster::~softCluster(){

}

MObject& softCluster::accessoryAttribute() const{
	 return aMatrix;
}

MStatus softCluster::accessoryNodeSetup(MDagModifier& mDagMod){

	MStatus stat;

	////// create spaceLocator ///////
	MObject oTransform = mDagMod.createNode("locator", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oSphere = mDagMod.createNode("implicitSphere", oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = mDagMod.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// Connect CTRL.tr --> node ////
	MFnDagNode fnCtrlLoc(oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aCtrlLocMatAttr = fnCtrlLoc.attribute("worldMatrix", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = mDagMod.connect(oTransform, aCtrlLocMatAttr, thisMObject(), aMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	softCluster::firstCall = true;


	return MS::kSuccess;

}

MStatus softCluster::deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix & localToWorldMat, unsigned int indexGeo){

	MStatus stat;

	//// get world matrix from locator /////
	MDataHandle matData = block.inputValue(aMatrix);
	MMatrix matr = matData.asMatrix();

	MMatrix matrInv = matr.inverse();

	/// get bindMatrix attr ////
	MDataHandle binMatHandle = block.inputValue(bindMatrix);


	//// aFalloff attribute ////
	float falloff;
	MRampAttribute hFalloff(thisMObject(), aFalloff, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// initialize aFalloff ///
	if(firstCall){
		/// aFalloff ///
		MFloatArray a1; // position
		MFloatArray b1; // values
		MIntArray c1; // interpolations

		a1.append(0.0);
		a1.append(1.0);

		b1.append(1.0);
		b1.append(0.0);

		c1.append(MRampAttribute::kSmooth);
		c1.append(MRampAttribute::kSmooth);


		hFalloff.addEntries(a1, b1, c1);

		/// get bindMatr //
		binMatHandle.set(matrInv);
		firstCall = false;
	}

	bool update = block.inputValue(bindUpdate).asBool();

	if (update){
		binMatHandle.set(matrInv);
		binMatrix = binMatHandle.asMatrix();
	}else{
		binMatrix = binMatHandle.asMatrix();
	}


	//// get attribute envelope ////
	float env = block.inputValue(envelope).asFloat();
	if(env == 0.0) return MS::kSuccess;

	/// calculate translation constraint   -- get mesh
	MArrayDataHandle hInputs = block.outputArrayValue(input, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = hInputs.jumpToElement(indexGeo);
	MDataHandle hInputElement = hInputs.outputValue(&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oInputGeom = hInputElement.child(inputGeom).asMesh();
	MFnMesh fnMesh(oInputGeom);

	/////// get normals
	MFloatVectorArray normals;
	fnMesh.getVertexNormals(false, normals);

	/// save point position to be set after iterator is done
	MPointArray paAllPoints;

	///get push
	bool p = block.inputValue(push).asBool();

	if(p){

		MMatrix matrixInverse = localToWorldMat.inverse();

		for(; !itGeo.isDone(); itGeo.next()){

			MPoint skinned = itGeo.position();

			skinned *= localToWorldMat;
			skinned *= matrInv;

			/// get vertex weight
			float weight = weightValue(block, indexGeo, itGeo.index());

			if(weight == 0){
				continue;
			}

			if (MVector(skinned).length() < 1){

				float poimtPos = MVector(skinned).length(), curveRampValue;

				hFalloff.getValueAtPosition(poimtPos, curveRampValue, &stat);

				skinned += normals[itGeo.index()] * curveRampValue * weight * env;
			}

			skinned *= matr;
			skinned *= matrixInverse;
			/// position point
			paAllPoints.append(skinned);

		}//end loop

	}else{

		/// loop
		for(; !itGeo.isDone(); itGeo.next()){

//			MPoint skenned;
//			MPoint pt = itGeo.position();
//
//			/// get vertex weight
//			float weight = weightValue(block, indexGeo, itGeo.index());
//
//			if( weight == 0){
//				skenned = pt;
//			}else{
//				skenned += ((( pt * binMatrix * matr) * weight) + (pt*(1-weight))) * env;
//			}
//
//			/// position point
//			paAllPoints.append(skenned);

			MPoint skinned;
			MPoint pt = itGeo.position();
			/// get vertex weight
			float weight = weightValue(block, indexGeo, itGeo.index());
			skinned += (((pt * binMatrix * matr)*weight)+(pt*(1-weight)))*env+(pt*(1-env));
			/// position point
			paAllPoints.append(skinned);

		}/// end loop

	}

	itGeo.setAllPositions(paAllPoints);
	return MS::kSuccess;

}

MStatus softCluster::initialize(){

	MStatus stat;

	MFnNumericAttribute nAttr;
	MRampAttribute rAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;
	MFnMatrixAttribute matAttr;
	MFnUnitAttribute MFnUnitAttr;

	///// bindMat update ///
	bindUpdate = nAttr.create("bind", "bind", MFnNumericData::kBoolean, 0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = nAttr.setKeyable(true);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(bindUpdate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///// bindMat update ///
	push = nAttr.create("push", "push", MFnNumericData::kBoolean, 0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = nAttr.setKeyable(true);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(push);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// aWorldMatrix attribute ////////
	aMatrix = matAttr.create("Matrix", "mat");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	stat = addAttribute(aMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// bindMatrix attribute ////////
	bindMatrix = matAttr.create("bindMatrix", "Bmat");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	stat = addAttribute(bindMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aFalloff attribute ////////
	aFalloff = rAttr.createCurveRamp("falloff", "fo");
	stat = addAttribute(aFalloff);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// affect attribute /////
	stat = attributeAffects(push, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(bindUpdate, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aMatrix, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aFalloff, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(bindMatrix, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// Make deformer weights paintable ////////
	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer softCluster weights;");

	return MS::kSuccess;
}

void* softCluster::creator(){
	return new softCluster;
}
