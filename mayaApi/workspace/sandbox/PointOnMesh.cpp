/*
 * PointOnMesh.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: rash
 */

#include "PointOnMesh.h"
#define MERR_CHK(status,msg) if ( !status ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

PointOnMeshCmd::PointOnMeshCmd()
{
}

PointOnMeshCmd::~PointOnMeshCmd()
{
}

MSyntax PointOnMeshCmd::newSyntax(){

	MSyntax syntax;
	MStatus stat;
	stat = syntax.setObjectType(MSyntax::kSelectionList, 3, 3);
	syntax.useSelectionAsDefault(true);
	syntax.setMaxObjects(3);
	syntax.setMinObjects(3);
	return syntax;
}


void *PointOnMeshCmd::creator(){
	return new PointOnMeshCmd;
}





MStatus PointOnMeshCmd::doIt(const MArgList& args){

	MStatus stat = MS::kSuccess;

	MArgDatabase argData(syntax(), args, &stat);

	MSelectionList sel;



	stat = argData.getObjects(sel);
	unsigned int iListLength = sel.length(&stat);

	if (iListLength == 3) {

		MPointArray TrgetMeshPointArray;
		MDoubleArray weightList;

		MFnWeightGeometryFilter cluster;

		MObject moSource, moTarget, moCluster;

		MFnDagNode dagSource, dagTarget, dagCluster;

		MDagPath mdSource, pathToMesh, mdTarget, mdCluster;

		stat = sel.getDependNode(0, moSource);
		stat = sel.getDependNode(1, moTarget);
		stat = sel.getDependNode(2, moCluster);

		stat = sel.getDagPath(0, mdSource);
		stat = sel.getDagPath(1, mdTarget);
		stat = sel.getDagPath(2, mdCluster);


		dagSource.setObject(moSource);
		dagTarget.setObject(moTarget);
		dagCluster.setObject(moCluster);


		moSource = dagSource.child(0, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		moTarget = dagTarget.child(0, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		moCluster = dagCluster.child(0, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		//MGlobal::displayInfo(moCluster.apiTypeStr());
		MGlobal::displayInfo("copy cluster weights");


		if(moSource.apiType() == MFn::kMesh && moTarget.apiType() == MFn::kMesh && moCluster.apiType() == MFn::kCluster){



			// ---------------------- find cluster node -------------------------------------------

			MFnDependencyNode DFnDNClst(moCluster, &stat), DFnDNSetClst;
			MFnDagNode FndgCluster;

			MFnMesh FnTargetShp(moTarget);
			FnTargetShp.getPoints(TrgetMeshPointArray, MSpace::kObject);

			CHECK_MSTATUS_AND_RETURN_IT(stat);

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

		    stat = fnSet.addMember(moTarget);
		    CHECK_MSTATUS_AND_RETURN_IT(stat);
			//MString melSkr = "sets -add "+DFnDNSetClst.name() + " "+dagTarget.name();
			//MGlobal::executeCommand(melSkr);
			//MGlobal::displayInfo("here MSTRINGGARAY");
			//MFnDagNode dnCluster;
			//MDagPath dgCls;
			//FndgCluster.setObject(PlugArrayClusterFind[0].node());
			//MERR_CHK(MDagPath::getAPathTo(cluster.deformerSet(&stat), dgCls), "Couldn't get a path to the mesh");

			//dnCluster.setObject(cluster.deformerSet(&stat));
			//dgCls = dnCluster.dagPath(&stat);



			MItGeometry vertIter(item, componentVer, &stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);
			MIntArray indexComp;
			MPointArray mPointArray;
			MFloatArray wts;

			for (unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++){
				MFloatArray w;

				MObject comp = vertIter.component(&stat);
				CHECK_MSTATUS_AND_RETURN_IT(stat);

				stat = cluster.getWeights(mdSource, comp, w);


				MPoint pt = vertIter.position();
				int id = vertIter.index();

				indexComp.append(id);
				mPointArray.append(pt);
				wts.append(w[0]);


			}

			/*
			for (; !vertIter.isDone(); vertIter.next()){

				MPoint pt = vertIter.position();
				int id = vertIter.index();

				indexComp.append(id);
				mPointArray.append(pt);


			}


			//-------------- get weights from source mesh -----------------------------------

			MItGeometry geomIter(mdSource, &stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			for (; !geomIter.isDone(); geomIter.next()){
				MFloatArray w;
				MObject comp = geomIter.component(&stat);
				CHECK_MSTATUS_AND_RETURN_IT(stat);

				stat = cluster.getWeights(mdSource, comp, w);
				wts.append(w[0]);
			}

			//MString h = " id: ";
			//h += indexComp[0];
			//h += " len id ";
			//h += indexComp.length();
			//MGlobal::displayInfo(h);
			*/
			if(indexComp.length() == 0){
				MGlobal::displayError("ERROR: cluster have not deformed meshes!");
				return MS::kFailure;
			}


			// -------------------------- set Weights -----------------------------------------------

			MERR_CHK(MDagPath::getAPathTo(moSource, pathToMesh), "Couldn't get a path to the mesh");



			MMeshIntersector intersector;
			MMatrix matrix = pathToMesh.inclusiveMatrix();
			MObject node = pathToMesh.node();
			stat = intersector.create( node, matrix );
			MPointOnMesh pointInfo;

			MItGeometry TargetGeomIter(mdTarget, &stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);
//			float allLen = TrgetMeshPointArray.length();
//			float p = 0.0;
//			MString percent = " proccesing: ";
			for (; !TargetGeomIter.isDone(); TargetGeomIter.next()){

				MObject comp = TargetGeomIter.component(&stat);
				CHECK_MSTATUS_AND_RETURN_IT(stat);
				MPoint pt =  TargetGeomIter.position();

				stat = intersector.getClosestPoint( pt, pointInfo );
				CHECK_MSTATUS_AND_RETURN_IT(stat);

				float f = PointOnMeshCmd::getNewWeights( pointInfo, moSource, wts, indexComp );

                if (f > 1){
                    f = 1.0;
                }else if(f < 0){
                    f = 0;
                }

				stat = cluster.setWeight(mdTarget, comp, f);

//				p += 1.0;
//				char buf[256];
//				float per = (p/allLen)*100;
//				sprintf(buf, "proccesing: %1.1f", per);
//				percent = MString(buf);
//				percent += "%  ";
//
//				MGlobal::displayInfo(percent);

			}

		}

	}
	MGlobal::displayInfo("Done");
	return MS::kSuccess;
}



float PointOnMeshCmd::getNewWeights( MPointOnMesh& info, MObject moSourceShp, MFloatArray w, MIntArray idComp)
{
	float bcU;
	float bcV;
	float bcC;

	float BcData[3];

	int verList[3];

	MDoubleArray arrayResult;

	MFnMesh meshes(moSourceShp);


	info.getBarycentricCoords(bcU, bcV);

	bcC = 1 - (bcU + bcV);

	BcData[0] = bcU;
	BcData[1] = bcV;
	BcData[2] = bcC;


	meshes.getPolygonTriangleVertices(info.faceIndex(), info.triangleIndex(), verList);

	float res = 0.0;

	for (unsigned int j = 0; j < idComp.length(); j++){
		for(unsigned int i=0; i < 3; i++){
			if (idComp[j] == verList[i]){
				res += w[j] * BcData[i];
			}
		}
	}

//	for (unsigned int i=0; i < 3; i++){
//		for(unsigned int j = 0; j < idComp.length(); j++){
//			if (idComp[j] == verList[i]){
//				res += w[j] * BcData[i];
//			}
//		}
//	}

	return res;

}


