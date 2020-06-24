/*
 * mainPlug.cpp
 *
 *  Created on: Oct 21, 2016
 *      Author: rash
 */


#include "PointOnMesh.h"
#include "zipperNode.h"
#include "zipperCmd.h"
#include "mirrorClusterCmd.h"
#include "rbfBlend.h"
#include "JointCreator.h"
#include "bulgeDeform.h"
#include "piercings.h"
#include "pop.h"
#include "slideDeform.h"
#include "tension.h"
#include "rampNode.h"
#include "softCluster.h"
#include "curveWave.h"
#include "wrapperNS.h"
#include "SquashStretch.h"
#include "blendPage.h"
#include "mathNode.h"
#include "softClamp.h"
#include "colideP.h"
#include "gator.h"
#include "RBS_deform.h"
#include "meshClone.h"

#include "radialBlendShape.h"


#include <maya/MFnPlugin.h>


MStatus initializePlugin(MObject obj){

	MStatus stat;

	MFnPlugin plugin( obj, PLUGIN_COMPANY, "2.0", "rash");

	stat = plugin.registerNode("radialBlendShape", radialBlendShape::id, radialBlendShape::creator, radialBlendShape::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register radialBlendShape Node failed");
		return stat;
	}

	stat = plugin.registerNode("RBSdef", RBS_deform::id, RBS_deform::creator, RBS_deform::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register RBS_deform Node failed");
		return stat;
	}

	stat = plugin.registerNode("squashStretch", squashStretch::id, squashStretch::creator, squashStretch::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register squashStretch Node failed");
		return stat;
	}

	stat = plugin.registerNode("meshClone", meshClone::id, meshClone::creator, meshClone::initialize);

	if(!stat){

		stat.perror("Register meshClone Node failed");
		return stat;
	}


	stat = plugin.registerNode("rbfBlend", rbfBlend::id, rbfBlend::creator, rbfBlend::initialize);

	if(!stat){

		stat.perror("Register rbfBlend Node failed");
		return stat;
	}

	stat = plugin.registerNode("collisionPoint", colideP::id, colideP::creator, colideP::initialize);

	if(!stat){

		stat.perror("Register math Node failed");
		return stat;
	}

	stat = plugin.registerNode("math", mathNode::id, mathNode::creator, mathNode::initialize);

	if(!stat){

		stat.perror("Register math Node failed");
		return stat;
	}

	stat = plugin.registerNode("softClamp", softClamp::id, softClamp::creator, softClamp::initialize);

	if(!stat){

		stat.perror("Register softClamp Node failed");
		return stat;
	}



	stat = plugin.registerNode("wrapper", wrapperNS::id, wrapperNS::creator, wrapperNS::initialize);

	if(!stat){

		stat.perror("Register wrapperNS Node failed");
		return stat;
	}

	stat = plugin.registerNode("blendPage", blendPage::id, blendPage::creator, blendPage::initialize);

	if(!stat){

		stat.perror("Register blendPage Node failed");
		return stat;
	}

	stat = plugin.registerNode("rampNode", rampNode::id, rampNode::creator, rampNode::initialize);

	if(!stat){

		stat.perror("Register rampNode Node failed");
		return stat;
	}

	stat = plugin.registerNode("curveWave", curveWave::id, curveWave::creator, curveWave::initialize);

	if(!stat){

		stat.perror("Register curveWave Node failed");
		return stat;
	}


	stat = plugin.registerNode("tensionNode", tension::id, tension::creator, tension::initialize);

	if(!stat){

		stat.perror("Register tensionNode Node failed");
		return stat;
	}


	stat = plugin.registerNode("softCluster", softCluster::id, softCluster::creator, softCluster::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register softCluster Node failed");
		return stat;
	}

	stat = plugin.registerNode("slideDeform", slideDeform::id, slideDeform::creator, slideDeform::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register slideDeform Node failed");
		return stat;
	}

	stat = plugin.registerNode("pop", pop::id, pop::creator, pop::initialize);

	if(!stat){

		stat.perror("Register pop Node failed");
		return stat;
	}

	stat = plugin.registerNode("piercings", piercings::id, piercings::creator, piercings::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register piercings Node failed");
		return stat;
	}

	stat = plugin.registerCommand( "createJntOnCurve", CreatorJonts::creator, CreatorJonts::newSyntax );

	if (!stat) {
		stat.perror("registerCommand");
		return stat;
	}


	stat = plugin.registerCommand("zipperCmd", zipperCmd::creator, zipperCmd::newSyntax);

	if(!stat){
		stat.perror("Register zipperCmd command failed");
		return stat;
	}

	stat = plugin.registerNode("zipper", zipperNode::id, zipperNode::creator, zipperNode::initialize);

	if(!stat){

		stat.perror("Register zipper Node failed");
		return stat;
	}

	stat = plugin.registerNode("bulgeDeform", budleDeform::id, budleDeform::creator, budleDeform::initialize, MPxNode::kDeformerNode);

	if(!stat){

		stat.perror("Register budleDeform Node failed");
		return stat;
	}

	stat = plugin.registerCommand("gator", gator::creator, gator::newSyntax);

	if(!stat){
		stat.perror("Register gator command failed");
		return stat;
	}

	stat = plugin.registerCommand("copyClusterWeights", PointOnMeshCmd::creator, PointOnMeshCmd::newSyntax);

	if(!stat){
		stat.perror("Register copyClusterWeights command failed");
		return stat;
	}

	stat = plugin.registerCommand("mirrorClusterWeights", mirrorClusterCmd::creator, mirrorClusterCmd::newSyntax);

	if(!stat){
		stat.perror("Register mirrorClusterCmd command failed");
		return stat;
	}

//	stat = plugin.registerCommand("ClusterCmd_B", ClusterCmd_B::creator, ClusterCmd_B::newSyntax);
//
//	if(!stat){
//		stat.perror("Register ClusterCmd_B command failed");
//		return stat;
//	}

	return stat;

}

MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin pluginFn(obj);
	MStatus stat;

	stat = pluginFn.deregisterCommand("gator");;

	if(!stat){

		stat.perror("deregister gator command failed");
		return stat;
	}

//	stat = pluginFn.deregisterCommand("ClusterCmd_B");;
//
//	if(!stat){
//
//		stat.perror("deregister ClusterCmd_B command failed");
//		return stat;
//	}

	stat = pluginFn.deregisterCommand("mirrorClusterWeights");;

	if(!stat){

		stat.perror("deregister mirrorClusterWeights command failed");
		return stat;
	}

	stat = pluginFn.deregisterCommand("copyClusterWeights");

	if(!stat){

		stat.perror("deregister copyClusterWeights command failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( meshClone::id);

	if(!stat){

		stat.perror("deregister meshClone node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( radialBlendShape::id);

	if(!stat){

		stat.perror("deregister radialBlendShape node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( RBS_deform::id);

	if(!stat){

		stat.perror("deregister RBS_deform node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( rbfBlend::id);

	if(!stat){

		stat.perror("deregister rbfBlend node failed");
		return stat;
	}


	stat = pluginFn.deregisterNode( zipperNode::id);

	if(!stat){

		stat.perror("deregister zipperNode node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( squashStretch::id);

	if(!stat){

		stat.perror("deregister squashStretch node failed");
		return stat;
	}


	stat = pluginFn.deregisterNode( budleDeform::id);

	if(!stat){

		stat.perror("deregister budleDeform node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( wrapperNS::id);

	if(!stat){

		stat.perror("deregister wrapperNS node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( softCluster::id);

	if(!stat){

		stat.perror("deregister softCluster node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( piercings::id);

	if(!stat){

		stat.perror("deregister piercings node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( curveWave::id);

	if(!stat){

		stat.perror("deregister curveWave node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( slideDeform::id);

	if(!stat){

		stat.perror("deregister slideDeform node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( pop::id);

	if(!stat){

		stat.perror("deregister pop node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( tension::id);

	if(!stat){

		stat.perror("deregister tension node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( rampNode::id);

	if(!stat){

		stat.perror("deregister rampNode node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( blendPage::id);

	if(!stat){

		stat.perror("deregister blendPage node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( mathNode::id);

	if(!stat){

		stat.perror("deregister mathNode node failed");
		return stat;
	}
	stat = pluginFn.deregisterNode( softClamp::id);

	if(!stat){

		stat.perror("deregister softClamp node failed");
		return stat;
	}

	stat = pluginFn.deregisterNode( colideP::id);

	if(!stat){

		stat.perror("deregister colideP node failed");
		return stat;
	}

	stat = pluginFn.deregisterCommand( "zipperCmd");

	if(!stat){

		stat.perror("deregister zipperCmd node failed");
		return stat;
	}

	stat = pluginFn.deregisterCommand( "createJntOnCurve" );

	if (!stat) {
		stat.perror("deregisterCommand");
	}

	return stat;

}

