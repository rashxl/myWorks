#ifndef PIERCINGS_H_
#define PIERCINGS_H_

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
#include <maya/MThreadUtils.h>

#include <omp.h>


class piercings : public MPxDeformerNode{
public:

	piercings();
	virtual ~piercings();

	virtual MStatus deform(MDataBlock& block, MItGeometry& itGeo, const MMatrix& localToWorldMat, unsigned int indexGeo);
	static void * creator();
	static MStatus initialize();

	virtual MObject& accessoryAttribute() const;
	virtual MStatus accessoryNodeSetup(MDagModifier& mDagMod);

	static MTypeId id;
	static MObject aPosition;
	static MObject aRadius;
	static MObject aFalloff;
	static MObject aSlide;
	static MObject aSize;
	static MObject aMatrix;
	static MObject bindMatrix;

	static MObject aDisplace;
	static MObject aDisplaceX;
	static MObject aDisplaceY;
	static MObject aDisplaceZ;

	static MObject aConTranslate;
	static MObject aConTranslateX;
	static MObject aConTranslateY;
	static MObject aConTranslateZ;

	bool firstCall;

private:
	MMatrix binMat;


};


#endif
