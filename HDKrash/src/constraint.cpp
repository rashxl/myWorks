/*
 * TestNode.cpp
 *
 *  Created on: Feb 20, 2018
 *      Author: r.gaziev
 */




#include "constraint.h"

#include <SYS/SYS_Math.h>
#include <UT/UT_DSOVersion.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include <UT/UT_DMatrix3.h>
#include <UT/UT_VectorTypes.h>
#include <PRM/PRM_Type.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_Parm.h>
#include <cstdio>
#include <UT/UT_XformOrder.h>
#include <UT/UT_Quaternion.h>

#include <malloc.h>

#ifndef PI
#define PI 3.14159265
#endif


using namespace HDK_Sample;


// Constructor for new object class
OBJ_TestNode::OBJ_TestNode( OP_Network *net,
				const char *name, OP_Operator *op )
	  : OBJ_Geometry(net, name, op)
{
	first = true;
	chekSize = 0;

}

double OBJ_TestNode::Qdot(UT_QuaternionD q1, UT_QuaternionD q2){
	double d = (q1.x() * q2.x() + q1.y() * q2.y() + q1.z() * q2.z() + q1.w() * q2.w());
	return d;
}

bool OBJ_TestNode::AreQuaternionsClose(UT_QuaternionD q1, UT_QuaternionD q2){

	double dot = Qdot(q1,q2);

	if(dot < .0){
		return false;
	}else{
		return true;
	}
}

// virtual destructor for new object class
OBJ_TestNode::~OBJ_TestNode()
{
}




static PRM_Name methodItems[] = {
    PRM_Name("bth",	"Both"),
    PRM_Name("rot",	"Rotate"),
	PRM_Name("trn",	"Translate"),
    PRM_Name(0),
};

static PRM_ChoiceList methodMenu((PRM_ChoiceListType)
				 (PRM_CHOICELIST_EXCLUSIVE |
				  PRM_CHOICELIST_REPLACE),
				 methodItems);

static PRM_Name objnames[] =
{
    PRM_Name("numobj",	"Number of Objects"),
    PRM_Name("objpath#","Driver Object #"),
    PRM_Name("xformpath","Driven Object"),
    PRM_Name("weight#", "Weight #"),
	PRM_Name("ofst", "Maintain Offset"),
	PRM_Name("method", "Method"),
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
{	PRM_Template(PRM_TOGGLE,	1, &objnames[4], 0 ),
    PRM_Template(PRM_MULTITYPE_LIST, theObjectTemplates, 2, &objnames[0],
		    PRMoneDefaults),
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &objnames[2],
		0, 0, 0, 0, &PRM_SpareData::objPath),
	PRM_Template(PRM_ORD, 1, &objnames[5], 0, &methodMenu),
    PRM_Template()
};



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


// GLOBAL func compute
OP_ERROR
OBJ_TestNode::cookMyObj(OP_Context &context)
{
    OP_ERROR	errorstatus;
    fpreal t = context.getTime();
    UT_String DrnObjName, MenuCheck;
    UT_Matrix4D ParMatInv, mat, _mat;
    UT_Matrix3D _mat_;
    _mat_.identity();
    mat.identity();

    XFORMPATH(DrnObjName, t);

    if ( DrnObjName == "" ){

        	std::cout << "OBJ CONSTRAINT::WARNING:: Empty Driven Path";
        	return errorstatus;

     }else{

    	 	OBJ_Node *ObjDrnObj = findOBJNode(DrnObjName);
	 	    ObjDrnObj->getParentToWorldTransform(context, ParMatInv );
	 	    ParMatInv.invertDouble();


    	 	if (ObjDrnObj != NULL){

    	 		int numobj = NUMOBJ();

    	 	    UT_Vector3D rot, scl, pos, sh;

    	 	    double tx = 0, ty = 0, tz = 0, rx = 0, ry = 0, rz = 0;


    	 	    // fill list offset mat
    	 	    if (first or chekSize != numobj){

    	 	    	// clear memory before filling
    	 	    	offsetMat.clear();

    	 	    	//filling
    				for (int i=0; i<numobj; i++){
    					offsetMat.append(mat);
    				}

    				// checking variables
    				first = false;
    				chekSize = numobj;



    	 	    }

    	 	    UT_QuaternionD qori;
    	 	    UT_Array<UT_QuaternionD> listQuat;
    	 	    UT_Array<UT_Vector3D> listPose;
    	 	    UT_Array<double> listWeight;

    	 	    listQuat.clear();
    	 	    listWeight.clear();


				ObjDrnObj->getWorldTransform(_mat, context);
				_mat.extractRotate(_mat_);

				// var sum weight
				float sum = 0;

				for (int i = 1; i <= numobj; i++){

					if (!ENABLEMERGE(i))             // Ignore disabled ones.
						continue;

					sum += ENABLEMERGE(i);
				}

				if (sum == 0){

					UT_QuaternionD qA;
					qA.updateFromRotationMatrix(_mat_);
					qori = qA;

				}else{

					// first cycle for remember transform and quaternion
					for (int objindex = 1; objindex <= numobj; objindex++){

						UT_Matrix4D matN;
						UT_Matrix3D mt;
						UT_QuaternionD qA;
						UT_Vector3D _pos;

						if (!ENABLEMERGE(objindex))             // Ignore disabled ones.
							continue;

						UT_String DrvObjName;

						OPPATH(DrvObjName, objindex, t);

						if (!DrvObjName.isstring())
							continue;


						OP_Node *drvObj = findNode(DrvObjName);

						if (!drvObj)
							continue;

						drvObj->getWorldTransform( matN, context);

						// compute offset list
						if(ENABLEOFFSET()){

							UT_Matrix4D matTempDrv, matTempDrn;

							OP_Node *drnObj = findNode(DrnObjName);
							drnObj->getWorldTransform( matTempDrn, context);

							drvObj->getWorldTransform( matTempDrv, context);
							matTempDrv.invertDouble();
							offsetMat[objindex-1] = matTempDrn * matTempDrv;

						}

						matN = offsetMat[objindex-1] * matN * ParMatInv;


						matN.getTranslates(_pos);
						matN.extractRotate(mt);

						qA.updateFromRotationMatrix(mt);

						if (numobj == 1){
							qori += qA * (ENABLEMERGE(objindex)/sum);
						}else{
							listQuat.append(qA);
							listWeight.append(ENABLEMERGE(objindex)/sum);
						}


						tx += _pos.x()*(ENABLEMERGE(objindex)/sum);
						ty += _pos.y()*(ENABLEMERGE(objindex)/sum);
						tz += _pos.z()*(ENABLEMERGE(objindex)/sum);


					}

					if (numobj > 1){
						for (int objindex = 1; objindex <= numobj-1; objindex++){


							UT_QuaternionD qA = listQuat[objindex-1];
							UT_QuaternionD qB = listQuat[objindex];

							if(!AreQuaternionsClose(qA, qB)){
								qA = -qA;
							}

							qori += ( (qB*listWeight[objindex]) + (qA*listWeight[objindex-1]) );
						}
					}

				}


				UT_Matrix3D mt;
				UT_Vector3D VecR;

				qori.normalize();
				qori.getRotationMatrix(mt);
				mt.crack (VecR, UT_XformOrder::rstOrder(5));

				// check menu
				GETMENU(MenuCheck, t);

				if (MenuCheck == "rot" || MenuCheck == "bth"){

					ObjDrnObj->setRX(t, (VecR.x()*180)/PI, 0);
					ObjDrnObj->setRY(t, (VecR.y()*180)/PI, 0);
					ObjDrnObj->setRZ(t, (VecR.z()*180)/PI, 0);

				}

				if (MenuCheck == "trn" || MenuCheck == "bth"){

					ObjDrnObj->setTX(t, tx, 0);
					ObjDrnObj->setTY(t, ty, 0);
					ObjDrnObj->setTZ(t, tz, 0);

				}


		 }

	 }


    errorstatus = OBJ_Geometry::cookMyObj(context);

    return errorstatus;
}




// this function installs the new object in houdini's object table.
// It is automatically called by Houdini when this dynamic library is loaded.
void
newObjectOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
		"OBJ_Constraint",   // operator unique name
		"obj_constraint",	    // operator label
		OBJ_TestNode::myConstructor,	// node instance constructor
		OBJ_TestNode::buildTemplatePair(0), // parameters
		OBJ_TestNode::theChildTableName,
		0,	// minimum inputs
		1,	// maximum inputs
		nullptr));	// variables
}




