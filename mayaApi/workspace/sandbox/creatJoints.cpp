/*
 * creatJonts.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: rash
 */
#include "JointCreator.h"

const char *CreatorJonts::numberFlag = "n";
const char *CreatorJonts::numberLongFlag = "number";

const char *CreatorJonts::pFlag = "p";
const char *CreatorJonts::pLongFlag = "parent";

MSyntax CreatorJonts::newSyntax(){

	MSyntax syntax;
	syntax.addFlag(numberFlag, numberLongFlag, MSyntax::kLong);
	syntax.addFlag(pFlag, pLongFlag, MSyntax::kBoolean);
	return syntax;

}

MStatus CreatorJonts::doIt(const MArgList &args){

	int rNumber = 5;
	bool parent = false;
	MQuaternion quat;
	MStatus status;

	MArgDatabase ArgData(syntax(), args);

	if( ArgData.isFlagSet(numberFlag) ){
		ArgData.getFlagArgument(numberFlag, 0, rNumber);
		MGlobal::displayInfo(MString("numberFlag ")+rNumber);
	}
	if( ArgData.isFlagSet(pFlag) ){
		ArgData.getFlagArgument(pFlag, 0, parent);
		MGlobal::displayInfo(MString("parent ")+parent);
	}

	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);

	MDagPath DagPath;
	MFnNurbsCurve curves;

	MItSelectionList iter(selection, MFn::kNurbsCurve);

	for( ; !iter.isDone(); iter.next()){

		iter.getDagPath(DagPath);
		curves.setObject(DagPath);
		double len = curves.length();
		double inLen = len/(rNumber-1);

		MPoint pt;
		MVector vec(1.0,0.0,0.0);

		MFnDagNode *dagNods = new MFnDagNode [rNumber];
		MObject *ob = new MObject [rNumber];


		for(int i=0; i<rNumber; i++){

			double par = curves.findParamFromLength(inLen*i);

			curves.getPointAtParam(par, pt, MSpace::kWorld);

			MVector v = curves.tangent(par, MSpace::kWorld);

			MFnIkJoint jointFn;

			ob[i] = jointFn.create(MObject::kNullObj, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = jointFn.setTranslation(pt,MSpace::kTransform);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = jointFn.getRotation(quat, MSpace::kTransform);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MQuaternion q = vec.rotateTo(v);

			jointFn.setRotation(q, MSpace::kTransform);

			dagNods[i].setObject(ob[i]);


		}

		if(parent){
			for (int k = 1; k<rNumber; k++){
				MString parentJnt = dagNods[k-1].fullPathName();
				MString childJnt = dagNods[k].fullPathName();
				MGlobal::executeCommand(MString("parent "+childJnt+" "+parentJnt));
				MGlobal::executeCommand(MString("rename "+parentJnt+" JointOnCurve_")+(k-1));
			}
			MGlobal::executeCommand(MString("rename "+dagNods[rNumber-1].fullPathName()+" JointOnCurve_")+(rNumber-1));
		}

		delete [] dagNods;
		delete [] ob;


	}

	return MS::kSuccess;
}


void *CreatorJonts::creator(){
	return new CreatorJonts;
}

MStatus CreatorJonts::undoIt() const{
	return MS::kSuccess;
}

MStatus CreatorJonts::redoIt() const{
	clearResult();
	setResult( (int) 1);
	return MS::kSuccess;
}
