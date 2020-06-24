#include "tension.h"



MObject tension::inMesh;
MObject tension::resMesh;
MObject tension::outMesh;
MObject tension::floatMap;
MObject tension::doubleMap;
MObject tension::env;
MObject tension::tensionColorContrast;
MObject tension::tensionColorRamp;
MObject tension::drawColorPerVertex;
MObject tension::colorDisplay;

void * tension::creator(){

	return new tension;
}

MStatus tension::initialize(){


	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;
	MFnMessageAttribute messAttr;
	MFnEnumAttribute enumAttr;

	// color display
	colorDisplay = enumAttr.create( "colorDisplay", "codis", 0 );
	enumAttr.setStorable(true);
	enumAttr.setKeyable(true);
	enumAttr.addField("RGB", 0) ;
	enumAttr.addField("GRB", 1) ;
	enumAttr.addField("BGR", 2) ;
	enumAttr.addField("Ramp", 3) ;
	stat = addAttribute(colorDisplay);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	/////// tension color ramp /////////
	tensionColorRamp = MRampAttribute::createColorRamp(MString("tensionColorRamp"),MString("tcrmp"));
	stat = addAttribute(tensionColorRamp); if(!stat)	{ cout << "ERROR in adding tensionColorRamp Attribute!\n";	}


	// drawColorPerVertex Attribute
	drawColorPerVertex = attNum.create( "drawColorPerVertex", "dcpv", MFnNumericData::kBoolean, 0, &stat );
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute (drawColorPerVertex);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// env attribute ////////
	env = attNum.create("env", "env", MFnNumericData::kDouble, 1.0, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0.0));
	CHECK_MSTATUS(attNum.setMax(1.0));
	stat = addAttribute(env);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// attribute resMesh ////
	resMesh = attrType.create("resMesh", "resMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(resMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// attribute outMesh ////
	outMesh = attrType.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	// tensionColorContrast
	tensionColorContrast = attNum.create( "tensionColorContrast", "tcctrst",	MFnNumericData::kFloat, 1.0, &stat );
	attNum.setKeyable(true);
	attNum.setSoftMin(0.0);
	attNum.setSoftMax(1.0);
	stat = addAttribute (tensionColorContrast);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute inMesh ////
	inMesh = attrType.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(inMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// outMap ////
	floatMap = attrType.create("floatMap", "floatMap", MFnData::kFloatArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(floatMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// outMap ////
	doubleMap = attrType.create("doubleMap", "doubleMap", MFnData::kDoubleArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(doubleMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	stat = attributeAffects( resMesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( inMesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( env, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( tensionColorContrast, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( drawColorPerVertex, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);;
	stat = attributeAffects( tensionColorRamp, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( colorDisplay, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = attributeAffects( inMesh, floatMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( inMesh, doubleMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( resMesh, floatMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( resMesh, doubleMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( env, floatMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( env, doubleMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	/*MString command =  "global proc AEtensionNodeTemplate( string $nodeName )\
						{\
							editorTemplate -beginScrollLayout;\
							\
									editorTemplate -beginLayout \"Color Per Vertex\" -collapse 1;\
											editorTemplate -addControl drawColorPerVertex;\
											editorTemplate -addControl colorDisplay;\
											editorTemplate -addControl tensionColorContrast;\
											editorTemplate -addControl forceColorRefresh;\
											AEaddRampControl ($nodeName+\".tensionColorRamp\");\
											editorTemplate -cl 0 ($nodeName+\".tensionColorRamp\");\
									editorTemplate -endLayout;\
										\
							editorTemplate -addExtraControls;\
							editorTemplate -endScrollLayout;\
						}\
						\
						AEtensionNodeTemplate(\"tensionNode\");";
	MGlobal::executeCommand(command);*/

	return MS::kSuccess;




}

MStatus tension::compute(const MPlug& plug, MDataBlock & data){
	MStatus stat;

	// Do this if we are using an OpenMP implementation that is not the same as Maya's.
	// Even if it is the same, it does no harm to make this call.
	MThreadUtils::syncNumOpenMPThreads();

	#ifndef _DEBUG
	#ifdef _OPENMP
	omp_set_num_threads(omp_get_num_procs());
	#endif
	#endif

	if(plug == outMesh){

		// Get colorDisplay Attr
		MDataHandle colorDisplayData = data.inputValue( colorDisplay, &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		// Get drawColorPerVertex Attr
		MDataHandle drawColorPerVertexData = data.inputValue( drawColorPerVertex, &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		// Get tensionColorContrast Attr
		MDataHandle tensionColorContrastData = data.inputValue(tensionColorContrast, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// Color Ramps
		MRampAttribute tensionColorRampData(thisMObject(), tensionColorRamp, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		/// resPose ///
		MDataHandle MresMeshHandle = data.inputValue(resMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject resM = MresMeshHandle.asMesh();
		MFnMesh fnResMesh(resM);


		/// inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();
		MFnMesh fnInMesh(moMesh);


		/// outMesh ///
		MDataHandle MoutMeshHandle = data.outputValue(outMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MoutMeshHandle.copy(inPolyDataHandle);
		MObject moOutMesh = MoutMeshHandle.asMesh();
		MFnMesh fnOutMesh(moOutMesh);

		/// env ///
		MDataHandle inEnv = data.inputValue(env, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double envDouble = inEnv.asDouble();


		float val = 0;

		MFloatPointArray pts;
		fnInMesh.getPoints(pts, MSpace::kWorld);


		MDataHandle h_FloatMap = data.outputValue(floatMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MDataHandle h_DoubleMap = data.outputValue(doubleMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		MFnDoubleArrayData fn_map;
		MDoubleArray mFloat;
		int nVrx = pts.length();

		mFloat.setLength(nVrx);


		if(inPtr == NULL){

			MItMeshVertex itVert(resM, &stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			inPtr = new MIntArray[nVrx];
			edges = fnResMesh.numEdges();

			for( int i=0; i < nVrx; i++){

				int dummy = 0;
				int numEdges;
				itVert.setIndex(i, dummy);
				stat = itVert.numConnectedEdges(numEdges);
				CHECK_MSTATUS_AND_RETURN_IT(stat);
				inPtr[i].append(numEdges);

			}

		}


		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nVrx; i++) {
			mFloat[i] = 0;
		}


		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif

		for (int i =0; i<edges; i++){

			/// res pose point
			MPoint resPointA;
			MPoint resPointB;

			MPoint meshPointA;
			MPoint meshPointB;

			int2 VetxEdges;
			fnResMesh.getEdgeVertices(i, VetxEdges);


			fnResMesh.getPoint(VetxEdges[0], resPointA, MSpace::kObject);
			fnResMesh.getPoint(VetxEdges[1], resPointB, MSpace::kObject);

			fnInMesh.getPoint(VetxEdges[0], meshPointA, MSpace::kObject);
			fnInMesh.getPoint(VetxEdges[1], meshPointB, MSpace::kObject);


			double lenRes = ((resPointA.x - resPointB.x)*(resPointA.x - resPointB.x)) + ((resPointA.y - resPointB.y)*(resPointA.y - resPointB.y)) + ((resPointA.z - resPointB.z)*(resPointA.z - resPointB.z));
			double lenMesh = ((meshPointA.x - meshPointB.x)*(meshPointA.x - meshPointB.x)) + ((meshPointA.y - meshPointB.y)*(meshPointA.y - meshPointB.y)) + ((meshPointA.z - meshPointB.z)*(meshPointA.z - meshPointB.z));

			double logLen = log(lenMesh/lenRes);

			mFloat[VetxEdges[0]] += logLen;
			mFloat[VetxEdges[1]] += logLen;


		}

		MColorArray tensionColors;
		MIntArray vertexList;
		MColor colorValue;
		float stretch;

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for private(stretch,colorValue)
		#endif
		#endif

		for(int i=0; i<nVrx; i++) {

			mFloat[i] = mFloat[i] / inPtr[i][0];

			if (mFloat[i] > 0){
				if (colorDisplayData.asShort() == 0){
					colorValue.r = 1.0 - float(mFloat[i]);
					colorValue.g = float(mFloat[i]);
					colorValue.b = 0;
					colorValue.a = 1;
					tensionColors.append(colorValue);
				}else if (colorDisplayData.asShort() == 1){
					colorValue.r = float(mFloat[i]);
					colorValue.g = 1.0 - float(mFloat[i]);
					colorValue.b = 0;
					colorValue.a = 1;
					tensionColors.append(colorValue);
				}else if (colorDisplayData.asShort() == 2){
					colorValue.r = 0;
					colorValue.g = float(mFloat[i]);
					colorValue.b = 1.0 - float(mFloat[i]);
					colorValue.a = 1;
					tensionColors.append(colorValue);
				}else if (colorDisplayData.asShort() == 3){
					stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) );
					tensionColorRampData.getColorAtPosition(stretch,colorValue);
					tensionColors.append(colorValue);
				}

			}else{
				if (colorDisplayData.asShort() == 0){
					colorValue.r = 1.0 - float(-mFloat[i]);
					colorValue.g = 0;
					colorValue.b = float(-mFloat[i]);
					colorValue.a = 1;
					tensionColors.append(colorValue);
				}else if (colorDisplayData.asShort() == 1){
					colorValue.r = 0;
					colorValue.g = 1.0 - float(-mFloat[i]);
					colorValue.b = float(-mFloat[i]);;
					colorValue.a = 1;
					tensionColors.append(colorValue);

				}else if (colorDisplayData.asShort() == 2){
					colorValue.r = float(-mFloat[i]);
					colorValue.g = 0;
					colorValue.b = 1.0 - float(-mFloat[i]);
					colorValue.a = 1;
					tensionColors.append(colorValue);

				}else if (colorDisplayData.asShort() == 3){
					stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) );
					tensionColorRampData.getColorAtPosition(stretch,colorValue);
					tensionColors.append(colorValue);
				}

			}

			vertexList.append(i);
		}

		if (drawColorPerVertexData.asBool()){
			fnOutMesh.setVertexColors( tensionColors, vertexList);
		}

		MObject o_map = fn_map.create(mFloat, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		h_FloatMap.set(o_map);
		h_DoubleMap.set(o_map);

		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);




	}else{

		return MS::kUnknownParameter;

	}


	return MS::kSuccess;
}



/*
 *
 *


					editorTemplate -endLayout;\
				editorTemplate -endLayout;\
			    \
				editorTemplate -beginLayout \"Color Per Vertex\" -collapse 1;\
					editorTemplate -addControl drawColorPerVertex;\
					editorTemplate -addControl colorDisplay;\
    				editorTemplate -as;\
					editorTemplate -addControl tensionColorContrast;\
					editorTemplate -addControl angleColorContrast;\
      				editorTemplate -as;\
					editorTemplate -addControl forceColorRefresh;\
					AEaddRampControl ($nodeName+\".tensionColorRamp\");\
					editorTemplate -cl 0 ($nodeName+\".tensionColorRamp\");\
					AEaddRampControl ($nodeName+\".angleColorRamp\");\
					editorTemplate -cl 0 ($nodeName+\".angleColorRamp\");\
				editorTemplate -endLayout;\

 global proc AEtensionNodeTemplate( string $nodeName )
{
    editorTemplate -beginScrollLayout;

			editorTemplate -beginLayout "Color Per Vertex" -collapse 1;
					editorTemplate -addControl drawColorPerVertex;
					editorTemplate -addControl colorDisplay;
					editorTemplate -addControl tensionColorContrast;
					editorTemplate -addControl forceColorRefresh;
					AEaddRampControl ($nodeName+".tensionColorRamp");
					editorTemplate -cl 0 ($nodeName+".tensionColorRamp");
				editorTemplate -endLayout;

    editorTemplate -addExtraControls;
    editorTemplate -endScrollLayout;
}

AEtensionNodeTemplate("tensionNode");


##########################################################################







MStatus tension::compute(const MPlug& plug, MDataBlock & data){
	MStatus stat;

	// Do this if we are using an OpenMP implementation that is not the same as Maya's.
	// Even if it is the same, it does no harm to make this call.
	MThreadUtils::syncNumOpenMPThreads();

	#ifndef _DEBUG
	#ifdef _OPENMP
	omp_set_num_threads(omp_get_num_procs());
	#endif
	#endif

	if(plug == outMesh){

		// Get drawColorPerVertex Attr
		MDataHandle drawColorPerVertexData = data.inputValue( drawColorPerVertex, &stat );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		// Get tensionColorContrast Attr
		MDataHandle tensionColorContrastData = data.inputValue(tensionColorContrast, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		// Color Ramps
		MRampAttribute tensionColorRampData(thisMObject(), tensionColorRamp, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		/// resPose ///
		MDataHandle MresMeshHandle = data.inputValue(resMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject resM = MresMeshHandle.asMesh();
		MFnMesh fnResMesh(resM);


		/// inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();
		MFnMesh fnInMesh(moMesh);


		/// outMesh ///
		MDataHandle MoutMeshHandle = data.outputValue(outMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MoutMeshHandle.copy(inPolyDataHandle);
		MObject moOutMesh = MoutMeshHandle.asMesh();
		MFnMesh fnOutMesh(moOutMesh);

		/// env ///
		MDataHandle inEnv = data.inputValue(env, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double envDouble = inEnv.asDouble();

		/// outVal ///
		MDataHandle outValHandle = data.outputValue(outVal, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		float val = 0;

		MFloatPointArray pts;
		fnInMesh.getPoints(pts, MSpace::kWorld);


		MDataHandle h_FloatMap = data.outputValue(floatMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MDataHandle h_DoubleMap = data.outputValue(doubleMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);


		MFnDoubleArrayData fn_map;
		MDoubleArray mFloat;
		int nVrx = pts.length();

		mFloat.setLength(nVrx);

		if(inPtr == NULL){

			MItMeshVertex itVert(resM, &stat);
			CHECK_MSTATUS_AND_RETURN_IT(stat);

			inPtr = new MIntArray[nVrx];
			edges = fnResMesh.numEdges();


			for( int i=0; i < nVrx; i++){

				int dummy = 0;
				int numEdges;
				itVert.setIndex(i, dummy);
				stat = itVert.numConnectedEdges(numEdges);
				CHECK_MSTATUS_AND_RETURN_IT(stat);
				inPtr[i].append(numEdges);

			}

		}


		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nVrx; i++) {
			mFloat[i] = 0;
		}


		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif

		for (int i =0; i<edges; i++){

			/// res pose point
			MPoint resPointA;
			MPoint resPointB;

			MPoint meshPointA;
			MPoint meshPointB;

			int2 VetxEdges;
			fnResMesh.getEdgeVertices(i, VetxEdges);


			fnResMesh.getPoint(VetxEdges[0], resPointA, MSpace::kObject);
			fnResMesh.getPoint(VetxEdges[1], resPointB, MSpace::kObject);

			fnInMesh.getPoint(VetxEdges[0], meshPointA, MSpace::kObject);
			fnInMesh.getPoint(VetxEdges[1], meshPointB, MSpace::kObject);


			double lenRes = ((resPointA.x - resPointB.x)*(resPointA.x - resPointB.x)) + ((resPointA.y - resPointB.y)*(resPointA.y - resPointB.y)) + ((resPointA.z - resPointB.z)*(resPointA.z - resPointB.z));
			double lenMesh = ((meshPointA.x - meshPointB.x)*(meshPointA.x - meshPointB.x)) + ((meshPointA.y - meshPointB.y)*(meshPointA.y - meshPointB.y)) + ((meshPointA.z - meshPointB.z)*(meshPointA.z - meshPointB.z));

			double logLen = log(lenMesh/lenRes);

			mFloat[VetxEdges[0]] += logLen;
			mFloat[VetxEdges[1]] += logLen;


		}

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nVrx; i++) {

			mFloat[i] = mFloat[i] / inPtr[i][0];
		}

		if (drawColorPerVertexData.asBool()){

			MColorArray colors;
			MColorArray tensionColors;
			MIntArray vertexList;

			MColor colorValue;

			vertexList.setLength(nVrx); // Set the length of our arrays so we can use ".set" afterward because if we don't, we can't work with it multithreaded. append(), insert(), remove() are not threadsafe
			tensionColors.setLength(nVrx);


			float stretch;
			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for private(stretch,colorValue)
			#endif
			#endif

			for(int i=0; i<nVrx; i++) {

				tensionColors.append(1,0,0,1);
				vertexList.append(i);

				//vertexList.set(i,i);
				// Tension Based
				//if (mFloat[i] > 0){	stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) ); } // stretch
				//else {						stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) ); } // squash

				//tensionColorRampData.getColorAtPosition(stretch,colorValue,&stat);
				//tensionColors.set(colorValue,i);

			}
			//colors = tensionColors;
			fnOutMesh.setVertexColors( tensionColors, vertexList);
			colorPerVertexDisplay = 1;

		}else{
			// Turn off color per vertex display
			if (colorPerVertexDisplay == 1) {
			//	MGlobal::executeCommandStringResult("for ($each in `listConnections -d 1 -s 0 " + tension::name() + "`){ if (`nodeType $each` == \"objectSet\") { for ($eachSet in `listConnections -d 1 -s 0 $each`){ setAttr ($eachSet + \".displayColors\") 0; } } }", false, false, NULL);
				colorPerVertexDisplay = 0;
			}
		}

///*

		if (drawColorPerVertexData.asBool()){

				// find attribute message
				//MFnDependencyNode FnDepNode(thisMObject());
				//MPlug MPlugMessage;

				MPlugArray MPlugList;
				//MPlugMessage = FnDepNode.findPlug("msg", true);
				MPlug MPlugMessage(thisMObject(), messager);

				if(MPlugMessage.isConnected()){
					//MGlobal::displayInfo(MString("True "));
					/// find dest object
					MPlugMessage.connectedTo(MPlugList, false,true);
					////get MObject from finded plug
					MObject MObjectShape;
					for(int i=0; i<MPlugList.length(); i++){
						MObjectShape = MPlugList[i].node();
						//MGlobal::displayInfo(MString("MPlugList ")+MPlugList[i].info());
						//MGlobal::displayInfo(MString("MObjectShape ")+MObjectShape.apiTypeStr());
						if (MObjectShape.apiType() == MFn::kMesh){
							//MGlobal::displayInfo(MString("HERE "));
							//MObjectShape = MPlugList[0].node();
							/// find outMesh attr
							MFnDependencyNode FnDepNodeShape(MObjectShape);
							MPlug MPlugOutMesh;
							MPlug MPluginMesh;
							MPlugOutMesh = FnDepNodeShape.findPlug("outMesh", true);
							//MPluginMesh = FnDepNodeShape.findPlug("inMesh", true);

							MDGContext MDGContextMesh;
							MObject MObjectAttrMesh;

							MObjectAttrMesh = MPlugOutMesh.asMObject(MDGContextMesh);
							MFnMesh FnMeshInf(MObjectAttrMesh);

							//MColorArray colors;
							MColorArray tensionColors;
							MIntArray vertexList;

							MColor colorValue;

							//vertexList.setLength(nVrx); // Set the length of our arrays so we can use ".set" afterward because if we don't, we can't work with it multithreaded. append(), insert(), remove() are not threadsafe
							//tensionColors.setLength(nVrx);


							float stretch;
							#ifndef _DEBUG
							#ifdef _OPENMP
							#pragma omp parallel for //private(stretch,colorValue)
							#endif
							#endif

							for(int i=0; i<nVrx; i++) {

								tensionColors.append(1,1,2,1);
								vertexList.append(i);

								//vertexList.set(i,i);
								// Tension Based
								//if (mFloat[i] > 0){	stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) ); } // stretch
								//else {						stretch = (0.5 + ((float(mFloat[i]) * tensionColorContrastData.asFloat())/2) ); } // squash

								//tensionColorRampData.getColorAtPosition(stretch,colorValue,&stat);
								//tensionColors.set(colorValue,i);

							}
							//colors = tensionColors;

							//fnOutMesh.setVertexColors( tensionColors, vertexList);
							FnMeshInf.setVertexColors( tensionColors, vertexList);
							MPlugOutMesh.setMObject(MObjectAttrMesh);
							//MGlobal::displayInfo(MString("len ")+vertexList.length());
							// Toggle the color display on the mesh
							//MGlobal::executeCommandStringResult("for ($each in `listConnections -d 1 -s 0 " + tension::name() + "`){ if (`nodeType $each` == \"objectSet\") { for ($eachSet in `listConnections -d 1 -s 0 $each`){ setAttr ($eachSet + \".displayColors\") 1; } } }", false, false, NULL);
							// Force display refresh
							//if (forceColorRefreshData.asBool() == 1){ MGlobal::executeCommandStringResult("dgdirty "+fStretch::name()+";", false, false, NULL); }
							colorPerVertexDisplay = 1;
						}else{
							MGlobal::displayError("### message attribute was not connected to the mesh! ###");
							return MS::kSuccess;
						}
					}
				}else{
					MGlobal::displayError("### message attribute was not connected! ###");
					return MS::kSuccess;
				}
			}else{
				// Turn off color per vertex display
				if (colorPerVertexDisplay == 1) {
				//	MGlobal::executeCommandStringResult("for ($each in `listConnections -d 1 -s 0 " + tension::name() + "`){ if (`nodeType $each` == \"objectSet\") { for ($eachSet in `listConnections -d 1 -s 0 $each`){ setAttr ($eachSet + \".displayColors\") 0; } } }", false, false, NULL);
					colorPerVertexDisplay = 0;
				}
			}

		//*/


	//	MObject o_map = fn_map.create(mFloat, &stat);
	//	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//	h_FloatMap.set(o_map);
	//	h_DoubleMap.set(o_map);

		//MoutMeshHandle.set(moMesh);
		//MGlobal::displayInfo(MString("Val ")+val);
		//outValHandle.set(val);

		//fnOutMesh.setPoints(pts, MSpace::kWorld);
	//	stat = data.setClean ( plug );
	//	CHECK_MSTATUS_AND_RETURN_IT(stat);




	//}else{

	//	return MS::kUnknownParameter;

//	}


///	return MS::kSuccess;
///}













/*
##########################################################################

#include "tension.h"

MObject tension::inMesh;
MObject tension::resMesh;
MObject tension::outMap;
MObject tension::outMesh;
MObject tension::w;

void * tension::creator(){

	return new tension;
}

MStatus tension::initialize(){


	MStatus stat;

	MFnTypedAttribute attrType;
	MFnNumericAttribute attNum;

	////// aPosition attribute ////////
	w = attNum.create("w", "w", MFnNumericData::kDouble, 0.5, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attNum.setKeyable(true));
	CHECK_MSTATUS(attNum.setMin(0));
	stat = addAttribute(w);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// attribute outMesh ////
	outMesh = attrType.create("outMesh", "outMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	//// attribute resMesh ////
	resMesh = attrType.create("resMesh", "resMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(resMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// attribute inMesh ////
	inMesh = attrType.create("inMesh", "inMesh", MFnData::kMesh, &stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	CHECK_MSTATUS(attrType.setStorable(true));
	stat = addAttribute(inMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	//// outMap ////
	outMap = attrType.create("oMap", "oMap", MFnData::kDoubleArray,&stat);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = addAttribute(outMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	stat = attributeAffects( resMesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( inMesh, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( w, outMesh);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( inMesh, outMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( resMesh, outMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = attributeAffects( w, outMap);
	CHECK_MSTATUS_AND_RETURN_IT(stat);


	////// Make deformer weights paintable ////////
	//MGlobal::executeCommand("addAttr -ln \"tensionAttr\" -dataType \"floatArray\";\nmakePaintable \"mesh\" \"tensionAttr\"; ");

	return MS::kSuccess;


}

MStatus tension::compute(const MPlug& plug, MDataBlock & data){
	MStatus stat;


	if(plug == outMesh){


		/// resPose ///
		MDataHandle MresMeshHandle = data.inputValue(resMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject resM = MresMeshHandle.asMesh();
		MFnMesh fnResMesh(resM);


		/// inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();
		MFnMesh fnInMesh(moMesh);

		/// w ///
		MDataHandle inW = data.inputValue(w, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		double inFloat = inW.asDouble();

		/// outMesh ///
		MDataHandle MoutMeshHandle = data.outputValue(outMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject outM = MoutMeshHandle.asMesh();
		MFnMesh fnOutMesh(outM);



		MFloatPointArray pts;
		fnInMesh.getPoints(pts, MSpace::kWorld);

		unsigned int len = pts.length();
		int edges = fnResMesh.numEdges();

		MDataHandle h_map = data.outputValue(outMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MFnDoubleArrayData fn_map;
		MDoubleArray mFloat;

		mFloat.setLength(len);

		for (int i =0; i<edges; i++){

			int2 resVtx;
			fnResMesh.getEdgeVertices(i,resVtx);
			/// res pose point
			MPoint resPointA;
			MPoint resPointB;

			MPoint meshPointA;
			MPoint meshPointB;

			fnResMesh.getPoint(resVtx[0], resPointA, MSpace::kObject);
			fnResMesh.getPoint(resVtx[1], resPointB, MSpace::kObject);

			fnInMesh.getPoint(resVtx[0], meshPointA, MSpace::kObject);
			fnInMesh.getPoint(resVtx[1], meshPointB, MSpace::kObject);

			//MVector disRes = resPointA - resPointB;
			//MVector disMesh = meshPointA - meshPointB;

			double lenRes = ((resPointA.x - resPointB.x)*(resPointA.x - resPointB.x)) + ((resPointA.y - resPointB.y)*(resPointA.y - resPointB.y)) + ((resPointA.z - resPointB.z)*(resPointA.z - resPointB.z));
			double lenMesh = ((meshPointA.x - meshPointB.x)*(meshPointA.x - meshPointB.x)) + ((meshPointA.y - meshPointB.y)*(meshPointA.y - meshPointB.y)) + ((meshPointA.z - meshPointB.z)*(meshPointA.z - meshPointB.z));


			//double logLen = lenMesh/lenRes - 1;

			double logLen = log(lenMesh/lenRes);

			//MGlobal::displayInfo(MString("edges ")+i);
			//MGlobal::displayInfo(MString("lenRes ")+lenRes);
			//MGlobal::displayInfo(MString("lenMesh ")+lenMesh);
			//MGlobal::displayInfo(MString("lenMesh/lenRes ")+lenMesh/lenRes);
			//MGlobal::displayInfo(MString("logLen ")+logLen);
			//MGlobal::displayInfo(MString("################## "));
			if(isnan(logLen)){
				MGlobal::displayInfo(MString("NaN############")+logLen);
				//logLen = 0;
			}
			mFloat[resVtx[0]] += logLen;
			mFloat[resVtx[1]] += logLen;

			//MGlobal::displayInfo(MString("edges ")+i);
			//MGlobal::displayInfo(MString("resVtx[0] ")+resVtx[0]);
			//MGlobal::displayInfo(MString("resVtx[1] ")+resVtx[1]);
			//MGlobal::displayInfo(MString("logLen ")+logLen);
			//MGlobal::displayInfo(MString("mFloat[resVtx[0]] ")+mFloat[resVtx[0]]);
			//MGlobal::displayInfo(MString("mFloat[resVtx[1]] ")+mFloat[resVtx[1]]);
		}

		for(MItMeshVertex itVert(resM, &stat); !itVert.isDone();itVert.next()){
			CHECK_MSTATUS_AND_RETURN_IT(stat);
			int numEdges;
			stat = itVert.numConnectedEdges(numEdges);
			CHECK_MSTATUS_AND_RETURN_IT(stat);
			//MGlobal::displayInfo(MString("numEdges ")+numEdges);
			double val = mFloat[itVert.index()] / double(numEdges);
			//MGlobal::displayInfo(MString("id ")+itVert.index());
			MGlobal::displayInfo(MString("val ")+val);
			//mFloat[itVert.index()];
		}
		//MGlobal::displayInfo(MString("mFloat.length() ")+mFloat.length());

		MObject o_map = fn_map.create(mFloat, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		fnOutMesh.setPoints(pts, MSpace::kWorld);

		MoutMeshHandle.set(moMesh);
		h_map.set(o_map);

		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);


	}else{

		MGlobal::displayInfo("#### FAILD tension!!!");
		return MS::kUnknownParameter;

	}

	/*
	if(plug == interval){

		MGlobal::displayInfo("#### tension!!!");

		//// position ////
		float nPosition = data.inputValue(add).asFloat();

		/// inMesh ///
		MDataHandle inPolyDataHandle = data.inputValue(inMesh, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		MObject moMesh = inPolyDataHandle.asMesh();

		MFnMesh fnMesh(moMesh);

		MFloatPointArray pts;
		fnMesh.getPoints(pts, MSpace::kWorld);

		unsigned int len = pts.length();

		MGlobal::displayInfo(MString("lens ")+len);

		MDataHandle h_map = data.outputValue(outMap, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		MFnFloatArrayData fn_map;

		MFloatArray mFloat;
		//mFloat.setLength(len);

		for (int i=0; i<=len; i++){

			mFloat.append(0.5);
		}
		MGlobal::displayInfo(MString("#### mFloat[0]!!!")+mFloat[int(nPosition)]);
		MObject o_map = fn_map.create(mFloat, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);

		h_map.set(o_map);
		stat = data.setClean ( plug );
		CHECK_MSTATUS_AND_RETURN_IT(stat);



	}else{
		MGlobal::displayInfo("#### FAILD tension!!!");
		return MS::kUnknownParameter;
	}
*/

//	return MS::kSuccess;
//}

//*/
