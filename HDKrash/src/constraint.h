/*
 * TestNode.h
 *
 *  Created on: Feb 20, 2018
 *      Author: r.gaziev
 */

#ifndef SRC_CONSTRAINT_H_
#define SRC_CONSTRAINT_H_



#include <OBJ/OBJ_Geometry.h>




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

    double Qdot(UT_QuaternionD q1, UT_QuaternionD q2);

    bool AreQuaternionsClose(UT_QuaternionD q1, UT_QuaternionD q2);

    // return numbers of elements
    int 	NUMOBJ() { return evalInt("numobj", 0, 0.0f); }
    // set numbers of elements (not necessery)
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

    void        GETMENU(UT_String &str, fpreal t)
                    {
                        evalString(str, "method", 0, t);
                    }

protected:

    // Performs the calculation of the local and the world transformation.
    virtual OP_ERROR cookMyObj(OP_Context &context);


private:

    UT_Array<UT_Matrix4D> offsetMat;

    bool first;
    int chekSize;
};

}	// End HDK_Sample namespace



#endif /* SRC_CONSTRAINT_H_ */
