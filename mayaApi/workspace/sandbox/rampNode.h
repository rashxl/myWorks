#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MDataHandle.h>


class rampNode:public MPxNode{
public:
	rampNode() {}
	virtual ~rampNode() {}

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MObject inDoubleArray;
	static MObject inFloatArray;
	static MObject inFloat;
	static MObject aRamp;
	static MObject oldMin;
	static MObject oldMax;
	static MObject Min;
	static MObject Max;
	static MObject outValue;
	static MObject outFloat;
	static MTypeId id;
	bool firstCall;
};


