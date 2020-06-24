/*
 * rbfBlend.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: rash
 */




#include "rbfBlend.h"

float GetDistByTwoMat(MMatrix matA, MMatrix matB);
float clamping(float val, float max, float min);
float dotP(MVector A, MVector B);



MObject rbfBlend::aPoseXform;		// pose compound
MObject rbfBlend::aDriver;		// curent driver
MObject rbfBlend::aSigma;		// sigma func.
MObject rbfBlend::aWeight;		// weight
MObject rbfBlend::aAxis;		// axis
MObject rbfBlend::aResult;		// result
MObject rbfBlend::aMethod;		// choose method
MObject rbfBlend::aTypes;
MObject rbfBlend::aMode;		// choose method
MObject rbfBlend::resWeight;	// result Pose weight
//MObject rbfBlend::trans;
//MObject rbfBlend::transX;
//MObject rbfBlend::transY;
//MObject rbfBlend::transZ;




// constructor
rbfBlend::rbfBlend(){

}

//destructor
rbfBlend::~rbfBlend(){

}

//for maya
void * rbfBlend::creator(){
	return new rbfBlend;
}


MStatus rbfBlend::initialize(){
	MStatus stat;

	MFnMatrixAttribute mAttr;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;


	aPoseXform = mAttr.create("PoseXform", "pxf", MFnMatrixAttribute::kDouble);
	mAttr.setArray(true);
	mAttr.setUsesArrayDataBuilder(true);

    aWeight = nAttr.create("inWeight", "iw", MFnNumericData::kDouble, 1.0);
    nAttr.setArray(true);
    nAttr.setUsesArrayDataBuilder(true);


	aSigma = nAttr.create("eps", "eps", MFnNumericData::kDouble, 1.0 );
	nAttr.setKeyable(true);


	aMethod = eAttr.create( "method", "mtd", 0);
	eAttr.addField("Angular", 0);
	eAttr.addField("Translate", 1);

	aMode = eAttr.create( "blendMode", "mod", 0);
	eAttr.addField("Normalize-RBF", 0);
	eAttr.addField("RBF", 1);

	aTypes = eAttr.create( "type", "tp", 0);
	eAttr.addField("Gaussian", 0);
	eAttr.addField("Multiqaudric", 1);
	eAttr.addField("Inverse quadratic", 2);
	eAttr.addField("Inverse multiquadric", 3);
	eAttr.addField("Thin plate spline", 4);


	aAxis = eAttr.create( "AxisAngularBlend", "aab", 0);
	eAttr.addField("X-Axis", 0);
	eAttr.addField("Y-Axis", 1);
	eAttr.addField("Z-Axis", 2);

	aDriver = mAttr.create("curentDriver", "cd", MFnMatrixAttribute::kDouble);
	mAttr.setConnectable(true);

	aResult = nAttr.create("result", "rs", MFnNumericData::kDouble, 0.0 );

	resWeight = nAttr.create("outWeight", "ow", MFnNumericData::kDouble);
	nAttr.setArray(true);
	nAttr.setUsesArrayDataBuilder(true);

//	transX = nAttr.create("transX", "transX", MFnNumericData::kDouble);
//	transY = nAttr.create("transY", "transY", MFnNumericData::kDouble);
//	transZ = nAttr.create("transZ", "transZ", MFnNumericData::kDouble);
//
//	trans = cAttr.create("trans", "trans");
//    cAttr.addChild( transX ) ;
//    cAttr.addChild( transY ) ;
//    cAttr.addChild( transZ ) ;



	stat = addAttribute( aSigma );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aMethod );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aMode );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aTypes );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aAxis );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aPoseXform );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute( aDriver );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
//	stat = addAttribute( trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// Affect them
	stat = attributeAffects( aSigma, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aAxis, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aMethod, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aMode, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aTypes, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aWeight, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aPoseXform, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aDriver, resWeight );
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	stat = attributeAffects( aSigma, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aAxis, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aMethod, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aMode, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aTypes, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aWeight, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aPoseXform, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( aDriver, aResult );
	CHECK_MSTATUS_AND_RETURN_IT(stat);

//	stat = attributeAffects( aSigma, trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);
//	stat = attributeAffects( aAxis, trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);
//	stat = attributeAffects( aWeight, trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);
//	stat = attributeAffects( aPoseXform, trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);
//	stat = attributeAffects( aDriver, trans );
//	CHECK_MSTATUS_AND_RETURN_IT(stat);

	return MS::kSuccess;
}


MStatus rbfBlend::compute(const MPlug &plug, MDataBlock & data){
	MStatus stat;

	if(plug == resWeight ){

	    unsigned int uMatPose = 0; 			// How big is array?
		MDoubleArray dArrWts, aArrWts;
		MVectorArray vArrRead;
		MVector vCur;

		MMatrixArray pPose;
		MMatrix pDrv;

	    stat = readMatrixArray(data, dArrWts, vArrRead, vCur, pPose, pDrv, uMatPose);
	    aArrWts = dArrWts;

//	    for(int i = 0; i<uMatPose; i++){
//	    	MGlobal::displayInfo(MString("pPose[i].x ")+pPose[i].x);
//	    	MGlobal::displayInfo(MString("pPose[i].y ")+pPose[i].y);
//	    	MGlobal::displayInfo(MString("pPose[i].z ")+pPose[i].z);
//	    }

	    double sigm = data.inputValue(aSigma).asDouble();

	    stat = interpWeights(data, dArrWts, vArrRead, vCur, pPose, pDrv, sigm);

	    MArrayDataHandle outputArrayWeightHandle = data.outputValue(resWeight, &stat);
	    CHECK_MSTATUS_AND_RETURN_IT(stat);

	    MArrayDataBuilder outputArrayWeightBuilder(resWeight, uMatPose, &stat);
	    double allWeight = 0;
	    for (unsigned int i =0; i<uMatPose; ++i){
	    	MDataHandle outWeightHandle = outputArrayWeightBuilder.addElement(i);
			outWeightHandle.set(dArrWts[i]*aArrWts[i]);
			allWeight += dArrWts[i]*aArrWts[i];
	    }

		stat = outputArrayWeightHandle.set(outputArrayWeightBuilder);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		stat = outputArrayWeightHandle.setAllClean();
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		/// set result attribute
		MDataHandle aResultHandle = data.outputValue(aResult);
		aResultHandle.set(allWeight);
		aResultHandle.setClean();

//		/// set result attribute
//		MDataHandle aResultMatHandle = data.outputValue(trans);
//		aResultMatHandle.set(vCur.x, vCur.y, vCur.z);
//		aResultMatHandle.setClean();

		data.setClean(plug);

	}else{
		return MS::kUnknownParameter;
	}

/*
	if(plug == result ){

		MDataHandle matCurHandle = data.inputValue( CurV );
		MMatrix matCur = matCurHandle.asMatrix();

		MDataHandle matDefhandle = data.inputValue( PosDef );
		MMatrix matDef = matDefhandle.asMatrix();

		MDataHandle matAhandle = data.inputValue( PosA );
		MMatrix matA = matAhandle.asMatrix();

		MDataHandle matBhandle = data.inputValue( PosB );
		MMatrix matB = matBhandle.asMatrix();

		MDataHandle matChandle = data.inputValue( PosC );
		MMatrix matC = matChandle.asMatrix();

		double SG = data.inputValue(sigma).asDouble();

		double wDef = data.inputValue(WtsDef).asDouble();
		double wA = data.inputValue(WtsA).asDouble();
		double wB = data.inputValue(WtsB).asDouble();
		double wC = data.inputValue(WtsC).asDouble();

		double disDef = GetDistByTwoMat(matCur, matDef);
		double disA = GetDistByTwoMat(matCur, matA);
		double disB = GetDistByTwoMat(matCur, matB);
		double disC = GetDistByTwoMat(matCur, matC);

		MDoubleArray dArrDis, dArrWts;
		unsigned uWts = 4;

		dArrDis.append(disDef);
		dArrDis.append(disA);
		dArrDis.append(disB);
		dArrDis.append(disC);

		dArrWts.append(wDef);
		dArrWts.append(wA);
		dArrWts.append(wB);
		dArrWts.append(wC);

		double dTotal = 0.0 ;
		for (unsigned u=0; u < uWts; ++u)
			dTotal += dArrDis[u] ;			// Add up totals

		for (unsigned u=0; u < uWts; ++u){
			dArrDis[u] = dArrDis[u] / dTotal ;
		}

		double Sigma2 = -(SG*SG);

		for (unsigned u=0; u < uWts; ++u){
			dArrDis[u] = exp(Sigma2*(dArrDis[u]*dArrDis[u])) ;
		}

		double res;
		for (unsigned u =0; u<uWts; ++u){
			dArrWts[u] *= dArrDis[u];
			res += dArrWts[u];
		}
//		MGlobal::displayInfo(MString("disDef ")+dArrDis[0]);
//		MGlobal::displayInfo(MString("disA ")+dArrDis[1]);
//		MGlobal::displayInfo(MString("disB ")+dArrDis[2]);
//		MGlobal::displayInfo(MString("disC ")+dArrDis[3]);
//
//		MGlobal::displayInfo(MString("wDef ")+dArrWts[0]);
//		MGlobal::displayInfo(MString("wA ")+dArrWts[1]);
//		MGlobal::displayInfo(MString("wB ")+dArrWts[2]);
//		MGlobal::displayInfo(MString("wC ")+dArrWts[3]);
		/// set result
		MDataHandle resHandle = data.outputValue(result);
		resHandle.setDouble(res);
		//resHandle.setClean();


		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);

	}else{
		return MS::kUnknownParameter;
	}
*/

	return stat;
}

MStatus rbfBlend::readMatrixArray(MDataBlock &data, MDoubleArray &dArrWts, MVectorArray &vArrRead, MVector &vCur, MMatrixArray &poseMat, MMatrix &DrMat, unsigned int &uMat){

	MStatus stat;

	uMat = 0;

	MArrayDataHandle hArrWorldMatrix = data.inputArrayValue( aPoseXform, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MDataHandle hDriveWorldMatrix = data.inputValue( aDriver, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MArrayDataHandle hArrWeights = data.inputArrayValue( aWeight, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	MDataHandle hPoseXFormReadAxis = data.inputValue( aAxis );	// Get poseReader matrix for this pose/xform combo
	int nReadAxis = hPoseXFormReadAxis.asShort();

	// First pass, go thru all of the entries and see what highed logical index is..
	do
	{
		unsigned uIdx = hArrWorldMatrix.elementIndex(&stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		if (stat != MS::kSuccess)
			continue ;
		if (uIdx >= uMat)
			uMat = uIdx + 1 ;

	}while(hArrWorldMatrix.next());

	// Alloc Matrix array
	if (uMat == 0)
		return MS::kFailure ;		// nothing to alloc!


		// RESET
	hArrWorldMatrix = data.inputArrayValue( aPoseXform, &stat) ;

	MVector vec;

	if (nReadAxis == 0)
		vec = MVector(1.0, 0.0, 0.0) ;
	else if (nReadAxis == 1)
		vec = MVector(0.0, 1.0, 0.0) ;
	else
		vec = MVector(0.0, 0.0, 1.0) ;

	vCur = vec * hDriveWorldMatrix.asMatrix();

	DrMat = hDriveWorldMatrix.asMatrix();

//	DrMat.x = hDriveWorldMatrix.asMatrix()[3][0];
//	DrMat.y = hDriveWorldMatrix.asMatrix()[3][1];
//	DrMat.z = hDriveWorldMatrix.asMatrix()[3][2];

	vArrRead.clear();
	dArrWts.clear();
	vArrRead.setLength(uMat);
	dArrWts.setLength(uMat);

	poseMat.clear();
	poseMat.setLength(uMat);

	// Second pass, actually read in each matrix...
	unsigned uIdx ;
	for (uIdx=0; uIdx < uMat; ++uIdx)
		{

		stat = hArrWeights.jumpToElement(uIdx);
		if (stat != MS::kSuccess)
			continue ;

		stat = hArrWorldMatrix.jumpToElement( uIdx ) ;
		if (stat != MS::kSuccess)
			continue ;

		MDataHandle hEleWorldMatrix = hArrWorldMatrix.inputValue(&stat) ;
		if (stat != MS::kSuccess)
			continue ;

		vArrRead[uIdx] = vec * hEleWorldMatrix.asMatrix();
		dArrWts[uIdx] = hArrWeights.inputValue().asDouble();

		poseMat[uIdx] = hEleWorldMatrix.asMatrix();


	}


	return MS::kSuccess ;
}

/*
MStatus rbfBlend::interpWeights(MDoubleArray &dArrWts, const MVectorArray &vArrRead, const MVector &vCur, const double &dRBFWidth)
{
	MStatus stat ;

    MObject thisNode = thisMObject();
    MString name = MFnDependencyNode(thisNode).name();

	unsigned uPoses = vArrRead.length() ;

	if (uPoses < 2)
			return MS::kSuccess ;

	// Calc inverse width
	double dInvWidth = 1.0 ;
	if (dRBFWidth != 0)
			dInvWidth = 1.0 / dRBFWidth;

	// And then sigma squared
	double dSigma2 = - ( dInvWidth * dInvWidth );


	MatrixNN matNNA ;	// Make the A matrix for Ax = b.
	matNNA.setDimension( uPoses ) ;		// alloc enough space for it.

	unsigned i,j ;
	for (i=0; i < uPoses; ++i)
		{
		for (j=0; j < uPoses; ++j)
			{
			// Now store how far away each pose is from each other pose.
			// ie: for pose i=#  compare to the pose j=#.  The diagonal will be
			// all zeros, which is why the LU factorization requires pivoting.
			//
			// Note: We normalize distance within 0-1 for better results.
			//	Easy since at max a pose can be 180 degrees away, or pi-radians away.
			//  So we just get the angle between and divide by pi to normalize.
			//
			double dDist = 	vArrRead[i].angle( vArrRead[j]) / 3.14159 ;
			matNNA[i][j] = exp( dSigma2 * dDist * dDist) ;	// e^(sigma*(dDist^2))
			} // end of j cols

		} // end of i rows
	// Now we've got our matrix built...make our "b" array/vector of doubles.
	// as well as the X output weights
	double *ptrB = NULL ;
	double *ptrX = NULL ;
	ptrB = new double [uPoses] ;
	ptrX = new double [uPoses] ;

	// now we count up orig weights and we use this when normalizing later.
	// This lets us kinda fake using cones, even tho technically the values
	// could be 1.0 for all ofB, and or the cones could all be 180.  But this
	// way we'll smoothly ramp down if cone values fall off.  Plus by using
	// the array wt value here for B we also handle twist on cone disabling...
	double dTotalOrig = 0.0 ;
	for (i=0; i < uPoses; ++i)
		{

		ptrB[i] = dArrWts[i] ;			// We could do 1.0 here, need to test.
		dTotalOrig += dArrWts[i] ;
		}
	if (dTotalOrig > 1.0)
		dTotalOrig = 1.0 ;

	// Now we are ready to do.  This will solve Ax=b using LU-Factorization.
	bool bSolve = matNNA.solveLU(ptrB, ptrX) ;

	if (!bSolve)
		{
		// uh-oh the matrix wasn't factorable
		MGlobal::displayWarning(MString("Unable to factorize matrix for LU Decomposition. ")+name) ;
		for (i=0; i < uPoses; ++i)
			dArrWts[i] = 0.0 ;
		delete [] ptrB ;		// Dealloc before we exit!
		delete [] ptrX ;
		return MS::kFailure ;
		}

	// now that we've solved...we'll store the real weight...
	//
	double dTotal = 0.0 ;
	for (i=0; i < uPoses; ++i)
		{

		double dDist = vCur.angle( vArrRead[i]) / 3.14159 ;		// Where are we relative to this pose?
		//dArrWts[i] = ptrB[i] * exp( dSigma2 * dDist * dDist ) * ptrX[i];	// w_[i] * e^(sigma * |pose-pose[i]|)
		dArrWts[i] = dArrWts[i] * exp( dSigma2 * dDist * dDist );
//		MGlobal::displayInfo(MString("dArrWts: ")+dArrWts[i]);
//		MGlobal::displayInfo(MString("ptrX[i]: ")+ptrX[i]);
//		MGlobal::displayInfo(MString("ptrB[i]: ")+ptrB[i]);
			if (dArrWts[i] < 0.0){
				dArrWts[i] = 0.0;
			}
		dTotal += dArrWts[i] ;
		}

	// Now renormalize to a value of what we want...
	for (i=0; i < uPoses && dTotal != 0.0; ++i){
		dArrWts[i] = dArrWts[i] / dTotal * dTotalOrig ;
	}


	delete [] ptrB ;		// Dealloc before we exit!
	delete [] ptrX ;
//	MGlobal::displayInfo(MString("dInvWidth ")+dInvWidth);
//	MGlobal::displayInfo(MString("dRBFWidth ")+dRBFWidth);
//	MGlobal::displayInfo(MString("dSigma2")+dSigma2);

	return MS::kSuccess;
}
*/


/*
MStatus rbfBlend::interpWeights(MDoubleArray &dArrWts, const MVectorArray &vArrRead, const MVector &vCur, const double &dRBFWidth)
{
	MStatus stat ;

    MObject thisNode = thisMObject();
    MString name = MFnDependencyNode(thisNode).name();

	unsigned uPoses = vArrRead.length() ;

	// Calc inverse width
	double dInvWidth = 1.0 ;
	if (dRBFWidth != 0)
			dInvWidth = 1.0 / dRBFWidth;

	// And then epsilon squared
	double epsilon = -( dInvWidth * dInvWidth );


	double dTotalOrig = 0.0 ;
	for (unsigned int i=0; i < uPoses; ++i)
		{
		dTotalOrig += dArrWts[i] ;
		}
	if (dTotalOrig > 1.0)
		dTotalOrig = 1.0 ;

	// now that we've solved...we'll store the real weight...
	//
	double dTotal = 0.0 ;
	for (unsigned int i=0; i < uPoses; ++i)
		{

		double dDist = vCur.angle( vArrRead[i]) / 3.14159 ;		// Where are we relative to this pose?
		// gaussian
		dArrWts[i] = dArrWts[i] * exp( epsilon * dDist * dDist );
		//Multiquadric
		//dArrWts[i] = dArrWts[i] * (1.0f+(epsilon * (dDist * dDist)));
		//Inverse quadratic
		//dArrWts[i] = dArrWts[i] * (1.0/(1.0f+(epsilon * (dDist * dDist))));
			if (dArrWts[i] < 0.0){
				dArrWts[i] = 0.0;
			}
		dTotal += dArrWts[i] ;
		}

	// Now renormalize to a value of what we want...
	for (unsigned int i=0; i < uPoses && dTotal != 0.0; ++i){
		dArrWts[i] = dArrWts[i] / dTotal * dTotalOrig ;
	}



	return MS::kSuccess;
}

*/


MStatus rbfBlend::interpWeights(MDataBlock &data, MDoubleArray &dArrWts, const MVectorArray &vArrRead, const MVector &vCur, MMatrixArray &poseMat, MMatrix &DrMat, const double &dRBFWidth)
{
	MStatus stat ;

	MDataHandle hPoseMethod = data.inputValue( aMethod );
	int nMethod = hPoseMethod.asShort();

	MDataHandle hPoseMode = data.inputValue( aMode );
	int nMode = hPoseMode.asShort();

	MDataHandle hRBFtype = data.inputValue( aTypes );
	int nType = hRBFtype.asShort();

    MObject thisNode = thisMObject();
    MString name = MFnDependencyNode(thisNode).name();

	unsigned uPoses = vArrRead.length() ;

	// Calc inverse width
	double dInvWidth = 1.0 ;
	if (dRBFWidth != 0)
			dInvWidth = 1.0 / dRBFWidth;

	// And then epsilon squared
	double epsilon = -( dInvWidth * dInvWidth );



	double dTotalOrig = 0.0 ;

	for (unsigned int i=0; i < uPoses; ++i)
	{
		dTotalOrig += dArrWts[i] ;
	}

	if (dTotalOrig > 1.0){
		dTotalOrig = 1.0 ;
	}


	if(nMethod == 0){

		double dTotal = 0.0 ;
		MVector norCur = vCur;
		norCur.normalize();
		MVectorArray norVecR = vArrRead;

		float totalDis = 0.0;

		for(unsigned int i=0; i < uPoses; ++i){
			norVecR[i].normalize();
			float dDist = dotP(norCur,norVecR[i]);		// Where are we relative to this pose?

			if(dDist < 0){
				dDist = 0;
			}
			totalDis += dDist;
		}

		for (unsigned int i=0; i < uPoses; ++i)
		{

			norVecR[i].normalize();
			float dDist = dotP(norCur,norVecR[i]);		// Where are we relative to this pose?

			if(dDist < 0){
				dDist = 0;
			}


			if(nMode == 0){
				dArrWts[i] = dArrWts[i] * (dDist/totalDis);
			}else{
				dDist = 1.0 - dDist;
				// gaussian
				//dArrWts[i] = dArrWts[i] * exp( epsilon * (dDist * dDist) );
				//Multiquadric
				//dArrWts[i] = dArrWts[i] * (1.0f+(epsilon * (dDist * dDist)));
				//Inverse quadratic
				//dArrWts[i] = dArrWts[i] * (1.0/(1.0f+(epsilon * (dDist * dDist))));

				// types of interpolations
				if (nType == 0){
					// gaussian
					dArrWts[i] = dArrWts[i] * exp( epsilon * (dDist * dDist) );
				}else if(nType == 1){
					//Multiquadric
					dArrWts[i] = dArrWts[i] * std::sqrt(1.0f+(-epsilon/(dDist * dDist)));
				}else if(nType == 2){
					//Inverse quadratic
					dArrWts[i] = dArrWts[i] * (1.0/(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 3){
					//Inverse multiquadric
					dArrWts[i] = dArrWts[i] * (1.0/std::sqrt(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 4){
					//Thin plate spline
					dArrWts[i] = dArrWts[i] * std::pow(-epsilon, 2) * log(-epsilon/dDist);
				}
			}

			if (dArrWts[i] < 0.0){
				dArrWts[i] = 0.0;
			}
			dTotal += dArrWts[i] ;
		}

		// Now renormalize to a value of what we want...
		for (unsigned int i=0; i < uPoses && dTotal != 0.0; ++i){
			dArrWts[i] = dArrWts[i] / dTotal * dTotalOrig ;
		}

	}else{

		if(nMode == 0){

			double dTotal = 0.0 ;

			for (unsigned int u=0; u < uPoses; ++u){
				dTotal += dArrWts[u] ;
			}


			if (dTotal > 1.00)
			{
				for (unsigned int u=0; u < uPoses; ++u){
					dArrWts[u] = dArrWts[u] / dTotal ;
				}
			}

			dTotal = 0.0 ;

			for(unsigned int i=0; i < uPoses; ++i){

				float dDist =  GetDistByTwoMat(DrMat, poseMat[i]);


				// types of interpolations
				if (nType == 0){
					// gaussian
					dArrWts[i] = dArrWts[i] * exp( epsilon * (dDist * dDist) );
				}else if(nType == 1){
					//Multiquadric
					dArrWts[i] = dArrWts[i] * std::sqrt(1.0f+(-epsilon/(dDist * dDist)));
				}else if(nType == 2){
					//Inverse quadratic
					dArrWts[i] = dArrWts[i] * (1.0/(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 3){
					//Inverse multiquadric
					dArrWts[i] = dArrWts[i] * (1.0/std::sqrt(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 4){
					//Thin plate spline
					dArrWts[i] = dArrWts[i] * std::pow(-epsilon, 2) * log(-epsilon/dDist);
				}


//				phi1(r) = sqrt ( r^2 + r0^2 ) (multiquadric)
//				sqrt(1+(r/R0)^2) /Inverse quadratic
//				phi2(r) = 1 / sqrt ( r^2 + r0^2 ) (inverse multiquadric)
//				phi3(r) = r^2 * log ( r / r0 ) (thin plate spline)
//				phi4(r) = exp ( -0.5 r^2 / r0^2 ) (gaussian)


				if (dArrWts[i] < 0.0){
					dArrWts[i] = 0.0;
				}
				dTotal += dArrWts[i] ;

			}

			for (unsigned int i=0; i < uPoses && dTotal != 0.0; ++i){
				dArrWts[i] = dArrWts[i] / dTotal * dTotalOrig ;
			}

		}else if(nMode == 1){

			double dTotal = 0.0 ;
			float totalDis = 0.0;

			for(unsigned int i=0; i < uPoses; ++i){
				float dDist = GetDistByTwoMat(DrMat, poseMat[i]);
				totalDis += dDist;
			}

			for(unsigned int i=0; i < uPoses; ++i){

				float dDist =  GetDistByTwoMat(DrMat, poseMat[i]);
				dDist = dDist/totalDis;


				// types of interpolations
				if (nType == 0){
					// gaussian
					dArrWts[i] = dArrWts[i] * exp( epsilon * (dDist * dDist) );
				}else if(nType == 1){
					//Multiquadric
					dArrWts[i] = dArrWts[i] * std::sqrt(1.0f+(-epsilon/(dDist * dDist)));
				}else if(nType == 2){
					//Inverse quadratic
					dArrWts[i] = dArrWts[i] * (1.0/(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 3){
					//Inverse multiquadric
					dArrWts[i] = dArrWts[i] * (1.0/std::sqrt(1.0f+(-epsilon * (dDist * dDist))));
				}else if(nType == 4){
					//Thin plate spline
					dArrWts[i] = dArrWts[i] * std::pow(-epsilon, 2) * log(-epsilon/dDist);
				}

				// gaussian
				//dArrWts[i] = dArrWts[i] * exp( epsilon * (dDist * dDist) );
				//Multiquadric
				//dArrWts[i] = dArrWts[i] * std::sqrt(1.0f+(-epsilon/(dDist * dDist)));
				//Inverse quadratic
				//dArrWts[i] = dArrWts[i] * (1.0/1.0f+(-epsilon / (dDist * dDist)));
				//Inverse multiquadric
				//dArrWts[i] = dArrWts[i] * (1.0/std::sqrt(1.0f+(-epsilon * (dDist * dDist))));
				//Thin plate spline
				//dArrWts[i] = dArrWts[i] * std::pow(-epsilon, 2) * log(-epsilon/dDist);



				if (dArrWts[i] < 0.0){
					dArrWts[i] = 0.0;
				}
				dTotal += dArrWts[i] ;
			}

			for (unsigned int i=0; i < uPoses && dTotal != 0.0; ++i){
					dArrWts[i] = dArrWts[i] / dTotal * dTotalOrig ;
			}

		}



	}



	return MS::kSuccess;
}

