/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012-2014 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#ifndef __PLUMED_multicolvar_MultiColvarFilter_h
#define __PLUMED_multicolvar_MultiColvarFilter_h

#include "tools/HistogramBead.h"
#include "BridgedMultiColvarFunction.h"

namespace PLMD {
namespace multicolvar {

/**
\ingroup INHERIT
This is the abstract base class to use for implementing a new way of filtering collective variable values 
to see whether or not they are within a certain range
*/

class MultiColvarFilter : public BridgedMultiColvarFunction {
public:
  static void registerKeywords( Keywords& keys );
  MultiColvarFilter(const ActionOptions&);
/// Do everything required to setup the derivatives
  void doJobsRequiredBeforeTaskList();
/// Get the number of quantities in the colvar
  unsigned getNumberOfQuantities();
/// Actually do what we are asked
  void completeTask();
/// Do the filtering
  virtual double applyFilter( const double& val, double& df )=0;
/// Just checks there are no bridging forces
  void addBridgeForces( const std::vector<double>& bb );
};

inline
unsigned MultiColvarFilter::getNumberOfQuantities(){
  return getPntrToMultiColvar()->getNumberOfQuantities();
}

}
}
#endif
