/*
 * OBJ_PointConstraint.h
 *
 *  Created on: Feb 28, 2018
 *      Author: r.gaziev
 */

#ifndef SRC_OBJ_POINTCONSTRAINT_H_
#define SRC_OBJ_POINTCONSTRAINT_H_

/*
 * TestNode.h
 *
 *  Created on: Feb 20, 2018
 *      Author: r.gaziev
 */


#include <OBJ/OBJ_Geometry.h>
#include <CHOP/CHOP_Realtime.h>


//#define	 ARG_BLEND_DIFFERENCE	(CHOP_Realtime::myParmBase +0)


namespace HDK_Sample {

class OBJ_TestNode : public OBJ_Geometry
{

public:

    // Standard constructor and destructor.
	OBJ_TestNode(OP_Network *net,
					   const char *name,
					   OP_Operator *op);
    virtual			~OBJ_TestNode();

    // Instantiates a new node of the type corresponding to this operator.
    static OP_Node * myConstructor(OP_Network *net, const char *name, OP_Operator *entry);

    virtual bool		 updateParmsFlags();

    static PRM_Template		 myTemplateList[];
    static PRM_Template		 myObsoleteList[];

    // Because the object merge can reference an op chain which has a
    // subnet with different D & R we must follow all our node's
    // d & r status
    virtual int			 getDandROpsEqual();
    virtual int			 updateDandROpsEqual(int = 1)
				 { return getDandROpsEqual(); }

    // Constructs a list of the parameters for this operator.
    static OP_TemplatePair	* buildTemplatePair(OP_TemplatePair *prevstuff);

//    int		GETDIFFERENCE()
//		{ return evalInt(ARG_BLEND_DIFFERENCE,0,0); }

    int 	NUMOBJ() { return evalInt("numobj", 0, 0.0f); }

    void	setNUMOBJ(int num_obj)
		    { setInt("numobj", 0, 0.0f, num_obj); }

    double		ENABLEMERGE(int i)
		{
		    return evalFloatInst("weight#", &i, 0, 0.0f);
		}
    void	setENABLEMERGE(int i, double val)
		{
		    setFloatInst(val, "weight#", &i, 0, 0.0f);
		}

    void	OPPATH(UT_String &str, int i, fpreal t)
		{
		    evalStringInst("objpath#", &i, str, 0, t);
		}
    void	setOPPATH( UT_String &str, CH_StringMeaning meaning,
			    int i, fpreal t)
		{
		    setStringInst(str, meaning, "objpath#", &i, 0, t);
		}

    void	XFORMPATH(UT_String &str, fpreal t)
		{
		    evalString(str, "xformpath", 0, t);
		}

    int		ENABLEOFFSET()
		{
		    return evalInt("ofst", 0, 0.0f);
		}

//    void        XFORMPATH(UT_String &str, fpreal t)
//                    {
//                        evalString(str, "drv", 0, t);
//                    }
//    void        XFORMPATH_(UT_String &str, fpreal t)
//                    {
//                        evalString(str, "drn", 0, t);
//                    }
protected:

    // Performs the calculation of the local and the world transformation.
    virtual OP_ERROR cookMyObj(OP_Context &context);

//    virtual int			 applyInputIndependentTransform(
//				    OP_Context &context, UT_DMatrix4 &mat);
private:

    UT_Matrix4D offsetMat;
};

}	// End HDK_Sample namespace






#endif /* SRC_OBJ_POINTCONSTRAINT_H_ */
