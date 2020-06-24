/*
 * zipperCmd.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: rash
 */

#include "zipperCmd.h"


const char * zipperCmd::parentShotFlag = "p";
const char * zipperCmd::parentLongFlag = "parent";


MSyntax zipperCmd::newSyntax(){

	MSyntax syntax;

	syntax.addFlag(parentShotFlag, parentLongFlag, MSyntax::kBoolean);

	syntax.setObjectType(MSyntax::kSelectionList, 0, 1);

	syntax.useSelectionAsDefault(true);
	syntax.setMinObjects(1);
	syntax.setMaxObjects(1);

	syntax.enableEdit(false);
	syntax.enableQuery(false);

	return syntax;

}


bool zipperCmd::isUndoable() const{
	return true;
}


MStatus zipperCmd::doIt(const MArgList& arg){


	MStatus stat;

	MArgDatabase argData(syntax(), arg, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MSelectionList selection;

	stat = argData.getObjects(selection);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	unsigned int lenSel = selection.length();

	if(lenSel < 0){

		MGlobal::displayInfo("###Must select one curve!");
		return MS::kSuccess;
	}

	stat = selection.getDagPath(0, pathShapeinCurve);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MDagPath curveTransNode = pathShapeinCurve;

	stat = zipperCmd::getShapeNode(pathShapeinCurve);


    char buffer[512];

    MString dtopCurve = "curveTopZipper";
    MString dlwCurve = "curveLwZipper";

	MGlobal::displayInfo(pathShapeinCurve.fullPathName());

	MGlobal::displayInfo(curveTransNode.fullPathName());

	return redoIt();

}

MStatus zipperCmd::getShapeNode(MDagPath &path){

	MStatus stat;

	if(path.apiType() == MFn::kNurbsCurve)
	{
		return MS::kSuccess;
	}

	unsigned int numShapes;
	stat = path.numberOfShapesDirectlyBelow(numShapes);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

    for ( unsigned int i = 0; i < numShapes; ++i )
	{
		stat = path.extendToShapeDirectlyBelow( i );
		CHECK_MSTATUS_AND_RETURN_IT( stat );

		if ( !path.hasFn( MFn::kNurbsCurve ) )
		{
			path.pop();
			continue;
		}

		MFnDagNode fnNode( path, &stat );
		CHECK_MSTATUS_AND_RETURN_IT( stat );
		if ( !fnNode.isIntermediateObject() )
		{
			return MS::kSuccess;
		}
		path.pop();
	}

	return MS::kFailure;
}



MStatus zipperCmd::undoIt(){

	MStatus stat;

	stat = DGmod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT( stat );
	return MS::kSuccess;
}

MStatus zipperCmd::redoIt(){
	MStatus stat;

	stat = DGmod.doIt();
	CHECK_MSTATUS_AND_RETURN_IT( stat );


	return stat;
}

