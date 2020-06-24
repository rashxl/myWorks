/*
 * mirrorClusterCmd.cpp
 *
 *  Created on: Oct 21, 2016
 *      Author: rash
 *
 *     description: For one cluster, select first mesh then cluster
 *     				for two cluster select first mesh then source cluster then new cluster
 */




#include "mirrorClusterCmd.h"

const char* mirrorClusterCmd::axisFlag = "ax";
const char* mirrorClusterCmd::axisLongFlag = "axis";
float noZero(double value);

MSyntax mirrorClusterCmd::newSyntax(){

	MSyntax syntax;
	MStatus stat;

	syntax.addFlag(axisFlag, axisLongFlag, MSyntax::kString);

	stat = syntax.setObjectType(MSyntax::kSelectionList, 0, 3);
	syntax.useSelectionAsDefault(true);
	syntax.setMaxObjects(3);
	syntax.setMinObjects(2);
	return syntax;
}


void *mirrorClusterCmd::creator(){
	return new mirrorClusterCmd;
}

MStatus mirrorClusterCmd::mirrorOneCluster(MSelectionList sel, MString axis){

	MStatus stat;

	MPointArray TrgetMeshPointArray;
	MDoubleArray weightList;

	MFnWeightGeometryFilter cluster;

	MObject moSource, moCluster;

	MFnDagNode dagSource, dagCluster;

	MDagPath mdSource, mdCluster;

	stat = sel.getDependNode(0, moSource);
	stat = sel.getDependNode(1, moCluster);

	stat = sel.getDagPath(0, mdSource);
	stat = sel.getDagPath(1, mdCluster);


	dagSource.setObject(moSource);
	dagCluster.setObject(moCluster);


	moSource = dagSource.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	moCluster = dagCluster.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);



	if(moSource.apiType() == MFn::kMesh && moCluster.apiType() == MFn::kCluster){


		// ---------------------- find cluster node -------------------------------------------

		MFnDependencyNode DFnDNClst(moCluster, &stat), DFnDNSetClst;
		//MFnDagNode FndgCluster;

		MPlug PlugCluster, PlugClusterFind;
		MPlugArray PlugArrayCluster, PlugArrayClusterFind;

		PlugCluster = DFnDNClst.findPlug("clusterTransforms", &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugCluster = PlugCluster.elementByLogicalIndex(0, &stat);

		PlugCluster.connectedTo(PlugArrayCluster, 0, 1, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugCluster = PlugArrayCluster[0];
		DFnDNClst.setObject(PlugCluster.node());

		stat = cluster.setObject(DFnDNClst.object());
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		////get number connections from cluster and check
		unsigned nGeometryInCluster;
		nGeometryInCluster =  cluster.numOutputConnections();

		if (nGeometryInCluster == 0){
			MGlobal::displayError("ERROR: cluster have not deformed meshes!");
			return MS::kFailure;
		}

		////END get number connections from cluster and check

		//// ------- find cluster set
		PlugClusterFind = DFnDNClst.findPlug("message", &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugClusterFind.connectedTo(PlugArrayClusterFind, 0, 1, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		DFnDNSetClst.setObject(PlugArrayClusterFind[0].node());
		MFnSet fnSet( cluster.deformerSet(&stat), &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MSelectionList obMembers;

		fnSet.getMembers( obMembers, true );

		MItSelectionList itSel(obMembers);

		MDagPath item;
		MObject componentVer;

	    for ( ; !itSel.isDone(); itSel.next() )
		  {
			  itSel.getDagPath( item, componentVer );

		  }

		MItGeometry vertIter(item, componentVer, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MIntArray indexComp;
		MPointArray mPointArray;
		MObjectArray comps;


		MPointArray mirrorMPointArray;
		MFloatArray mirrorWts;

		float rangeCentric = 0.05;
		float rangeVert = 0.05;

		for (; !vertIter.isDone(); vertIter.next()){
			MFloatArray w;

			MObject comp = vertIter.component(&stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			stat = cluster.getWeights(mdSource, comp, w);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			MPoint pt = vertIter.position();
			int id = vertIter.index();

			comps.append(comp);
			indexComp.append(id);
			mPointArray.append(pt);
			if(w[0] != 0.0){
				if ( axis == "x"){
					if(pt.x > rangeCentric){
						pt.x = -pt.x;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-x"){
					if(pt.x < rangeCentric){
						pt.x = -pt.x;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "y"){
					if(pt.y > rangeCentric){
						pt.y = -pt.y;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-y"){
					if(pt.y < rangeCentric){
						pt.y = -pt.y;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "z"){
					if(pt.z > rangeCentric){
						pt.z = -pt.z;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-z"){
					if(pt.z < rangeCentric){
						pt.z = -pt.z;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else{
					MGlobal::displayError("Failed to specified axis!!!");
					return MS::kFailure;
				}
			}
		}
		unsigned int allLen = comps.length();
		float p = 0.0;
		MString percent = " #mirroring cluster weights: ";

		for (unsigned int i = 0; i< comps.length(); i++){

			for(unsigned int j=0; j< mirrorMPointArray.length(); j++){

				MVector MVecLenth = mirrorMPointArray[j] - mPointArray[i];

				if(MVecLenth.length() < rangeVert){
					stat = cluster.setWeight(mdSource, comps[i], mirrorWts[j]);

				}
			}

			p += 1.0;
			char buf[256];
			float per = (p/allLen)*100;
			sprintf_s(buf, "weights mirror: %1.1f", per);
			percent = MString(buf);
			percent += "%  ";
			MGlobal::displayInfo(percent);

		}

	}// end if type

	return MS::kSuccess;

}

MStatus mirrorClusterCmd::mirrorTwoCluster(MSelectionList sel, MString axis){

	MStatus stat;

	MPointArray TrgetMeshPointArray;
	MDoubleArray weightList;

	MFnWeightGeometryFilter cluster, clusterNew;

	MObject moSource, moCluster, moClusterNew;

	MFnDagNode dagSource, dagCluster, dagClusterNew;

	MDagPath mdSource, mdCluster, mdClusterNew;

	stat = sel.getDependNode(0, moSource);
	stat = sel.getDependNode(1, moCluster);
	stat = sel.getDependNode(2, moClusterNew);

	stat = sel.getDagPath(0, mdSource);
	stat = sel.getDagPath(1, mdCluster);
	stat = sel.getDagPath(2, mdClusterNew);


	dagSource.setObject(moSource);
	dagCluster.setObject(moCluster);
	dagClusterNew.setObject(moClusterNew);


	moSource = dagSource.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	moCluster = dagCluster.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	moClusterNew = dagClusterNew.child(0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	if(moSource.apiType() == MFn::kMesh && moCluster.apiType() == MFn::kCluster && moClusterNew.apiType() == MFn::kCluster){


		// ---------------------- find cluster node -------------------------------------------



		MFnDependencyNode DFnDNClst(moCluster, &stat), DFnDNClstNew(moClusterNew), DFnDNSetClst, DFnDNSetClstNew;


		MPlug PlugCluster, PlugClusterNew, PlugClusterFind, PlugClusterFindNew;
		MPlugArray PlugArrayCluster, PlugArrayClusterNew, PlugArrayClusterFind, PlugArrayClusterFindNew;

		///#### first cluster ///

		PlugCluster = DFnDNClst.findPlug("clusterTransforms", &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugCluster = PlugCluster.elementByLogicalIndex(0, &stat);

		PlugCluster.connectedTo(PlugArrayCluster, 0, 1, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugCluster = PlugArrayCluster[0];
		DFnDNClst.setObject(PlugCluster.node());

		stat = cluster.setObject(DFnDNClst.object());
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		///#### second cluster ///

		PlugClusterNew = DFnDNClstNew.findPlug("clusterTransforms", &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugClusterNew = PlugClusterNew.elementByLogicalIndex(0, &stat);

		PlugClusterNew.connectedTo(PlugArrayClusterNew, 0, 1, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugClusterNew = PlugArrayClusterNew[0];
		DFnDNClstNew.setObject(PlugClusterNew.node());

		///stat = clusterNew.setObject(DFnDNClstNew.object());
		///CHECK_MSTATUS_AND_RETURN_IT(stat);

		////#### get number connections from cluster and check

		unsigned nGeometryInCluster;

		nGeometryInCluster =  cluster.numOutputConnections();

		if (nGeometryInCluster == 0){
			MGlobal::displayError("ERROR: cluster have not deformed meshes!");
			return MS::kFailure;
		}

		////END get number connections from cluster and check

		//// ------- find cluster set
		PlugClusterFind = DFnDNClst.findPlug("message", &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		PlugClusterFind.connectedTo(PlugArrayClusterFind, 0, 1, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		DFnDNSetClst.setObject(PlugArrayClusterFind[0].node());

		MFnSet fnSet( cluster.deformerSet(&stat), &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MSelectionList obMembers;

		fnSet.getMembers( obMembers, true );

		MItSelectionList itSel(obMembers);

		MDagPath item;
		MObject componentVer;

	    for ( ; !itSel.isDone(); itSel.next() )
		  {
			  itSel.getDagPath( item, componentVer );

		  }

		MItGeometry vertIter(item, componentVer, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MIntArray indexComp;
		MPointArray mPointArray;
		MObjectArray comps;


		MPointArray mirrorMPointArray;
		MFloatArray mirrorWts;

		float rangeCentric = 0.05;
		float rangeVert = 0.1;

		for (; !vertIter.isDone(); vertIter.next()){
			MFloatArray w;

			MObject comp = vertIter.component(&stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			stat = cluster.getWeights(mdSource, comp, w);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			MPoint pt = vertIter.position();
			int id = vertIter.index();

			comps.append(comp);
			indexComp.append(id);
			mPointArray.append(pt);
			if(w[0] != 0.0){
				if ( axis == "x"){
					if(pt.x > rangeCentric){
						pt.x = -pt.x;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-x"){
					if(pt.x < rangeCentric){
						pt.x = -pt.x;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "y"){
					if(pt.y > rangeCentric){
						pt.y = -pt.y;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-y"){
					if(pt.y < rangeCentric){
						pt.y = -pt.y;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "z"){
					if(pt.z > rangeCentric){
						pt.z = -pt.z;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-z"){
					if(pt.z < rangeCentric){
						pt.z = -pt.z;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else{
					MGlobal::displayError("Failed to specified axis!!!");
					return MS::kFailure;
				}
			}
		}
		unsigned int allLen = mirrorMPointArray.length();

		float p = 0.0;
		MString percent = " #mirroring cluster weights: ";

		/// set all vertex zero weight
		for (unsigned int i = 1; i< comps.length(); i++){
			MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+i+"] 0.0;"));
		}

		/// set weight from closest vertex

		for(unsigned int j=0; j< mirrorMPointArray.length(); j++){

			MVector MVecLenthTemp = mPointArray[0] - mirrorMPointArray[j];
			double minV = MVecLenthTemp.length();
			unsigned int g = 0;

			for (unsigned int i = 1; i< comps.length(); i++){

				MVector MVecLenth = mPointArray[i] - mirrorMPointArray[j];

				/// get min value
				if(MVecLenth.length()<minV){

						minV = MVecLenth.length();
						g = i;
				}

			}///exp(mirrorWts[j]-exp(minV)


			MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+g+"]"+(mirrorWts[j]+((mirrorWts[j]/mirrorMPointArray[j].x)*minV))+";"));



			p += 1.0;
			char buf[256];
			float per = (p/allLen)*100;
			sprintf_s(buf, "weights mirror: %1.1f", per);
			percent = MString(buf);
			percent += "%  ";
			MGlobal::displayInfo(percent);

		}

//		for (unsigned int i = 0; i< comps.length(); i++){
//			MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+i+"] 0.0;"));
//
//			MVector MVecLenthTemp = mPointArray[i] - mirrorMPointArray[0];
//			double minV = MVecLenthTemp.length();
//			unsigned int g = 0;
//
//			for(unsigned int j=1; j< mirrorMPointArray.length(); j++){
//
//				MVector MVecLenth = mPointArray[i] - mirrorMPointArray[j];
//
//				/// get min value
//				if(MVecLenth.length()<minV){
//
//						minV = MVecLenth.length();
//						g = j;
//				}
//
//
////				if(MVecLenth.length() < rangeVert){
////					MGlobal::displayInfo(MString("MVecLenth.length() ")+MVecLenth.length());
////					MGlobal::displayInfo(MString("id ")+j);
////					MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+i+"]"+mirrorWts[j]+";"));
////					//stat = cluster.setWeight(mdSource, comps[i], mirrorWts[j]);
////					//CHECK_MSTATUS_AND_RETURN_IT(stat);
////				}
//
//			}
//
//			if(g != 0){
//				MGlobal::displayInfo(MString("g ")+g);
//				MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+i+"]"+mirrorWts[g]+";"));
//			}
//
//			p += 1.0;
//			char buf[256];
//			float per = (p/allLen)*100;
//			sprintf(buf, "weights mirror: %1.1f", per);
//			percent = MString(buf);
//			percent += "%  ";
//			MGlobal::displayInfo(percent);
//
//		}

	}// end if type
	return MS::kSuccess;
}

MStatus mirrorClusterCmd::doIt(const MArgList& args){

	MStatus stat;

	MString axis;


	MArgDatabase argData(syntax(), args, &stat);

	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MSelectionList sel;

	stat = argData.getObjects(sel);

	unsigned int iListLength = sel.length();

	if (iListLength == 2) {

		if(argData.isFlagSet(axisFlag)){

			stat = argData.getFlagArgument(axisFlag, 0, axis);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

		}

		stat = mirrorOneCluster(sel, axis);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
	}

	if (iListLength == 3) {

		MGlobal::displayInfo("33");

		if(argData.isFlagSet(axisFlag)){

			stat = argData.getFlagArgument(axisFlag, 0, axis);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

		}

		stat = mirrorTwoCluster(sel, axis);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

	}

	MGlobal::displayInfo("Done");
	return MS::kSuccess;
}
