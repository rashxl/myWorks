/*
 * mathNode.h
 *
 *  Created on: Apr 13, 2017
 *      Author: rash
 */

#ifndef MATHNODE_H_
#define MATHNODE_H_

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MTypeId.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MDataHandle.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MRampAttribute.h>

#include <cmath>


class mathNode : public MPxNode{
public:
	mathNode(){};
	virtual ~mathNode(){};

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId id;

	// pow
	static MObject PowVal;
	static MObject PowBase;
	// log
	static MObject LogVal;
	static MObject LogBase;
	// exp
	static MObject ExpVal;
	// mod
	static MObject Num;
	static MObject Denom;
	// abs
	static MObject absVal;
	// sin
	static MObject aSin;
	// fcurve
	static MObject fcurve;
	static MObject inVal;


	static MObject method;

	static MObject result;
};


#endif /* MATHNODE_H_ */
