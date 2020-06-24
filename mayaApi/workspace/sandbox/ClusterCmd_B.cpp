/*
 * mirrorClusterCmd_B.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: rash
 */


#include "ClusterCmd_B.h"

const char* ClusterCmd_B::axisFlag = "ax";
const char* ClusterCmd_B::axisLongFlag = "axis";
float noZero(double value);

MSyntax ClusterCmd_B::newSyntax(){

	MSyntax syntax;
	MStatus stat;

	syntax.addFlag(axisFlag, axisLongFlag, MSyntax::kString);

	stat = syntax.setObjectType(MSyntax::kSelectionList, 0, 3);
	syntax.useSelectionAsDefault(true);
	syntax.setMaxObjects(3);
	syntax.setMinObjects(2);
	return syntax;
}


void *ClusterCmd_B::creator(){
	return new ClusterCmd_B;
}

MStatus ClusterCmd_B::mirrorOneCluster(MSelectionList sel, MString axis){

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
			sprintf(buf, "weights mirror: %1.1f", per);
			percent = MString(buf);
			percent += "%  ";
			MGlobal::displayInfo(percent);

		}

	}// end if type

	return MS::kSuccess;

}

MStatus ClusterCmd_B::mirrorTwoCluster(MSelectionList sel, MString axis){

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

		MIntArray indexComp, clos;
		MPointArray mPointArray, mPointArrayNew, arPt;
		MObjectArray comps;

		vertIter.allPositions(mPointArrayNew);

		MPointArray mirrorMPointArray;
		MFloatArray mirrorWts;

		float rangeCentric = 0.05;
		float rangeVert = 0.1;
		unsigned u;

		for (; !vertIter.isDone(); vertIter.next()){
			MFloatArray w;

			MObject comp = vertIter.component(&stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			stat = cluster.getWeights(mdSource, comp, w);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			MPoint pt = vertIter.position();
			pt = pt * item.inclusiveMatrix();
			int id = vertIter.index();
			u = vertIter.index();

			comps.append(comp);
			indexComp.append(id);
			mPointArray.append(pt);

			if(w[0] != 0.0){
				if ( axis == "x"){
					if(pt.x > rangeCentric){
						pt.x *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-x"){
					if(pt.x < rangeCentric){
						pt.x *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "y"){
					if(pt.y > rangeCentric){
						pt.y *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-y"){
					if(pt.y < rangeCentric){
						pt.y *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "z"){
					if(pt.z > rangeCentric){
						pt.z *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else if(axis == "-z"){
					if(pt.z < rangeCentric){
						pt.z *= -1;
						mirrorMPointArray.append(pt);
						mirrorWts.append(w[0]);
					}
				}else{
					MGlobal::displayError("Failed to specified axis!!!");
					return MS::kFailure;
				}
//				/// set all vertex zero weight
//				MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+u+"] 0.0;"));


			}
		}

		double radius = 1.0;
		MIntArray idVecs;
		MDoubleArray LenVecs;

		for (unsigned int i = 0; i < mirrorMPointArray.length(); ++i)
		{
			for (unsigned int j =0; j < mPointArrayNew.length(); ++j){

				MVector vVecs = mirrorMPointArray[i] - mPointArrayNew[j];

				if (radius > vVecs.length()){
					idVecs.append(j);
					LenVecs.append(vVecs.length());
				}

			}
		}


//		double dClosest = 100000000;	// start with something big
//		unsigned uClosest = u ;
//		unsigned int v;
//
//		for (v = 0; v< vertIter.count(); v++)
//			{
//
//
//			MPoint WorldPt = mPointArrayNew[v] * item.inclusiveMatrix();
//			MVector vDelta = pt - WorldPt ;
//			double dDist = vDelta.length() ;
//			if (dDist < dClosest)
//				{
//				dClosest = dDist ;
//				uClosest = v ;
//
//				}
//
//			} // end of v loop

		//arPt.append(mPointArrayNew[uClosest]);
		//MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+uClosest+"]"+w[0]+";"));

//		//RBF
//		for (unsigned int h=0; h<arPt.length(); h++){
//
//			for (unsigned int i = 0; i< vertIter.count(); i++){
//				MPoint WorldAllPt = mPointArrayNew[i] * item.inclusiveMatrix();
//				MPoint WorldPt = mPointArrayNew[h] * item.inclusiveMatrix();
//				MVector vDelta = WorldAllPt - WorldPt;
//				double dDist = vDelta.length();
//				double scale = 2.0;
//				if(dDist < scale){
//					double normDis = scale/dDist;
//					double gaus = exp( -1.0 * (normDis*normDis)  ) ;
//					MGlobal::executeCommand(MString("setAttr "+DFnDNClstNew.name()+".weightList[0].weights["+i+"]"+mirrorWts[i]+";"));
//				}
//
//			}
//		}


	}// end if type
	return MS::kSuccess;
}

MStatus ClusterCmd_B::doIt(const MArgList& args){

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
