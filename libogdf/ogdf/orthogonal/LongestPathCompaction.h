/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2008-01-17 22:56:41 +0100 (Thu, 17 Jan 2008) $ 
 ***************************************************************/
 
/** \file
 * \brief Contains constructive and improvement compaction by
 *        applying computation of longest paths in constraint graphs
 * 
 * \author Carsten Gutwenger
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


#ifndef OGDF_LONGEST_PATH_COMPACTION_H
#define OGDF_LONGEST_PATH_COMPACTION_H


#include <ogdf/orthogonal/OrthoRep.h>
#include <ogdf/planarity/PlanRepUML.h>
#include <ogdf/internal/orthogonal/RoutingChannel.h>
#include <ogdf/basic/tuples.h>
#include <ogdf/basic/GridLayoutMapped.h>


namespace ogdf {

	template<class ATYPE> class CompactionConstraintGraph;
	class OGDF_EXPORT Layout;


/**
 * \brief Compaction algorithm using longest paths in the constraint graph.
 *
 * <h3>Optional Parameters</h3>
 * 
 * <table>
 *   <tr>
 *     <th>Option</th><th>Type</th><th>Default</th><th>Description</th>
 *   </tr><tr>
 *     <td><i>tighten</i></td><td>bool</td><td>true</td>
 *     <td>if true, an additional improvement step tries to reduce the total edge length</td>
 *   </tr><tr>
 *     <td><i>max improvement steps</i></td><td>int</td><td>0</td>
 *     <td>the maximal number of steps performed by the improvement heuristic; 0 means no upper limit.</td>
 *   </tr>
 * </table>
*/
class OGDF_EXPORT LongestPathCompaction
{
public:
	//! Creates an instance of the longest path compaction algorithm.
	LongestPathCompaction(bool tighten = true,
		int maxImprovementSteps = 0);

	//! Constructive heurisitic for orthogonal representations.
	void constructiveHeuristics(
		PlanRepUML &PG,
		OrthoRep &OR,
		const RoutingChannel<int> &rc,
		GridLayoutMapped &drawing);


	//! Improvement heurisitic for orthogonal drawings.
	void improvementHeuristics(
		PlanRepUML &PG,
		OrthoRep &OR,
		const RoutingChannel<int> &rc,
		GridLayoutMapped &drawing);

	//
	// options

	//! Sets option <i>tighten</i> to \a select.
	void tighten(bool select) {
		m_tighten = select;
	}

	//! Returns the option <i>tighten</i>.
	bool tighten() const {
		return m_tighten;
	}


	//! Sets the option <i>max improvement steps</i>.
	void maxImprovementSteps(int maxSteps) {
		m_maxImprovementSteps = maxSteps;
	}

	//! Returns the option <i>max improvement steps</i>.
	int maxImprovementSteps() const {
		return m_maxImprovementSteps;
	}


private:
	void computeCoords(
		const CompactionConstraintGraph<int> &D,
		NodeArray<int> &pos);

	void applyLongestPaths(const CompactionConstraintGraph<int> &D,
		NodeArray<int> &pos);

	void moveComponents(const CompactionConstraintGraph<int> &D,
		NodeArray<int> &pos);


	// options
	bool m_tighten;  //!< Tighten pseudo-components.
	int m_maxImprovementSteps; //!< The maximal number of improvement steps.

	SList<node>    m_pseudoSources; //!< The list of pseudo-sources.
	NodeArray<int> m_component;     //!< The pseudo component of a node.
};


} // end namespace ogdf


#endif
