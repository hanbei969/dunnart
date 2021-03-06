/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2008-01-17 22:56:41 +0100 (Thu, 17 Jan 2008) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of class SimpleIncNodeInserter.
 *
 * This class represents a strategy for the incremental drawing
 * approach to insert nodes (having no layout fixation) into the
 * fixed part of a PlanRep. The simple strategy searches for the
 * face with the maximum number of neighbours without counting
 * the cost of introduced crossings.
 * 
 * \author Karsten Klein
 * 
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 * Copyright (C) 2005-2007
 * 
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 * 
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * \par
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/


#ifdef _MSC_VER
#pragma once
#endif


#ifndef OGDF_SIMPLE_INC_NODE_INSERTER_H
#define OGDF_SIMPLE_INC_NODE_INSERTER_H


#include <ogdf/planarity/PlanRepInc.h>
#include <ogdf/basic/UMLGraph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphObserver.h>
#include <ogdf/basic/IncNodeInserter.h>

namespace ogdf {


//===============================================
//main function(s):
//

//===============================================


class OGDF_EXPORT SimpleIncNodeInserter : public IncNodeInserter
{
public:
	//creates inserter on PG
	SimpleIncNodeInserter(PlanRepInc &PG);
	virtual ~SimpleIncNodeInserter();

	//insert copy in m_planRep for original node v
	void insertCopyNode(node v, CombinatorialEmbedding &E,
		Graph::NodeType vTyp);

	//insert copy without respecting embedding
	void insertCopyNode(node v, Graph::NodeType vTyp);

protected:
	//insertAfterAdj will be filled with adjEntries for the 
	//(new) edges around the copy of v to be inserted after.
	//sorted in the order of the edge around v
	face getInsertionFace(node v, CombinatorialEmbedding &E);

	//constructs a dual graph on the copy PlanRep,
	//vCopy is the node to be inserted
	void constructDual(const Graph &G, const CombinatorialEmbedding &E, 
		bool forbidCrossings = true);

	void insertFaceEdges(node v, node vCopy, face f, CombinatorialEmbedding &E,
		adjEntry &adExternal);
	void insertCrossingEdges(node v, node vCopy, CombinatorialEmbedding &E, adjEntry &adExternal);
	void findShortestPath(const CombinatorialEmbedding &E, node s,
		node t, Graph::EdgeType eType, SList<adjEntry> &crossed);
	void insertEdge(CombinatorialEmbedding &E, edge eOrig,
		const SList<adjEntry> &crossed, bool forbidCrossingGens);

private:
	//dual graph for the edge insertion
	Graph m_dual;
	FaceArray<node> m_nodeOf;      // node in dual corresponding to
	                               // to face in primal
	NodeArray<bool> m_insertFaceNode; //node lies at border of insertionface
	NodeArray<bool> m_vAdjNodes; //node is adjacent to insertion node
	NodeArray< List<edge>* > m_incidentEdges; //original edges(insertionnode) 
	                                          //incident to original(node)
	EdgeArray<adjEntry> m_primalAdj; //copy adj for edges in dual graph
	EdgeArray<bool>     m_primalIsGen; // true iff corresponding primal edge
									   // is a generalization
	bool m_forbidCrossings; //should generalization crossings be avoided
	node m_vS; //source and sink in the dual graph for edge insertion
	node m_vT; 

}; //simpleincnodeinserter

} //end namespace ogdf

#endif
