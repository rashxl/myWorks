/*
 * JointCreator.h
 *
 *  Created on: Sep 28, 2016
 *      Author: rash
 */

#ifndef JOINTCREATOR_H_
#define JOINTCREATOR_H_


#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgList.h>
//#include <maya/MSimple.h>
#include <maya/MPxCommand.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPoint.h>
#include <maya/MFnIkJoint.h>
#include <maya/MIOStream.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>
#include <cmath>



class CreatorJonts:public MPxCommand{
private:

	static const char *numberFlag;
	static const char *numberLongFlag;

	static const char *pFlag;
	static const char *pLongFlag;

	void push(double a, bool b);

public:

	virtual MStatus doIt( const MArgList& );
	virtual MStatus undoIt() const;
	virtual MStatus redoIt() const;
	virtual bool isUndoable() const {return true;}


	static void* creator();
	static MSyntax newSyntax();
};


#endif /* JOINTCREATOR_H_ */
