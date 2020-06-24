/*
 * softClamp.h
 *
 *  Created on: Apr 18, 2017
 *      Author: rash
 */

#ifndef SOFTCLAMP_H_
#define SOFTCLAMP_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MDataHandle.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MRampAttribute.h>

#include <cmath>



class softClamp : public MPxNode{
public:
	softClamp(){};
	virtual ~softClamp(){};

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId id;

	// soft clamp
	static MObject value;
	static MObject limitA;
	static MObject limitASoft;
	static MObject limitB;
	static MObject limitBSoft;
	// soft max
	static MObject valMax;
	static MObject limMax;
	static MObject softMax;
	// soft min
	static MObject valMin;
	static MObject limMin;
	static MObject softMin;

	// clamp
	static MObject valClamp;
	static MObject limACalmp;
	static MObject limBCalmp;


	static MObject method;

	static MObject result;
};




#endif /* SOFTCLAMP_H_ */
