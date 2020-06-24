#include "slideDeform.h"


MObject slideDeform::aPosition;
MObject slideDeform::aRadius;
MObject slideDeform::aFalloff;
MObject slideDeform::aAlgorithm;
MObject slideDeform::aMatrix;



MObject slideDeform::aDisplace;
MObject slideDeform::aDisplaceX;
MObject slideDeform::aDisplaceY;
MObject slideDeform::aDisplaceZ;

MObject slideDeform::aConTranslate;
MObject slideDeform::aConTranslateX;
MObject slideDeform::aConTranslateY;
MObject slideDeform::aConTranslateZ;


slideDeform::slideDeform(){


}
slideDeform::~slideDeform(){

}

MObject& slideDeform::accessoryAttribute() const{
	 return aDisplace;
}

MStatus slideDeform::accessoryNodeSetup(MDagModifier& mDagMod){

	MStatus stat;

/*
	////// create spaceLocator ///////
	MObject oTransform = mDagMod.createNode("implicitSphere", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MObject oLoc = mDagMod.createNode("locator", oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// create nodes to be able to work with them in accessoryNodeSetup ///////
	stat = mDagMod.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	///// aConTranslate >> transform.t //////
	MFnDagNode fnTransform(oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocTranslate = fnTransform.attribute("translate", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(thisMObject(), aConTranslate, oTransform, aLocTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	///// locator.worldMatrix >> aMatrix ////
	MFnDagNode fnLocator(oLoc, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocWorldSpace = fnLocator.attribute("worldMatrix", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(oLoc, aLocWorldSpace, thisMObject(), aMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///// aRadius >> Sphere.radius ////
	MObject oShereShape = fnTransform.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MFnDagNode fnSphere(oShereShape, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aSphereRadius = fnSphere.attribute("radius", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(thisMObject(), aRadius, oShereShape, aSphereRadius);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// set attribute / name   -- locatorShape.localPosition keyable
	//stat = MGlobal::executeCommand("setAttr -k on "+fnLocator.name()+".localPositionX; \n setAttr -k on "+fnLocator.name()+".localPositionY;\n setAttr -k on "+fnLocator.name()+".localPositionZ;");
	///CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// sphereShape visibility template ////
	stat = MGlobal::executeCommand("setAttr "+fnSphere.name()+".overrideEnabled 1; \n setAttr "+fnSphere.name()+".overrideDisplayType 1;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// rename transform and shapes ////
	stat = MGlobal::executeCommand("rename "+fnTransform.name()+" loc_slideNode#; rename "+fnLocator.name()+" "+fnTransform.name()+"Shape#; rename "+fnSphere.name()+" "+fnTransform.name()+"Shape;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	slideDeform::firstCall = true;*/



	////// create spaceLocator ///////
	MObject oTransform = mDagMod.createNode("locator", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oSphere = mDagMod.createNode("implicitSphere", oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// create nodes to be able to work with them in accessoryNodeSetup ///////
	stat = mDagMod.doIt();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///// aConTranslate >> transform.t //////
	MFnDagNode fnTransform(oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocTranslate = fnTransform.attribute("translate", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(thisMObject(), aConTranslate, oTransform, aLocTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///// locatorShape.worldPosition >> aDisplace ////
	MObject oLocator = fnTransform.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MFnDagNode fnLocator(oLocator, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocWorldSpace = fnLocator.attribute("worldPosition", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(oLocator, aLocWorldSpace, thisMObject(), aDisplace);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	//MObject aLocWorldMatSpace = fnLocator.attribute("worldMatrix", &stat);
	//CHECK_MSTATUS_AND_RETURN_IT(stat);
	////stat = mDagMod.connect(oLocator, aLocWorldMatSpace, thisMObject(), aMatrix);
	//CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// aRadius >> locatorShape.localScaleXYZ /////
	MObject aLocScaleX = fnLocator.attribute("localScaleX", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocScaleY = fnLocator.attribute("localScaleY", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aLocScaleZ = fnLocator.attribute("localScaleZ", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = mDagMod.connect(thisMObject(), aRadius, oLocator, aLocScaleX);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(thisMObject(), aRadius, oLocator, aLocScaleY);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(thisMObject(), aRadius, oLocator, aLocScaleZ);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// aRadius >> sphereShape.radius ////
	MFnDagNode fnSphere(oSphere, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aSphereRadius = fnSphere.attribute("radius");
	stat = mDagMod.connect(thisMObject(), aRadius, oSphere, aSphereRadius);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// set attribute / name   -- locatorShape.localPosition keyable
	stat = MGlobal::executeCommand("setAttr -k on "+fnLocator.name()+".localPositionX; \n setAttr -k on "+fnLocator.name()+".localPositionY;\n setAttr -k on "+fnLocator.name()+".localPositionZ;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// sphereShape visibility template ////
	stat = MGlobal::executeCommand("setAttr "+fnSphere.name()+".overrideEnabled 1; \n setAttr "+fnSphere.name()+".overrideDisplayType 1;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// rename transform and shapes ////
	stat = MGlobal::executeCommand("rename "+fnTransform.name()+" loc_slideNode; rename "+fnLocator.name()+" "+fnTransform.name()+"Shape; rename "+fnSphere.name()+" "+fnTransform.name()+"Shape;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	slideDeform::firstCall = true;

	/*stat = MGlobal::executeCommand("\
    global proc AEslideDeformTemplate( string $nodeName )\
    {\
        AEswatchDisplay  $nodeName;\
        editorTemplate -beginScrollLayout;\
            editorTemplate -beginLayout \"slideDeform attributes\" -collapse 0;\
                editorTemplate -addControl \"position\";\
                editorTemplate -addControl \"radius\";\
                editorTemplate -addControl \"algorithm\";\
                editorTemplate -addControl \"displace\";\
                AEaddRampControl ($nodeName+\".falloff\");\
            editorTemplate -endLayout;\
            \
            AEdependNodeTemplate $nodeName;\
            \
            editorTemplate -addExtraControls;\
        editorTemplate -endScrollLayout;\
    }\
    ");
	CHECK_MSTATUS_AND_RETURN_IT(stat);*/



	return MS::kSuccess;
}

MStatus slideDeform::deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix & localToWorldMat, unsigned int indexGeo){
	MStatus stat;


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

		firstCall = false;
	}



	//// get world matrix from locator /////
	MDataHandle matData = block.inputValue(aMatrix);
	MMatrix matr = matData.asMatrix();
	MMatrix invMatr = matr.inverse();

/*
	/// get bind Matrix ///
	MDataHandle matDataFirst = block.outputValue(bindMatrix);
	MMatrix BinMat = matDataFirst.asMatrix();
	//MGlobal::displayInfo(MString("BinMat[1][0] ")+BinMat[1][0]);



	if (BinMat[1][0] == 0){
		MGlobal::displayInfo("firstMatrix");
		MDataHandle matDataFirst = block.outputValue(bindMatrix);
		matDataFirst.set(invMatr);
	}

	BinMat = matDataFirst.asMatrix();*/

	//// get attribute envelope ////
	float env = block.inputValue(envelope).asFloat();
	if(env == 0.0) return MS::kSuccess;

	//// position ////
	int nPosition = block.inputValue(aPosition).asInt();

	//// radius  ////
	float fRadius = block.inputValue(aRadius).asFloat();

	//// algorithm  ////
	short sAlgorithm = block.inputValue(aAlgorithm).asShort();

	/// calculate translation constraint   -- get mesh
	MArrayDataHandle hInputs = block.outputArrayValue(input, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = hInputs.jumpToElement(indexGeo);
	MDataHandle hInputElement = hInputs.outputValue(&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oInputGeom = hInputElement.child(inputGeom).asMesh();
	MFnMesh fnMesh(oInputGeom);

	//// create mesh intersection to get closest point more efficient
	MMeshIntersector intersector;
	intersector.create(oInputGeom, localToWorldMat);

	/// get world space of position
	MPoint pPosition;
	fnMesh.getPoint(nPosition, pPosition, MSpace::kWorld);

	/// set constrain translate attribute
	MDataHandle hConTranslate = block.outputValue(aConTranslate);
	hConTranslate.set3Double(pPosition.x,pPosition.y,pPosition.z);

	//// displace (has to come after constraint, else delayed evaluation (&) for constraint)
	MDataHandle hDisplace = block.inputValue(aDisplace);
	double3& d3Displace = hDisplace.asDouble3();
	MPoint pDisplace(d3Displace[0],d3Displace[1], d3Displace[2], 1.0);
	//MPoint pDisplace;
	//pDisplace.x = matr[3][0];
	//pDisplace.y = matr[3][1];
	//pDisplace.z = matr[3][2];

	/// get displace vector
	MPoint vecDisplace = pPosition - pDisplace;

	///frequently used variables outside of loop (maybe will increase performance a tiny bit)
	MMatrix matrixInverse = localToWorldMat.inverse();

	/// save point position to be set after iterator is done
	MPointArray paAllPoints;

	/// loop
	for(; !itGeo.isDone(); itGeo.next()){

		/// get vertex weight
		float weight = weightValue(block, indexGeo, itGeo.index());
		//if(weight == 0){
		//	paAllPoints.append(itGeo.position());
		//	itGeo.next();
			//continue;
		//}
		/// get current vertex
		MPoint pt = itGeo.position();

		/// set point to world space
		pt *= localToWorldMat;



		/// get distance to pPosition to see if it is in radius
		double distance = (pPosition - pt).length();

		/// if it is in radius, get aFalloff for point
		if(distance < fRadius){
			/// get fall-off value
			hFalloff.getValueAtPosition(distance/fRadius, falloff);

			/// adjust displace vector with fall-off
			MPoint vecDisplaceEach = vecDisplace * falloff;

			/// vector to move variable
			MPoint vecMove;

			//-------------------------------
			// translate
			//-------------------------------
			if(sAlgorithm == 0){


				//MPoint VecotorMove;
				//VecotorMove = VecotorMove * matr;
				//vecMove = VecotorMove + vecDisplaceEach;
				//vecMove = (vecMove * invMatr);


				MPoint VecotorMove;
				VecotorMove = VecotorMove * matr;
				vecMove = VecotorMove + vecDisplaceEach;
				vecMove = (vecMove * invMatr);


				//vecMove = pt * (BinMat * matr);
				//pt = pt*BinMat;
				//vecMove = pt*matr;
				//vecMove = vecDisplaceEach*BinMat * matr;



			}
			//-------------------------------
			//... then to closest on mesh
			//-------------------------------
			else if(sAlgorithm == 1){
				/// get closest point
				MPointOnMesh ptOM;
				intersector.getClosestPoint((pt - vecDisplaceEach), ptOM);
				MPoint ptClosest(ptOM.getPoint());
				vecMove = pt-ptClosest;
			}
			//------------------------------
			//... translate on vertex normal plane
			//------------------------------
			else if(sAlgorithm == 2 || sAlgorithm == 3){
				// get vertex normal
				MVector vecNormal;
				fnMesh.getVertexNormal(itGeo.index(), true, vecNormal, MSpace::kObject);//check for results when second arg set to False

				/// create plane
				MPlane planeNormal;
				planeNormal.setPlane(vecNormal, 0.0);

				/// get distance to plane
				double dDistance = planeNormal.directedDistance(vecDisplaceEach);

				/// vector to plane
				MVector ptOnPlane = (vecNormal * dDistance);


				/// create final move vector
				vecMove = vecDisplaceEach - ptOnPlane;

				//... then to closest point on mesh
				//----------------------------------
				if(sAlgorithm == 3){
					/// get closest point
					MPointOnMesh ptOM;
					intersector.getClosestPoint((pt-vecMove), ptOM);
					MPoint ptClosest(ptOM.getPoint());
					vecMove = pt-ptClosest;

				}
			}

			// adjust movement with envelope and weight
			vecMove = vecMove*(env*weight);

			pt = pt - vecMove;


		}/// end if radius

		/// set point to object space again

		pt *= matrixInverse;



		/// position point
		paAllPoints.append(pt);

	}/// end loop

	itGeo.setAllPositions(paAllPoints);
	return MS::kSuccess;
}

MStatus slideDeform::initialize(){
	MStatus stat;

	MFnNumericAttribute nAttr;
	MRampAttribute rAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;
	MFnMatrixAttribute matAttr;

	////// aWorldMatrix attribute ////////
	aMatrix = matAttr.create("Matrix", "mat");
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	stat = addAttribute(aMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aPosition attribute ////////
	aPosition = nAttr.create("position", "pos", MFnNumericData::kInt, 0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setMin(0));
	stat = addAttribute(aPosition);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aRadius attribute ////////
	aRadius = nAttr.create("radius", "rr", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setMin(0.0));
	stat = addAttribute(aRadius);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aFalloff attribute ////////
	aFalloff = rAttr.createCurveRamp("falloff", "fo");
	stat = addAttribute(aFalloff);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aAlgorithm attribute ////////
	aAlgorithm = eAttr.create("algorithm", "alg", 3);
	CHECK_MSTATUS(eAttr.setKeyable(true));
	CHECK_MSTATUS(eAttr.addField("move", 0));
	CHECK_MSTATUS(eAttr.addField("moveClosest", 1));
	CHECK_MSTATUS(eAttr.addField("movePlane", 2));
	CHECK_MSTATUS(eAttr.addField("movePlaneClosest", 3));
	stat = addAttribute(aAlgorithm);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/////// aDisplace attribute ////////
	aDisplaceX = nAttr.create("displaceX", "dpx", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aDisplaceY = nAttr.create("displaceY", "dpy", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aDisplaceZ = nAttr.create("displaceZ", "dpz", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aDisplace = cAttr.create("displace", "dsp", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(cAttr.addChild(aDisplaceX));
	CHECK_MSTATUS(cAttr.addChild(aDisplaceY));
	CHECK_MSTATUS(cAttr.addChild(aDisplaceZ));
	CHECK_MSTATUS(cAttr.setKeyable(true));
	stat = addAttribute(aDisplace);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aConTranslate attribute ////////
	aConTranslateX = nAttr.create("conTranslateX", "ctx", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aConTranslateY = nAttr.create("conTranslateY", "cty", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aConTranslateZ = nAttr.create("conTranslateZ", "ctz", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	aConTranslate = cAttr.create("conTranslate", "cot", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(cAttr.addChild(aConTranslateX));
	CHECK_MSTATUS(cAttr.addChild(aConTranslateY));
	CHECK_MSTATUS(cAttr.addChild(aConTranslateZ));
	CHECK_MSTATUS(cAttr.setKeyable(false));
	CHECK_MSTATUS(cAttr.setWritable(false));
	CHECK_MSTATUS(cAttr.setStorable(false));
	stat = addAttribute(aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// effects ////////
	stat = attributeAffects(aPosition, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aMatrix, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aRadius, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aFalloff, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aAlgorithm, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aDisplace, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(inputGeom, aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aPosition, aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aMatrix, aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// Make deformer weights paintable ////////
	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer slideDeform weights;");

	return MS::kSuccess;
}

void* slideDeform::creator(){
	return new slideDeform;
}
