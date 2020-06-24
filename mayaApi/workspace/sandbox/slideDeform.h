#ifndef SLIDEDEFORM_H_
#define SLIDEDEFORM_H_

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

class slideDeform : public MPxDeformerNode{
public:

	slideDeform();
	virtual ~slideDeform();

	virtual MStatus deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix& localToWorldMat, unsigned int indexGeo);
	static void * creator();
	static MStatus initialize();

	virtual MObject& accessoryAttribute() const;
	virtual MStatus accessoryNodeSetup(MDagModifier& mDagMod);

	static MTypeId id;
	static MObject aPosition;
	static MObject aRadius;
	static MObject aFalloff;
	static MObject aAlgorithm;
	static MObject aMatrix;

	static MObject aDisplace;
	static MObject aDisplaceX;
	static MObject aDisplaceY;
	static MObject aDisplaceZ;

	static MObject aConTranslate;
	static MObject aConTranslateX;
	static MObject aConTranslateY;
	static MObject aConTranslateZ;
	bool firstCall;


};


#endif
