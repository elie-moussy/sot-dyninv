/*
 * Copyright 2011, Nicolas Mansard, LAAS-CNRS
 *
 * This file is part of sot-dyninv.
 * sot-dyninv is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * sot-dyninv is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.  You should
 * have received a copy of the GNU Lesser General Public License along
 * with sot-dyninv.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __sot_dyninv_TaskDynInequality_H__
#define __sot_dyninv_TaskDynInequality_H__
/* --------------------------------------------------------------------- */
/* --- API ------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

#if defined (WIN32)
#  if defined (task_dyn_inequality_EXPORTS)
#    define SOTTASKDYNINEQUALITY_EXPORT __declspec(dllexport)
#  else
#    define SOTTASKDYNINEQUALITY_EXPORT __declspec(dllimport)
#  endif
#else
#  define SOTTASKDYNINEQUALITY_EXPORT
#endif

/* --------------------------------------------------------------------- */
/* --- INCLUDE --------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* SOT */
#include <sot-dyninv/signal-helper.h>
#include <sot-dyninv/entity-helper.h>
#include <sot/core/task.hh>
#include <sot/core/flags.hh>
#include <sot-dyninv/task-dyn-pd.h>

namespace dynamicgraph {
  namespace sot {
    namespace dyninv {

      /* --------------------------------------------------------------------- */
      /* --- CLASS ----------------------------------------------------------- */
      /* --------------------------------------------------------------------- */


      class SOTTASKDYNINEQUALITY_EXPORT TaskDynInequality
	:public TaskDynPD
	,public EntityHelper<TaskDynInequality>
	{

	public: /* --- CONSTRUCTOR ---- */

	  TaskDynInequality( const std::string& name );

	public: /* --- ENTITY INHERITANCE --- */

	  static const std::string CLASS_NAME;
	  virtual const std::string& getClassName( void ) { return CLASS_NAME; }

	public:  /* --- SIGNALS --- */

	  DECLARE_SIGNAL_IN(referenceInf,dynamicgraph::Vector);
	  DECLARE_SIGNAL_IN(referenceSup,dynamicgraph::Vector);
	  DECLARE_SIGNAL_IN(selec,Flags);

	  DECLARE_SIGNAL_OUT(normalizedPosition,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(size,int);

	public:  /* --- COMPUTATION --- */
	  dg::sot::VectorMultiBound&
	    computeTaskDyn( dg::sot::VectorMultiBound& res,int time );
	  dynamicgraph::Matrix&
	    computeJacobian( dynamicgraph::Matrix& res,int time );

	}; // class TaskDynInequality

    } // namespace dyninv
  } // namespace sot
} // namespace dynamicgraph


#endif // #ifndef __sot_dyninv_TaskDynInequality_H__
