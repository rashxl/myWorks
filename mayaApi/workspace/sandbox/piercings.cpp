#include "piercings.h"

MObject piercings::aPosition;
MObject piercings::aRadius;
MObject piercings::aFalloff;
MObject piercings::aSlide;
MObject piercings::aSize;
MObject piercings::aMatrix;
MObject piercings::bindMatrix;

MObject piercings::aDisplace;
MObject piercings::aDisplaceX;
MObject piercings::aDisplaceY;
MObject piercings::aDisplaceZ;

MObject piercings::aConTranslate;
MObject piercings::aConTranslateX;
MObject piercings::aConTranslateY;
MObject piercings::aConTranslateZ;


piercings::piercings(){
}

piercings::~piercings(){

}

MObject& piercings::accessoryAttribute() const{
	 return aMatrix;
}

MStatus piercings::accessoryNodeSetup(MDagModifier& mDagMod){

	MStatus stat;

	////// create spaceLocator ///////
	MObject oTransform = mDagMod.createNode("locator", MObject::kNullObj, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oSphere = mDagMod.createNode("implicitSphere", oTransform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject oCtrl = mDagMod.createNode("locator", MObject::kNullObj, &stat);
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
	MObject aLocLocalSpace = fnLocator.attribute("localPosition", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(oLocator, aLocLocalSpace, thisMObject(), aDisplace);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

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

	//// Connect CTRL.tr --> transform node ////
	MFnDagNode fnCtrlLoc(oCtrl, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aCtrltransAttr = fnCtrlLoc.attribute("translate", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MObject aCtrlLocMatAttr = fnCtrlLoc.attribute("matrix", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// connect local mat to node
	stat = mDagMod.connect(oCtrl, aCtrlLocMatAttr, thisMObject(), aMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// parent CTRl
	stat = MGlobal::executeCommand("parent "+fnCtrlLoc.name()+" "+fnLocator.name()+";");
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	//// set attribute / name   -- locatorShape.localPosition keyable
	stat = MGlobal::executeCommand("setAttr -k on "+fnLocator.name()+".localPositionX; \n setAttr -k on "+fnLocator.name()+".localPositionY;\n setAttr -k on "+fnLocator.name()+".localPositionZ;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	//// set attribute / name   -- locatorShape.localPosition keyable
	stat = MGlobal::executeCommand("setAttr -k on "+fnLocator.name()+".localPositionX; \n setAttr -k on "+fnLocator.name()+".localPositionY;\n setAttr -k on "+fnLocator.name()+".localPositionZ;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// visibility template ////
	stat = MGlobal::executeCommand("setAttr "+fnSphere.name()+".overrideEnabled 1; \n setAttr "+fnSphere.name()+".overrideDisplayType 1;");
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = MGlobal::executeCommand("setAttr \""+fnLocator.name()+".visibility\" 0; ");
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = mDagMod.connect(oCtrl, aCtrltransAttr, oLocator, aLocLocalSpace);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// rename
	mDagMod.renameNode(oTransform, "loc_piercing#");
	mDagMod.renameNode(oLocator, "loc_piercingShape#");
	mDagMod.renameNode(oSphere, "loc_piercingSphereShape#");
	mDagMod.renameNode(oCtrl, "CTRL_piercing#");
	mDagMod.doIt();

	piercings::firstCall = true;

	return MS::kSuccess;
}

MStatus piercings::deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix & localToWorldMat, unsigned int indexGeo){

	MStatus stat;

	MThreadUtils::syncNumOpenMPThreads();

	#ifndef _DEBUG
	#ifdef _OPENMP
	omp_set_num_threads(omp_get_num_procs());
	#endif
	#endif


	//######################### matrix#############################

	//// get world matrix from locator /////
	MDataHandle matData = block.inputValue(aMatrix);
	MMatrix matr = matData.asMatrix();

	MMatrix matrInv = matr.inverse();

	/// get bindMatrix attr ////
	MDataHandle binMatHandle = block.inputValue(bindMatrix);

	//######################### matrix#############################

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
		binMat = binMatHandle.asMatrix();

		firstCall = false;

	}

	//// get attribute envelope ////
	float env = block.inputValue(envelope).asFloat();
	if(env == 0.0) return MS::kSuccess;

	//// position ////
	int nPosition = block.inputValue(aPosition).asInt();

	//// radius  ////
	float fRadius = block.inputValue(aRadius).asFloat();

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

	/// get world space of position
	MPoint pPosition;
	stat = fnMesh.getPoint(nPosition, pPosition, MSpace::kWorld);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// displace (has to come after constraint, else delayed evaluation (&) for constraint)
	MDataHandle hDisplace = block.inputValue(aDisplace);
	double3& d3Displace = hDisplace.asDouble3();
	MPoint pDisplace(d3Displace[0],d3Displace[1], d3Displace[2], 1.0);


	//// get rotate in radians type
	//MDataHandle hRotate = block.inputValue(aRot);
	//double3& d3Rotate = hRotate.asDouble3();
	//MEulerRotation MErot(d3Rotate[0]*180/3.1415,d3Rotate[1]*180/3.1415, d3Rotate[2]*180/3.1415, MEulerRotation::kXYZ);
	//MVector vRotate(d3Rotate[0],d3Rotate[1], d3Rotate[2]);
	//MGlobal::displayInfo(MString("MErot.x ")+roundf((int)MErot.x * 10)/10); /// round value
	//MGlobal::displayInfo(MString("MErot.y ")+roundf((int)MErot.y * 10)/10);
	//MGlobal::displayInfo(MString("MErot.z ")+roundf((int)MErot.z * 10)/10);

	/// set constrain translate attribute
	MDataHandle hConTranslate = block.outputValue(aConTranslate);
	hConTranslate.set3Double(pPosition.x,pPosition.y,pPosition.z);

	///frequently used variables outside of loop (maybe will increase performance a tiny bit)
	MMatrix matrixInverse = localToWorldMat.inverse();

	/// get displace vector
	MPoint vecDisplace = pPosition - pDisplace;

	/// save point position to be set after iterator is done
	MPointArray paAllPoints;

	///get aSlide
	bool slide = block.inputValue(aSlide).asBool();
	///get aSize
	float size = block.inputValue(aSize).asFloat();

	if(slide){

		matr[3][0] += pPosition.x;
		matr[3][1] += pPosition.y;
		matr[3][2] += pPosition.z;

		matrInv = matr.inverse();

		//*------------------- slide aAlgorithm -------------------*//

		for(; !itGeo.isDone(); itGeo.next()){

			MPoint skinned;
			MPoint pt = itGeo.position();

			double distance = (pPosition - pt).length();

			hFalloff.getValueAtPosition(distance/fRadius, falloff);

			/// get vertex weight
			float weight = weightValue(block, indexGeo, itGeo.index());


			skinned = pt;

			skinned *= localToWorldMat ;

			skinned *= matrInv;

			if(MVector(skinned).length()/fRadius < 1){

				float poimtPos = MVector(skinned).length(), curveRampValue;

				hFalloff.getValueAtPosition(poimtPos/fRadius, curveRampValue, &stat);

				skinned += (normals[itGeo.index()]*size) * curveRampValue * weight * env ;

			}

			skinned *= matr;
			skinned *= matrixInverse;

			/// position point
			paAllPoints.append(skinned);

		}//end loop
	}
	else
	{

		//*------------------- move aAlgorithm -------------------*//

			//loop
		for(; !itGeo.isDone(); itGeo.next()){


			MPoint skinned;
			MPoint pt = itGeo.position();

			double distance = (pPosition - pt).length();
			hFalloff.getValueAtPosition(distance/fRadius, falloff);

			/// get vertex weight
			float weight = weightValue(block, indexGeo, itGeo.index());

			skinned += (( pt * binMat * matr)*falloff) + (pt*(1-falloff));
			skinned = (((skinned*weight)+(pt*(1-weight)))*env)+(skinned*(1-env));

			/// position point
			paAllPoints.append(skinned);

		}///end loop

	}


				/*
	/// loop
	for(; !itGeo.isDone(); itGeo.next()){


		MPoint skinned;
		MPoint pt = itGeo.position();

		double distance = (pPosition - pt).length();
		hFalloff.getValueAtPosition(distance/fRadius, falloff);

		/// get vertex weight
		float weight = weightValue(block, indexGeo, itGeo.index());

		if( weight == 0){
			skinned = pt;
		}else{

			if(slide){



				skinned = pt;

				skinned *= localToWorldMat ;

				skinned *= matrInv;

				if(MVector(skinned).length()/fRadius < 1){

					float poimtPos = MVector(skinned).length(), curveRampValue;

					hFalloff.getValueAtPosition(poimtPos/fRadius, curveRampValue, &stat);

					skinned += normals[itGeo.index()] * curveRampValue * weight * env ;

				}

				skinned *= matr;

				skinned *= matrixInverse;

			}else{

				skinned += (( pt * binMat * matr)*falloff) + (pt*(1-falloff));
				skinned = (((skinned*weight)+(pt*(1-weight)))*env)+(skinned*(1-env));

			}
		}


		/// position point
		paAllPoints.append(skinned);



	}/// end loop
*/

	itGeo.setAllPositions(paAllPoints);
	return MS::kSuccess;

}

MStatus piercings::initialize(){

	MStatus stat;

	MFnNumericAttribute nAttr;
	MRampAttribute rAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;
	MFnMatrixAttribute matAttr;
	MFnUnitAttribute MFnUnitAttr;

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

	////// aSize attribute ////////
	aSize = nAttr.create("Size", "sz", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setMin(0.0));
	stat = addAttribute(aSize);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aFalloff attribute ////////
	aFalloff = rAttr.createCurveRamp("falloff", "fo");
	stat = addAttribute(aFalloff);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	////// aSlide attribute ////////
	aSlide = nAttr.create("slide", "sl", MFnNumericData::kBoolean, 0, &stat);
	CHECK_MSTATUS(nAttr.setKeyable(false));
	stat = addAttribute(aSlide);
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
	stat = attributeAffects(bindMatrix, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aDisplace, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aSlide, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aSize, outputGeom);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects(inputGeom, aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(aPosition, aConTranslate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


///// Make deformer weights paintable ////////
	MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer piercings weights;");

	return MS::kSuccess;
}

void* piercings::creator(){
	return new piercings;
}
