/*
 * colideP.cpp
 *
 *  Created on: Apr 21, 2017
 *      Author: rash
 */


#include "colideP.h"


MObject colideP::inMesh;
MObject colideP::inSurfMesh;
MObject colideP::WM;
MObject colideP::Point;
MObject colideP::Ctrl;
MObject colideP::depth;
MObject colideP::scale;

MObject  colideP::translate;
MObject  colideP::translateX;
MObject  colideP::translateY;
MObject  colideP::translateZ;

MObject  colideP::rotate;
MObject  colideP::rotateX;
MObject  colideP::rotateY;
MObject  colideP::rotateZ;

/// math func
MVector crossProd(MVector a, MVector b);
float clamping(float val, float max, float min);
float GetDistByTwoMat(MMatrix matA, MMatrix matB);


MStatus colideP::compute(const MPlug& plug, MDataBlock& data){
	MStatus stat;

	if(plug == translate || plug == rotate ){

		// scale
		double scaleVal = data.inputValue(scale).asDouble();
		double depthVal = data.inputValue(depth).asDouble();

		/// inMesh ///
		MDataHandle inMeshDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject MOinMesh = inMeshDataHandle.asMesh();
		MFnMesh fnInMesh(MOinMesh);

		/// inSurface ///
		MDataHandle inSurfDataHandle = data.inputValue(inSurfMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject MOinSurf = inSurfDataHandle.asNurbsSurface();
		MFnNurbsSurface fnNurbSrf(MOinSurf);

		/// matrix from transform mesh
		MMatrix worldMeshTransformMatrix = data.inputValue(WM).asMatrix();

		MMatrix matPoint = data.inputValue(Point).asMatrix();
		MMatrix matCtrl = data.inputValue(Ctrl).asMatrix();
		MMatrix newMatrPoint, resultMat;

		MPoint Point(matPoint[3][0],matPoint[3][1],matPoint[3][2],1.0), closPoint;
		MVector closNormal;
		Point = Point * worldMeshTransformMatrix.inverse();
//		MGlobal::displayInfo(MOinMesh.apiTypeStr());
//		MGlobal::displayInfo(MOinSurf.apiTypeStr());
		if(MOinMesh.apiType() == MFn::kMeshData){
			stat = fnInMesh.getClosestPointAndNormal(Point, closPoint, closNormal);
			CHECK_MSTATUS_AND_RETURN_IT(stat);
		}else if(MOinSurf.apiType() == MFn::kNurbsSurfaceData){
			double U,V;
			closPoint = fnNurbSrf.closestPoint(Point, &U, &V);
			closNormal = fnNurbSrf.normal(U,V);
		}else{
			MGlobal::displayWarning("Have not the mesh witch have finding closest point!");
			return MS::kSuccess;
		}

		MVector xTemp(1,0,0);
		closNormal = closNormal * worldMeshTransformMatrix;
		xTemp = xTemp * worldMeshTransformMatrix;

//		// axis Z
		MVector vecZ = crossProd(xTemp, closNormal);
//		// axis X
		MVector vecX = crossProd(closNormal, vecZ);

		closPoint = closPoint * worldMeshTransformMatrix;


		/// x ///
		newMatrPoint[0][0] = vecX[0];
		newMatrPoint[0][1] = vecX[1];
		newMatrPoint[0][2] = vecX[2];

		/// y ///
		newMatrPoint[1][0] = closNormal[0];
		newMatrPoint[1][1] = closNormal[1];
		newMatrPoint[1][2] = closNormal[2];

		/// z ///
		newMatrPoint[2][0] = vecZ[0];
		newMatrPoint[2][1] = vecZ[1];
		newMatrPoint[2][2] = vecZ[2];

		/// pos ///
		newMatrPoint[3][0] = closPoint[0];
		newMatrPoint[3][1] = closPoint[1];
		newMatrPoint[3][2] = closPoint[2];

		/// mult newMatrixPoint to matW

		float dis = GetDistByTwoMat(newMatrPoint, matPoint);
		//MGlobal::displayInfo(MString("distance ")+dis);
		//float dis = GetDistByTwoMat(newMatrPoint, matPoint);
		//MGlobal::displayInfo(MString("distance ")+dis);
		newMatrPoint = newMatrPoint * matPoint.inverse();
		/// mult mat to ctrl mat
		newMatrPoint = newMatrPoint * matCtrl;

		MMatrix BlendMat = newMatrPoint * matCtrl.inverse();

		double t = (double)clamping((float)exp(BlendMat[3][1])*depthVal, 1.0, 0.0);

		/// get distance between mat ctrls and clsPoint
		//float dis = GetDistByTwoMat(newMatrPoint, matCtrl);
		//MGlobal::displayInfo(MString("distance ")+dis);

		//resultMat = newMatrPoint;
		//double t = (double)clamping((float)exp(newMatrPoint[3][1])*scaleVal, 1.0, 0.0);
		//resultMat = depthVal*((1-t)*matCtrl+t*newMatrPoint);

//		double t = 1.0-(double)clamping((float)exp(dis)*scaleVal, 1.0, 0.0);
//		MGlobal::displayInfo(MString("distance ")+dis);
		resultMat = (1-t)*matCtrl+t*newMatrPoint;
		resultMat = (1-scaleVal)*resultMat+ scaleVal*matCtrl;


		MEulerRotation EuRot;
		EuRot = resultMat;


		/// set translate attribute
		MDataHandle hConTranslate = data.outputValue(translate);
		hConTranslate.set3Double(resultMat[3][0],resultMat[3][1],resultMat[3][2]);
		hConTranslate.setClean();

		/// set rotation attribute
		MDataHandle hConRotate = data.outputValue(rotate);
		hConRotate.set3Double(EuRot.x,EuRot.y,EuRot.z);
		hConRotate.setClean();


		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


	}else{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}


void* colideP::creator(){
	return new colideP;
}

MStatus colideP::initialize(){
	MStatus stat;

	MFnNumericAttribute fnNumAAttr;
	MFnTypedAttribute fnTypeAttr;
	MFnMatrixAttribute fnMatAttr;
	MFnCompoundAttribute fnCmpAttr;
	MFnUnitAttribute fnUnAttr;

	/// mesh
	inMesh = fnTypeAttr.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnTypeAttr.setStorable(true));
	stat = addAttribute(inMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat)

	/// surf
	inSurfMesh = fnTypeAttr.create("inSurf", "inSurf", MFnNurbsSurfaceData::kNurbsSurface, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnTypeAttr.setStorable(true));
	stat = addAttribute(inSurfMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat)

	/// world matrix
	WM = fnMatAttr.create("wm", "wm");
	fnMatAttr.setStorable(false);
	fnMatAttr.setConnectable(true);
	stat = addAttribute(WM);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/// Point
	Point = fnMatAttr.create("point", "pt");
	fnMatAttr.setStorable(false);
	fnMatAttr.setConnectable(true);
	stat = addAttribute(Point);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	/// Ctrl
	Ctrl = fnMatAttr.create("Ctrl", "ctrl");
	fnMatAttr.setStorable(false);
	fnMatAttr.setConnectable(true);
	stat = addAttribute(Ctrl);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/// depth
	depth = fnNumAAttr.create("depth", "depth", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumAAttr.setMin(0.0));
	stat = addAttribute(depth);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/// scale
	scale = fnNumAAttr.create("blend", "blend", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumAAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumAAttr.setMax(1.0));
	stat = addAttribute(scale);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// translate
	translateX = fnNumAAttr.create("translateX", "tx", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	translateY = fnNumAAttr.create("translateY", "ty", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	translateZ = fnNumAAttr.create("translateZ", "tz", MFnNumericData::kDouble, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	translate = fnCmpAttr.create("translate", "t", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnCmpAttr.addChild(translateX));
	CHECK_MSTATUS(fnCmpAttr.addChild(translateY));
	CHECK_MSTATUS(fnCmpAttr.addChild(translateZ));
	CHECK_MSTATUS(fnCmpAttr.setKeyable(false));
	CHECK_MSTATUS(fnCmpAttr.setWritable(false));
	CHECK_MSTATUS(fnCmpAttr.setStorable(false));
	stat = addAttribute(translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// rotate
	rotateX = fnUnAttr.create("rotateX", "rx", MFnUnitAttribute::kAngle, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	rotateY = fnUnAttr.create("rotateY", "ry", MFnUnitAttribute::kAngle, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	rotateZ = fnUnAttr.create("rotateZ", "rz", MFnUnitAttribute::kAngle, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	rotate = fnCmpAttr.create("rotate", "r", &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnCmpAttr.addChild(rotateX));
	CHECK_MSTATUS(fnCmpAttr.addChild(rotateY));
	CHECK_MSTATUS(fnCmpAttr.addChild(rotateZ));
	CHECK_MSTATUS(fnCmpAttr.setKeyable(false));
	CHECK_MSTATUS(fnCmpAttr.setWritable(false));
	CHECK_MSTATUS(fnCmpAttr.setStorable(false));
	stat = addAttribute(rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/// affects attributes
	stat = attributeAffects(inMesh, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(inSurfMesh, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(WM, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(Point, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(Ctrl, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(depth, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(scale, translate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects(inMesh, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(inSurfMesh, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(WM, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(Point, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(Ctrl, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(depth, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(scale, rotate);
	CHECK_MSTATUS_AND_RETURN_IT(stat);




	return MS::kSuccess;
}

