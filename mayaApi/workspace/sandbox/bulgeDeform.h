
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MTypeId.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MItGeometry.h>
#include <maya/MGlobal.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MDagModifier.h>
#include <maya/MMatrix.h>
#include <maya/MRampAttribute.h>

#include <math.h>

class budleDeform : public MPxDeformerNode

{
public:
	budleDeform();
	virtual ~budleDeform();
	virtual MStatus deform(MDataBlock& block, MItGeometry& itgeo, const MMatrix& mat, unsigned int multiIndex);
	static void * creator();

	virtual MObject& accessoryAttribute() const;
	virtual MStatus accessoryNodeSetup(MDagModifier& cmd);

	static MObject matrix;
	static MObject startDist;
	static MObject endDist;

	static MTypeId id;
	static MStatus initialize();

	static MObject aBugleAmount;

	static MObject aMagnitude;
	static MObject aMean;
	static MObject aVariance;
	static MObject aCurveRamp;
};


