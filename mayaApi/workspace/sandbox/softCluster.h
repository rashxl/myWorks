/*
 * softCluster.h
 *
 *  Created on: Mar 29, 2017
 *      Author: rash
 */

#ifndef SOFTCLUSTER_H_
#define SOFTCLUSTER_H_

#include <maya/MPxDeformerNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagModifier.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>
#include <maya/MMeshIntersector.h>
#include <maya/MPoint.h>
#include <maya/MPlane.h>
#include <maya/MPointArray.h>
#include <maya/MItGeometry.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFloatVectorArray.h>

class softCluster : public MPxDeformerNode{
public:

	softCluster();
	virtual ~softCluster();

	virtual MStatus deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix& localToWorldMat, unsigned int indexGeo);
	static void * creator();
	static MStatus initialize();

	virtual MObject& accessoryAttribute() const;
	virtual MStatus accessoryNodeSetup(MDagModifier& mDagMod);

	static MTypeId id;
	static MObject aFalloff;
	static MObject bindUpdate;
	static MObject push;
	static MObject aMatrix;
	static MObject bindMatrix;

	bool firstCall;

private:
	MMatrix binMatrix;

};




#endif /* SOFTCLUSTER_H_ */
