#include "meshClone.h"


MTypeId meshClone::id(0x30860403);
MObject meshClone::inMesh;
MObject meshClone::outMesh;
MObject meshClone::count;
MObject meshClone::inCurv;
MObject meshClone::rootA;
MObject meshClone::rootB;
MObject meshClone::crvStart;
MObject meshClone::crvEnd;
MObject meshClone::offset;

void * meshClone::creator(){
	return new meshClone;
}


double meshClone::quaternionDot(MQuaternion a, MQuaternion b){
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

MQuaternion meshClone::quaternionSlerp(MQuaternion a, MQuaternion b, double t){

	double cos = quaternionDot(a, b);

	if(cos < 0.0){
		cos = quaternionDot(a, b.negateIt());
	}else if (cos > 1){
		cos = 1.0;
	}

	double theta = acos(cos);// * 180.0 / PI;
	double sin = asin(theta);

	double w1;
	double w2;
    if(sin > 0.001){
        w1 = asin((1.0 - t) * theta) / sin;
        w2 = asin(t * theta) / sin;
	}
    else{
       w1 = 1.0 - t;
       w2 = t;
	}
    MQuaternion aa = a;
    MQuaternion bb = b;

    aa.scaleIt(w1);
    bb.scaleIt(w2);

    return aa + bb;
}


MStatus meshClone::compute(const MPlug &plug, MDataBlock &data){
	MStatus stat;

	if(plug != outMesh){
		return MS::kUnknownParameter;
	}

	MMatrix mInputRootA = data.inputValue(rootA, &stat).asMatrix();

	MMatrix mRA;

	mRA[1][1] = mInputRootA[1][1];
	mRA[2][1] = mInputRootA[2][1];

	mRA[1][2] = mInputRootA[1][2];
	mRA[2][2] = mInputRootA[2][2];

	MTransformationMatrix mTransMInputRootA(mRA);
	MQuaternion qInputRootA = mTransMInputRootA.rotation();

	MMatrix mInputRootB = data.inputValue(rootB, &stat).asMatrix();

	MMatrix mRB;

	mRB[1][1] = mInputRootB[1][1];
	mRB[2][1] = mInputRootB[2][1];

	mRB[1][2] = mInputRootB[1][2];
	mRB[2][2] = mInputRootB[2][2];

	MTransformationMatrix mTransMInputRootB(mRB);
	MQuaternion qInputRootB = mTransMInputRootB.rotation();


	unsigned int numCount = data.inputValue(count, &stat).asInt();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MObject inCrvObj = data.inputValue(inCurv, &stat).asNurbsCurve();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MObject inMeshObj = data.inputValue(inMesh, &stat).asMesh();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MDataHandle outMeshHandle = data.outputValue(outMesh, &stat);

	MFnMesh soursFnMesh(inMeshObj);

	MFnNurbsCurve curve(inCrvObj);

	//MFloatPointArray m_vertices;
	MPointArray m_vertices;
	unsigned int m_numVertices;
	MIntArray m_counts;
	MIntArray m_connects;

	MFloatArray m_Us;
	MFloatArray m_Vs;
	MIntArray m_uvCounts;
	MIntArray m_uvIds;

	stat = soursFnMesh.getUVs(m_Us, m_Vs);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = soursFnMesh.getAssignedUVs(m_uvCounts, m_uvIds);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	m_numVertices = soursFnMesh.numVertices(&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = soursFnMesh.getPoints(m_vertices, MSpace::kObject);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = soursFnMesh.getVertices(m_counts, m_connects);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MIntArray flockFaceCounts;
	MIntArray flockConnectivity;
	MFloatPointArray flockVertices;
	MIntArray flockUVCounts;
	MIntArray flockUVIds;

	double curveLength = curve.length();
	double nStrat, nEnd;
	curve.getKnotDomain(nStrat, nEnd);
	double tEnd = data.inputValue(crvEnd, &stat).asFloat() * curveLength;
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	double tStart = data.inputValue(crvStart, &stat).asFloat() * curveLength;
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	double tOffset = data.inputValue(offset, &stat).asFloat();
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	double tInc = (tEnd-tStart)/(numCount-1);
	double t = tStart;

	double endT;
	double startT;
	curve.getKnotDomain(startT, endT);
	double increment = (endT-startT)/(numCount-1);

	double effectiveCurveLength = min(max(tEnd-tStart, 0.001), curveLength);
	double lengthIncrement = effectiveCurveLength/float(numCount);
	double maxParam = curve.findParamFromLength(curveLength);


	MVector vec(1.0,0.0,0.0);

	unsigned index = 0;
	////////////////// POINTS //////////////////////
	unsigned nPoints =  m_vertices.length() ;
	flockVertices.setLength(nPoints * numCount);
	for (unsigned int i = 0; i<numCount; i++) {


		double dist = fmod(tStart + fmod(lengthIncrement * i + tOffset, effectiveCurveLength), curveLength);
        double param = max( min( curve.findParamFromLength( dist ), maxParam ), 0.0);
		//t = fmod(tStart+fmod(tInc * i + tOffset, nEnd+0.0001), nEnd);
		//t =  fmod(tInc * i + fmod(tOffset, 1.0), nEnd+0.0001);
		//MGlobal::displayInfo(MString("fmod(tInc * i + tOffset, nEnd+0.0001): ")+fmod(lengthIncrement * i + tOffset, effectiveCurveLength));
		//MGlobal::displayInfo(MString("param: ")+param);
		//MGlobal::displayInfo(MString("maxParam: ")+maxParam);
		//MGlobal::displayInfo(MString("maxParam/param: ")+maxParam/param);
		//MGlobal::displayInfo(MString("1/param: ")+1/param);
		//MGlobal::displayInfo(MString("endT: ")+endT);
		//MGlobal::displayInfo(MString("increment: ")+increment);
		//MGlobal::displayInfo(MString("t: ")+t);
		//MGlobal::displayInfo(MString("float(numCount/i): ")+float(numCount));
		//MGlobal::displayInfo(MString("float(numCount/i): ")+(float(i)+1.0)/float(numCount));
		t+=increment;
        ///POSE
		MPoint pt;
		curve.getPointAtParam(param, pt, MSpace::kObject);
		//ROTATE
		MVector v = curve.tangent(param, MSpace::kWorld);
		MQuaternion q1 = vec.rotateTo(v);
		MQuaternion Pq1;

		Pq1 = quaternionSlerp(qInputRootA, qInputRootB, (float(i)+1.0)/float(numCount));


		q1 = Pq1.normalizeIt() * q1;

		q1.normalizeIt();

		MMatrix Mq = q1.asMatrix();

		Mq[3][0] = pt.x;
		Mq[3][1] = pt.y;
		Mq[3][2] = pt.z;

		//Mm[0][0] = Mq[0][0];
		//Mm[0][1] = Mq[0][1];
		//Mm[0][2] = Mq[0][2];

		//Mm[1][0] = Mq[1][0];
		//Mm[1][1] = Mq[1][1];
		//Mm[1][2] = Mq[1][2];

		//Mm[2][0] = Mq[2][0];
		//Mm[2][1] = Mq[2][1];
		//Mm[2][2] = Mq[2][2];


		//Mm[3][0] = Mm[3][0] + pt.x;
		//Mm[3][1] = Mm[3][1] + pt.y;
		//Mm[3][2] = Mm[3][2] + pt.z;

		for (unsigned j = 0;j<nPoints;j++) {
			MPoint m_res;
			m_res = m_vertices[j] * Mq;
			//m_res = m_vertices[j] + pt;
			flockVertices.set(m_res,index);
			index++;
		}

	}


	////////////////// COUNTS //////////////////////
	index = 0;
	unsigned nCounts =  m_counts.length() ;
	flockFaceCounts.setLength(nCounts * numCount);
	for (unsigned i = 0;i<numCount;i++) {
		for (unsigned j = 0;j<nCounts;j++) {
			flockFaceCounts.set(m_counts[j],index);
			index++;
		}
	}

	////////////////// CONNECTS //////////////////////
	index = 0;
	unsigned nConnects = (m_connects.length() );
	flockConnectivity.setLength(nConnects * numCount);
	for (unsigned i = 0;i<numCount;i++) {
		unsigned startIndex = i*m_numVertices;
		for (unsigned j = 0;j<nConnects;j++) {
			flockConnectivity.set( ( startIndex + m_connects[j] ), index);
			index++;
		}
	}

	MFnMeshData mFnMeshData;

	MObject outMesh = mFnMeshData.create(&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	MFnMesh outFnMesh;
	outFnMesh.create(m_numVertices * numCount, flockFaceCounts.length(), flockVertices, flockFaceCounts, flockConnectivity, outMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///////////////// ASSIGN UVS /////////////////////
	unsigned nUVCounts = m_uvCounts.length();
	if (nUVCounts == nCounts) {

		for (unsigned i = 0;i<numCount;i++) {// number of agents
			for (unsigned j = 0;j<nUVCounts;j++) { // number of polygons in one agent
				flockUVCounts.append(m_uvCounts[j]);
			}
		}

		unsigned nUVIds = (m_uvIds.length() );
		for (unsigned i = 0;i<numCount;i++) {
			for (unsigned j = 0;j<nUVIds;j++) {
				flockUVIds.append(m_uvIds[j]);
			}
		}
		stat = outFnMesh.setUVs(m_Us, m_Vs);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		stat = outFnMesh.assignUVs ( flockUVCounts, flockUVIds );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

	} else {
		cerr << "nUVCounts counts don't match nCounts: " <<  nUVCounts << " " <<  nCounts << endl;
		MGlobal::displayError(MString("nUVCounts counts don't match nCounts: ")+nUVCounts +" "+nCounts);
	}
	//////////////////////////////////////

	stat = outMeshHandle.set(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	data.setClean(plug);

	return MS::kSuccess;
}

MStatus meshClone::initialize(){
	MStatus stat;

	MFnNumericAttribute fnNumAttr;
	MFnTypedAttribute fnTypeAttr;
	MFnMatrixAttribute fnMatAttr;

	rootA = fnMatAttr.create("rA","rootA");
	CHECK_MSTATUS(addAttribute(rootA));

	rootB = fnMatAttr.create("rB","rootB");
	CHECK_MSTATUS(addAttribute(rootB));

	crvStart = fnNumAttr.create("tStart", "tStart", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumAttr.setSoftMax(1.0));
	CHECK_MSTATUS(fnNumAttr.setKeyable(true));
	CHECK_MSTATUS(addAttribute(crvStart));

	crvEnd = fnNumAttr.create("tEnd", "tEnd", MFnNumericData::kFloat, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumAttr.setSoftMax(1.0));
	CHECK_MSTATUS(fnNumAttr.setKeyable(true));
	CHECK_MSTATUS(addAttribute(crvEnd));

	offset = fnNumAttr.create("offset", "offset", MFnNumericData::kFloat, 0.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumAttr.setKeyable(true));
	CHECK_MSTATUS(addAttribute(offset));

	count = fnNumAttr.create("count", "count", MFnNumericData::kInt, stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnNumAttr.setMin(1));
	CHECK_MSTATUS(fnNumAttr.setDefault(1));
	CHECK_MSTATUS(addAttribute(count));

	inCurv = fnTypeAttr.create("inCrv", "inCrv",  MFnNurbsCurveData::kNurbsCurve, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnTypeAttr.setStorable(true));
	CHECK_MSTATUS(addAttribute(inCurv));


	inMesh = fnTypeAttr.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnTypeAttr.setStorable(true));
	CHECK_MSTATUS(addAttribute(inMesh));

	outMesh = fnTypeAttr.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(fnTypeAttr.setStorable(true));
	CHECK_MSTATUS(addAttribute(outMesh));

	stat = attributeAffects(count, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(inMesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(inCurv, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(crvStart, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(crvEnd, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(offset, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(rootA, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects(rootB, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;

}
