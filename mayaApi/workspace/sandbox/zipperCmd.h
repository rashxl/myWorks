/*
 * zipperCmd.h
 *
 *  Created on: Nov 10, 2016
 *      Author: rash
 */

#ifndef ZIPPERCMD_H_
#define ZIPPERCMD_H_



#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MDGModifier.h>
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MSelectionList.h>

class zipperCmd : public MPxCommand{
public:
	zipperCmd() {}
	virtual ~zipperCmd() {}
	virtual MStatus doIt(const MArgList &arg);
	virtual MStatus redoIt();
	virtual MStatus undoIt();
	virtual bool isUndoable() const;

	static void * creator(){ return new zipperCmd;}
	static MSyntax newSyntax();

private:
	MStatus getShapeNode(MDagPath& path);
	MDagPath pathShapeinCurve;
	MDGModifier DGmod;
	static const char *parentShotFlag;
	static const char *parentLongFlag;
	MString flag;

};


#endif /* ZIPPERCMD_H_ */
