/*
 * OBJ_PointConstraint.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: r.gaziev
 */


/*
 * TestNode.cpp
 *
 *  Created on: Feb 20, 2018
 *      Author: r.gaziev
 */


//static PRM_Name sopStringName("solveMethod", "Solve Method");
//static PRM_Name sopStrChoices[] =
//{
//    PRM_Name("none", "None"),
//    PRM_Name("arap_matrix", "ARAP Matrix"),
//    PRM_Name("arap_jacobi", "ARAP Jacobi"),
//    PRM_Name(0)
//};
//static PRM_ChoiceList sopStringMenu(PRM_CHOICELIST_SINGLE, sopStrChoices);
//
//OP_ERROR
//SOP_Elastic::cookMySop(OP_Context &context)
//{
//    UT_String projectionMethod;
//    evalString(projectionMethod, "solveMethod", 0, context.getTime());
//
//    if (projectionMethod == "arap_matrix" || projectionMethod == "arap_jacobi")
//    {
//
//    }
//}

#include <SYS/SYS_Math.h>
#include <UT/UT_DSOVersion.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include "OBJ_PointConstraint.h"
#include <UT/UT_DMatrix3.h>
#include <UT/UT_VectorTypes.h>
#include <PRM/PRM_Type.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_Parm.h>
#include <cstdio>
#include <UT/UT_XformOrder.h>

#ifndef PI
#define PI 3.14159265
#endif


using namespace HDK_Sample;


// Constructor for new object class
OBJ_TestNode::OBJ_TestNode( OP_Network *net,
				const char *name, OP_Operator *op )
	  : OBJ_Geometry(net, name, op)
{
	offsetMat.identity();
    // initialize local member data here.
}



// virtual destructor for new object class
OBJ_TestNode::~OBJ_TestNode()
{
}


// here is the name of the parameter that is used by the shake object
//static PRM_Name objNames[] ={
//		PRM_Name("drv", "Driver"),
//		PRM_Name("drn", "Driven"),
//		PRM_Name("weight", "Weight")
//};

//static PRM_Name methodItems[] = {
//    PRM_Name("par",	"Parent Constraint"),
//    PRM_Name("ori",	"Orient Constraint"),
//	PRM_Name("ptn",	"Point Constraint"),
//	PRM_Name("scl",	"Scale Constraint"),
//    PRM_Name(0),
//};

//static PRM_ChoiceList methodMenu((PRM_ChoiceListType)
//				 (PRM_CHOICELIST_EXCLUSIVE |
//				  PRM_CHOICELIST_REPLACE),
//				 methodItems);

static PRM_Name objnames[] =
{
    PRM_Name("numobj",	"Number of Objects"),
    PRM_Name("objpath#","Driver Object #"),
    PRM_Name("xformpath","Driven Object"),
    PRM_Name("weight#", "Weight #"),
	PRM_Name("ofst", "Maintain Offset"),
    PRM_Name(0)

};

static PRM_Template	theObjectTemplates[] = {
    PRM_Template(PRM_FLT, 1, &objnames[3], PRMoneDefaults),
    PRM_Template(PRM_STRING,	PRM_TYPE_DYNAMIC_PATH, 1,
		 &objnames[1], 0, 0,
		 0, 0, &PRM_SpareData::objPath),
    PRM_Template()
};

PRM_Template
OBJ_TestNode::myTemplateList[] =
{	PRM_Template(PRM_TOGGLE,	1, &objnames[4], PRMoneDefaults ),
    PRM_Template(PRM_MULTITYPE_LIST, theObjectTemplates, 2, &objnames[0],
		    PRMoneDefaults),
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &objnames[2],
		0, 0, 0, 0, &PRM_SpareData::objPath),
    PRM_Template()
};


//static PRM_Template	templatelist[] = {
//
//	PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &objNames[0], 0, 0, 0, 0, &PRM_SpareData::objPath),
//	PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &objNames[1], 0, 0, 0, 0, &PRM_SpareData::objPath),
//	PRM_Template(PRM_FLT, 1, &objNames[2], PRMoneDefaults),
//	// followed by this blank terminating Template.
//	PRM_Template()
//
//};


bool
OBJ_TestNode::updateParmsFlags()
{
    bool changed = false;

    int n = NUMOBJ();
    for (int i = 1; i <= n; i++)
    {

        changed |= enableParmInst(objnames[1].getToken(), &i, (ENABLEMERGE(i) ? 1 : 0) );
    }

    return changed;
}

int
OBJ_TestNode::getDandROpsEqual()
{
    // don't do anything if we're locked
    if (flags().getHardLocked())
        return 1;

    int numobj = NUMOBJ();

    // Determine if any of our SOPs are evil.
    for (int objindex = 1; objindex <= numobj; objindex++)
    {
        if (!ENABLEMERGE(objindex)) // Ignore disabled ones.
            continue;

        UT_String objname;
        OPPATH(objname, objindex, 0.0f);

        OP_Network *objptr = (OP_Network *)findNode((const char *)objname);
        if (!objptr)
            continue;

        // Self-referential nodes are assumed to have equal DandR ops,
        // as it doesn't matter as they will be ignored and flagged
        // as errors during cook anyways.
        if (objptr == this)
        {
            continue;
        }

        if (!objptr->getDandROpsEqual())
        {
            return 0;
        }
    }
    // None of our interests was unequal, thus we are equal!
    return 1;
}

// this function copies the original parameter and then adds an invisible flag
static void
copyParmWithInvisible(PRM_Template &src, PRM_Template &dest)
{
    PRM_Name *	new_name;

    new_name = new PRM_Name(src.getToken(), src.getLabel(),
			    src.getExpressionFlag());
    new_name->harden();
    dest.initialize(
	    (PRM_Type) (src.getType() | PRM_TYPE_INVISIBLE),
	    src.getTypeExtended(),
	    src.exportLevel(),
	    src.getVectorSize(),
	    new_name,
	    src.getFactoryDefaults(),
	    src.getChoiceListPtr(),
	    src.getRangePtr(),
	    src.getCallback(),
	    src.getSparePtr(),
	    src.getParmGroup(),
	    (const char *)src.getHelpText(),
	    src.getConditionalBasePtr());
}



// this function returns the OP_TemplatePair that combines the parameters
// of this object with those of its ancestors in the (object class hierarchy)
OP_TemplatePair     *
OBJ_TestNode::buildTemplatePair(OP_TemplatePair *prevstuff)
{
    OP_TemplatePair     *pth, *geo;

    // The parm templates here are not created as a static list because
    // if that static list was built before the OBJbaseTemplate static list
    // (which it references) then that list would be corrupt. Thus we have
    // to force our static list to be created after OBJbaseTemplate.
    static PRM_Template *theTemplate = 0;

    if(!theTemplate)
    {
	PRM_Template	*obj_template;
	int		 i;
	int		 size;
	UT_String	 parm_name;

	obj_template = OBJ_Geometry::getTemplateList(OBJ_PARMS_PLAIN);
	size = PRM_Template::countTemplates( obj_template );
	theTemplate = new PRM_Template[size + 1];   // add +1 for sentinel
	for( i = 0; i < size; i++ )
	{
	    theTemplate[i] = obj_template[i];
	    theTemplate[i].getToken( parm_name );

	    // leave only the translation parameter visible (and its containing
	    // switcher)
	   // if( parm_name != "t" && parm_name != "stdswitcher" )
		copyParmWithInvisible( obj_template[i], theTemplate[i] );
	}
    }

    // Here, we have to "inherit" template pairs from geometry and beyond. To
    // do this, we first need to instantiate our template list, then add the
    // base class templates.
    pth = new OP_TemplatePair(myTemplateList, prevstuff);
    geo   = new OP_TemplatePair(theTemplate, pth);

    return geo;
}



OP_Node *
OBJ_TestNode::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new OBJ_TestNode(net, name, op);
}



OP_ERROR
OBJ_TestNode::cookMyObj(OP_Context &context)
{
    OP_ERROR	errorstatus;
    fpreal t = context.getTime();
    UT_String DrnObjName;
    UT_Matrix4D matInv;

    XFORMPATH(DrnObjName, t);

    if ( DrnObjName == "" ){

        	std::cout << "empty driven path";
        	return errorstatus;

     }else{
    	 	OBJ_Node *ObjDrnObj = findOBJNode(DrnObjName);
	 	    ObjDrnObj->getParentToWorldTransform(context, matInv );
	 	    matInv.invertDouble();


	 	    if(ENABLEOFFSET()){
 	        	OP_Node *drnObj = findNode(DrnObjName);
 	        	drnObj->getWorldTransform( offsetMat, context);
 	        }

    	 	if (ObjDrnObj != NULL){

    	 		int numobj = NUMOBJ();

    	 	    UT_Vector3D rot, scl, pos, sh;
    	 	    double x = 0, y = 0, z = 0, rx = 0, ry = 0, rz = 0;


    	 		for (int objindex = 1; objindex <= numobj; objindex++)
    	 	    {

    	 	    	if (!ENABLEMERGE(objindex))             // Ignore disabled ones.
    	 	            continue;

    	 	    	// normalize weights
    	 	    	float sum = 0;
    	 	    	for (int i = 1; i <= numobj; i++){

    	 	    		if (!ENABLEMERGE(objindex))             // Ignore disabled ones.
							continue;

    	 	    		sum += ENABLEMERGE(i);
    	 	    	}

    	 	        UT_String DrvObjName;

    	 	        OPPATH(DrvObjName, objindex, t);

    	 	        if (!DrvObjName.isstring())
    	 	        	continue;

    	 	        UT_Matrix4D matN;

    	 	        OP_Node *drvObj = findNode(DrvObjName);


    	 	        if (!drvObj)
    	 	        	continue;

    	 	        drvObj->getWorldTransform( matN, context);

    	 	       if(ENABLEOFFSET()){
    	 	    	  UT_Matrix4D matTemp;
    	 	    	  drvObj->getWorldTransform( matTemp, context);
    	 	    	  matTemp.invertDouble();
    	 	    	  offsetMat *= matTemp;
    	 	       }

    	 	        matN *= offsetMat * matInv;

    				UT_Vector3D _rot, _scl, _pos, _sh;
    				matN.explode(UT_XformOrder::rstOrder(5), _rot, _scl, _pos, _sh);

    				x += _pos.x()*(ENABLEMERGE(objindex)/sum);
    				y += _pos.y()*(ENABLEMERGE(objindex)/sum);
    				z += _pos.z()*(ENABLEMERGE(objindex)/sum);

//    				x += matN[3][0];
//    				y += matN[3][1];
//    				z += matN[3][2];

    				rx += ((_rot.x()*180)/PI)*(ENABLEMERGE(objindex)/sum);
    				ry += ((_rot.y()*180)/PI)*(ENABLEMERGE(objindex)/sum);
    				rz += ((_rot.z()*180)/PI)*(ENABLEMERGE(objindex)/sum);

    				//pos.assign( , _pos.y()*(ENABLEMERGE(objindex)/sum), _pos.z()*(ENABLEMERGE(objindex)/sum));

    	 	        //mat *= (matN *(ENABLEMERGE(objindex)/sum));
//    	 	        std::cout << "\n";
//    	 	        std::cout << ENABLEMERGE(objindex)/sum;
//    	 	        std::cout << "\n";
//    	 	        std::cout << ENABLEMERGE(objindex);
//    	 	        std::cout << "\n";

    	 	    }


    	 		//std::cout << offsetMat;

    	 	    errorstatus = OBJ_Geometry::cookMyObj(context);



//			    res = mat * matInv;

//				/// set Result matrix
//				UT_Vector3D rot, scl, pos, sh;
//				res.explode(UT_XformOrder::rstOrder(5), rot, scl, pos, sh);

				ObjDrnObj->setTX(t, x, 0);
				ObjDrnObj->setTY(t, y, 0);
				ObjDrnObj->setTZ(t, z, 0);
//
//				ObjDrnObj->setRX(t, rx, 0);
//				ObjDrnObj->setRY(t, ry, 0);
//				ObjDrnObj->setRZ(t, rz, 0);

//				ObjDrnObj->setSX(t, scl.x(), 0);
//				ObjDrnObj->setSY(t, scl.y(), 0);
//				ObjDrnObj->setSZ(t, scl.z(), 0);

    	 	 }

     	 }
//
//    XFORMPATH(DrvObjName, t);
//    XFORMPATH_(DrnObjName, t);
//
//
//    if ( DrvObjName == "" and DrnObjName == "" ){
//
//    	std::cout << "empty path";
//
//    }else{
//
//    	OP_Node *drvObj = findNode(DrvObjName);
//    	OBJ_Node *ObjDrnObj = findOBJNode(DrnObjName);
//
//    	if (ObjDrnObj != NULL){
//
//
//
//			drvObj->getWorldTransform( mat, context);
//			ObjDrnObj->getParentToWorldTransform(context, matNew );
//			matNew.invertDouble();
//
//			res = mat * matNew;
//
//			/// set Result matrix
//			UT_Vector3D rot, scl, pos, sh;
//			res.explode(UT_XformOrder::rstOrder(5), rot, scl, pos, sh);
//
//			ObjDrnObj->setTX(t, pos.x(), 0);
//			ObjDrnObj->setTY(t, pos.y(), 0);
//			ObjDrnObj->setTZ(t, pos.z(), 0);
//
//			ObjDrnObj->setRX(t, (rot.x()*180)/PI, 0);
//			ObjDrnObj->setRY(t, (rot.y()*180)/PI, 0);
//			ObjDrnObj->setRZ(t, (rot.z()*180)/PI, 0);
//
//			ObjDrnObj->setSX(t, scl.x(), 0);
//			ObjDrnObj->setSY(t, scl.y(), 0);
//			ObjDrnObj->setSZ(t, scl.z(), 0);
//
//    	}
//    	//std::cout << mat;
//
//    	//drvObj->getWorldTransform( mat, context);
//    	//drnObj->buildXform(mat.FullTransformModel(), matNew);
//    	//ObjDrvObj->getObjectToWorldTransform(context, mat);
//    	//const UT_XformOrder rst;
//    	//UT_Vector3D r, s, pos, sh;
//    	//mat.explode(rst, r, s, pos, sh);
//    	//std::cout << pos.x();
//    	//ObjDrnObj->setTX(t, .5, 0);
//    	//ObjDrnObj->setTY(t, pos.y());
//    	//ObjDrnObj->setTZ(t, pos.z());
//
//    	//std::cout << mat;
//    }
//
//    //std::cout << ObjDrnObj->getObjectType();
//
//    //mat = ObjDrvObj->getWorldXform();
//    //ObjDrvObj->getObjectToWorldTransform(context, mat);
//    //bool ch = ObjDrvObj->getWorldTransform(mat, con);
//    //ObjDrnObj->setWorldXform(mat);
//
//    //setTX
//
//    //OP_Network *xformobjptr = (OP_Network *)findNode(DrnObjName);
//
//    //drvObj->getWorldTransform( mat, context);
//    //setWorldXform( mat );
//    //ObjDrnObj->setPreTransform(mat);
//
//    //std::cout << drvObj;
//    //std::cout << ObjDrnObj;
//
//
////    UT_DMatrix4	rotation4;
////
////    // OBJ_Geometry::cookMyObj computes the local and global transform, and
////    // dirties the inverse of the global transform matrix. These are stored
////    // in myXform, myWorldXform, and myIWorldXform, respectively.
////    errorstatus = OBJ_Geometry::cookMyObj(context);
////
////    // get rid of the rotation component in the matrices
//    //getWorldXform().extractRotate(rotation);
////    if( ! rotation.invert() )
////    {
////	rotation4    = rotation;
////	setWorldXform(rotation4 * getWorldXform());
////	setLocalXform(rotation4 * getLocalXform());
////    }

    return errorstatus;
}

//int
//OBJ_TestNode::applyInputIndependentTransform(OP_Context &context, UT_DMatrix4 &mat)
//{
//	std::cout << "HEY!!!";
//
//	return 1;
//
//}


// this function installs the new object in houdini's object table.
// It is automatically called by Houdini when this dynamic library is loaded.
void
newObjectOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
		"_constraint",   // operator unique name
		"constraint",	    // operator label
		OBJ_TestNode::myConstructor,	// node instance constructor
		OBJ_TestNode::buildTemplatePair(0), // parameters
		OBJ_TestNode::theChildTableName,
		0,	// minimum inputs
		1,	// maximum inputs
		nullptr));	// variables
}


