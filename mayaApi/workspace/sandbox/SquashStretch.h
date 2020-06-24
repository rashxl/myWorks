/*
 * SquashStretch.h
 *
 *  Created on: Apr 7, 2017
 *      	Author: rash
 *
 *     description: for use, you need select mesh and run commant "deformer -type squashStretch". Then parent target locator to the root locator and set compute attribute to on.
 */

#ifndef SQUASHSTRETCH_H_
#define SQUASHSTRETCH_H_


#include <maya/MPxDeformerNode.h>
#include <maya/MDataBlock.h>
#include <maya/MItGeometry.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagModifier.h>
#include <maya/MThreadUtils.h>
#include <maya/MPlug.h>
#include <maya/MQuaternion.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>

#include <omp.h>
#include <cmath>

class squashStretch : public MPxDeformerNode{
public:
	squashStretch();
	virtual ~squashStretch();

	//virtual void postConstructor();

	virtual MStatus deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix& localToWorldMat, unsigned int indexGeo);
	static void* creator();
	static MStatus initialize();

	virtual MObject& accessoryAttribute() const;
	virtual MStatus accessoryNodeSetup(MDagModifier& mDagMod);


	static MTypeId id;
	static MObject root;
	static MObject target;
	static MObject factor;
	static MObject StrScale;
	static MObject SqhScale;
	static MObject squash;
	static MObject stretch;
	static MObject bend;

	static MObject compute;
private:
	bool firstCall;
	double fixdisY;

};








#endif /* SQUASHSTRETCH_H_ */
