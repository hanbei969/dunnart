/*
 * $Revision:$
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2008-01-17 22:56:41 +0100 (Thu, 17 Jan 2008) $
 ***************************************************************/
 
/** \file
 * \brief planar biconnected augmentation algorithm with fixed
 * 		  combinatorial embedding.
 * 
 * \author Bernd Zey
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

#ifndef OGDF_PLANAR_AUGMENTATION_FIX_H
#define OGDF_PLANAR_AUGMENTATION_FIX_H

#include <ogdf/module/AugmentationModule.h>
#include <ogdf/basic/List.h>
#include <ogdf/decomposition/DynamicBCTree.h>
#include <ogdf/basic/CombinatorialEmbedding.h>
#include <ogdf/basic/FaceArray.h>
#include <ogdf/basic/GraphCopy.h>

#include <ogdf/augmentation/PlanarAugmentation.h>


namespace ogdf {



/**
 * \brief The algorithm for biconnectivity augmentation with fixed combinatorial embedding.
 * 
 */
class OGDF_EXPORT PlanarAugmentationFix : public AugmentationModule {

public:
	//! Creates an instance of planar augmentation with fixed embedding.
	PlanarAugmentationFix() { }

	~PlanarAugmentationFix() { }


protected:

	/**
	 * \brief The implementation of the algorithm call.
	 * 
	 * \param g is the working graph.
	 * \param L is the list of all new edges.
	 */
	void doCall(Graph& g, List<edge>& L);
	
private:	
	/**
	 * \brief The embedding of g.
	 */
	CombinatorialEmbedding* m_pEmbedding;
	
	/**
	 * \brief The embedding of the actual partial graph.
	 */	
	CombinatorialEmbedding* m_pActEmbedding;
	
	/**
	 * \brief The working graph.
	 */
	Graph* m_pGraph;

	/**
	 * \brief The inserted edges by the algorithm.
	 */
	List<edge>* m_pResult;
	
	/**
	 * \brief The actual dynamic bc-tree.
	 */
	DynamicBCTree* m_pBCTree;
	
	/**
	 * \brief The actual partial graph.
	 */
	GraphCopy m_graphCopy;
	
	/**
	 * \brief Edge-array required for construction of the graph copy.
	 */
	EdgeArray<edge> m_eCopy;

	/**
	 * \brief The list of all labels.
	 */	
	List<label> m_labels;
	
	/**
	 * \brief Array that contains iterators to the list of labels 
	 * 		 if a node is a parent of a label.
	 */
	NodeArray< ListIterator<label> > m_isLabel;
	
	/**
	 * \brief Array that contains the label a node belongs to.
	 */
	NodeArray<label> m_belongsTo;
	
	/**
	 * \brief Array that contains the iterator of the label a node belongs to.
	 */
	NodeArray< ListIterator<node> > m_belongsToIt;

	/**
	 * \brief The actual root of the bc-tree.
	 */
	node m_actBCRoot;

private:

	/**
	 * \brief The main function for planar augmentation.
	 */
	void augment(adjEntry adjOuterFace);
	
	/**
	 * \brief Modifies the root of the bc-tree.
	 */
	void modifyBCRoot(node oldRoot, node newRoot);
	
	/**
	 * \brief Exchanges oldRoot by newRoot and updates data structurs in the bc-tree.
	 */
	void changeBCRoot(node oldRoot, node newRoot);

	/**
	 * \brief Adds the pendant to a label or creates one (uses followPath()).
	 */
	void reduceChain(node pendant);

	/**
	 * \brief Traverses upwards in the bc-tree, starting at the pendant node.
	 */
	paStopCause followPath(node v, node& last);
	
	/**
	 * \brief Finds the next matching pendants.
	 */
	bool findMatching(node& pendant1, node& pendant2, adjEntry& v1, adjEntry& v2);
	
	/**
	 * \brief Called by findMatching, if a dominating tree was detected.
	 */
	void findMatchingRev(node& pendant1, node& pendant2, adjEntry& v1, adjEntry& v2);

	/**
	 * \brief Creates a new label.
	 */
	label newLabel(node cutvertex, node parent, node pendant, paStopCause whyStop);

	/**
	 * \brief Adds pendant \a p to label \a l.
	 */
	void addPendant(node p, label& l);

	/**
	 * \brief Inserts the label into the list of labels maintaining decreasing order.
	 */	
	ListIterator<label> insertLabel(label l);

	/**
	 * \brief Connect the two pendants.
	 */	
	void connectPendants(node pendant1, node pendant2, adjEntry adjV1, adjEntry adjV2);

	/**
	 * \brief Connects the remaining label.
	 */	
	void connectSingleLabel();

	/**
	 * \brief Deletes the pendant.
	 */	
	void deletePendant(node pendant);

	/**
	 * \brief Deletes the label.
	 */	
	void deleteLabel(label& l, bool removePendants = true);
	
	/**
	 * \brief Removes the label from the list of labels.
	 */		
	void removeLabel(label& l);
	
};	// class PlanarAugmentationFix


} // namespace ogdf

#endif
