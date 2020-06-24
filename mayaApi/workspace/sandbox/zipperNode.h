#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MGlobal.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPointArray.h>
#include <maya/MRampAttribute.h>
#include <maya/MArrayDataBuilder.h>


class zipperNode:public MPxNode{
public:
	zipperNode() {}
	virtual ~zipperNode() {}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MObject rCurveRamp;
	static MObject gCurveRamp;
	static MObject inCurve;
	static MObject amount;
	static MObject scale;
	static MObject bias;
	static MObject zip;
	static MObject outCurve;
	static MTypeId id;
};


