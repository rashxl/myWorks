 /*
  * gator.cpp
  *
  *  Created on: May 29, 2017
  *      Author: rash
  */



 #include "gator.h"

 #define MERR_CHK(status,msg) if ( !status ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

 void Barycentric(MPoint p, MPoint a, MPoint b, MPoint c, float &u, float &v, float &w);
 bool BarycentricB(MPoint p, MPoint a, MPoint b, MPoint c, float &u, float &v, float &w);

 gator::gator()
 {
 }

 gator::~gator()
 {
 }

 MSyntax gator::newSyntax(){

 	MSyntax syntax;
 	MStatus stat;
 	stat = syntax.setObjectType(MSyntax::kSelectionList, 3, 3);
 	syntax.useSelectionAsDefault(true);
 	syntax.setMaxObjects(3);
 	syntax.setMinObjects(3);
 	return syntax;
 }


 void *gator::creator(){
 	return new gator;
 }





 MStatus gator::doIt(const MArgList& args){

 	MThreadUtils::syncNumOpenMPThreads();

 	#ifndef _DEBUG
 	#ifdef _OPENMP
 	omp_set_num_threads(omp_get_num_procs());
 	#endif
 	#endif

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
 //			MGlobal::displayInfo(MString("TrgetMeshPointArray.length(): ")+TrgetMeshPointArray.length());
 //			for (int h =0; h<TrgetMeshPointArray.length(); h++){
 //				MGlobal::displayInfo(MString("i: ")+h);
 //			}
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

 //			MItSelectionList itSel(obMembers);

 //			MDagPath item;
 //			MObject componentVer;
 //
 //		    for ( ; !itSel.isDone(); itSel.next() )
 //			  {
 //				  itSel.getDagPath( item, componentVer );
 //
 //			  }

 		    stat = fnSet.addMember(moTarget);
 		    CHECK_MSTATUS_AND_RETURN_IT(stat);


 		    MItMeshVertex vIter(moSource);
 		    MFnMesh meshSrs(moSource);
 		    MPointArray vertList;
 		    MIntArray indexComp;
 		    MFloatArray wts;
 		    meshSrs.getPoints(vertList);

 			#ifndef _DEBUG
 			#ifdef _OPENMP
 			#pragma omp parallel for
 			#endif
 			#endif

 		    for(int i=0; i<vertList.length(); i++){
 		    	MFloatArray w;
 		    	int preI;
 		    	stat = vIter.setIndex(i,preI);
 		    	CHECK_MSTATUS_AND_RETURN_IT(stat);
 		    	MObject comVert = vIter.currentItem(&stat);
 		    	CHECK_MSTATUS_AND_RETURN_IT(stat);
 		    	stat = cluster.getWeights(mdSource, comVert, w);
 		    	indexComp.append(vIter.index());
 		    	wts.append(w[0]);
 		    }


 			if(vertList.length() == 0){
 				MGlobal::displayError("ERROR: cluster have not deformed meshes!");
 				return MS::kFailure;
 			}


 			// -------------------------- set Weights -----------------------------------------------

 /*

 		    MItMeshVertex vIterMd(moTarget);
 		    MIntArray indexCompMd;

 		    /// iterator of source mesh
 		    MItMeshPolygon polyIter(moSource);

 //		    float allLen = TrgetMeshPointArray.length();
 //		    float p = 0.0;
 //		    MString percent = " proccesing: ";
 		    MFnTransform fnTrs(moTarget);
 		    MMatrix wordMat = fnTrs.transformation().asMatrix();

 			#ifndef _DEBUG
 			#ifdef _OPENMP
 			#pragma omp parallel for
 			#endif
 			#endif

 		    for(int i=0; i<TrgetMeshPointArray.length(); i++){



 		    	int preI;
 		    	stat = vIterMd.setIndex(i,preI);
 		    	CHECK_MSTATUS_AND_RETURN_IT(stat);
 		    	MObject comVertTr = vIterMd.currentItem(&stat);
 		    	CHECK_MSTATUS_AND_RETURN_IT(stat);

 		    	/// find closest polygon
 		    	MPoint clPoint;
 		    	int idPolygon;
 		    	meshSrs.getClosestPoint(TrgetMeshPointArray[i], clPoint, MSpace::kObject, &idPolygon);
 		    	//MGlobal::displayInfo(MString("idPolygon ")+idPolygon);
 		    	stat = polyIter.setIndex(idPolygon, preI);
 		    	int NumTriang;
 		    	polyIter.numTriangles(NumTriang);

 		    	float weightResult = 0.0f;

 				#ifndef _DEBUG
 				#ifdef _OPENMP
 				#pragma omp parallel for
 				#endif
 				#endif

 		    	for (int n = 0; n<NumTriang; n++){

 		    		MPointArray pointsTriangle;
 		    		MIntArray IdVerTriang;

 		    		float BcData[3];
 		    		BcData[0] = 0.0f;
 		    		BcData[1] = 0.0f;
 		    		BcData[2] = 0.0f;

 		    		polyIter.getTriangle (n, pointsTriangle, IdVerTriang, MSpace::kObject);
 		    		clPoint = clPoint * wordMat;
 		    		Barycentric(clPoint, pointsTriangle[0]* wordMat, pointsTriangle[1]* wordMat, pointsTriangle[2]* wordMat, BcData[0], BcData[1], BcData[2]);
 		    		float total = BcData[0] + BcData[1] + BcData[2];


 		    		if(BcData[0] >= 0.0f && BcData[1] >= 0.0f && BcData[2] >= 0.0f){
 						#ifndef _DEBUG
 						#ifdef _OPENMP
 						#pragma omp parallel for
 						#endif
 						#endif
 		    			for(int j =0; j<indexComp.length(); j++){
 		    				for(int g=0; g<3; g++){
 		    					if(IdVerTriang[g] == indexComp[j]){
 		    						weightResult += wts[j] * BcData[g];
 		    					}
 		    				}
 		    			}


 		    		}

 		    	} // end weight calculate

 		    	/// set weight
 		    	stat = cluster.setWeight(mdTarget, comVertTr, weightResult);
 		    	//MGlobal::displayInfo(MString("id: ")+i);
 		    	//MGlobal::displayInfo(MString("w: ")+weightResult);
 //				p += 1.0;
 //				char buf[256];
 //				float per = (p/allLen)*100;
 //				sprintf(buf, "proccesing: %1.1f", per);
 //				percent = MString(buf);
 //				percent += "%  ";
 //
 //				MGlobal::displayInfo(percent);

 		    }


 */

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

 				float f = gator::getNewWeights( pointInfo, moSource, wts, indexComp );

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



 float gator::getNewWeights( MPointOnMesh& info, MObject moSourceShp, MFloatArray w, MIntArray idComp)
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


