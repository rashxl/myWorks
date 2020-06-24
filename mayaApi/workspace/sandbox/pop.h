#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnCompoundAttribute.h>


class pop:public MPxNode{
public:
	pop() {}
	virtual ~pop() {}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MObject inPoly;
	static MObject vtx;
	static MObject aPose;
	static MObject aPoseX;
	static MObject aPoseY;
	static MObject aPoseZ;
	static MTypeId id;
};


