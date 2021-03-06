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
#ifndef __PLUMED_crystallization_Gradient_h
#define __PLUMED_crystallization_Gradient_h

#include "multicolvar/VolumeGradientBase.h"

namespace PLMD {
namespace crystallisation {

class Gradient : public multicolvar::VolumeGradientBase {
friend class GradientVessel;
private:
/// The value of sigma
  double sigma;
/// Number of quantities in use in this colvar
  unsigned vend, nquantities;
/// Number of bins in each direction
  std::vector<unsigned> nbins;
/// The bead for the histogram
  HistogramBead bead;
public:
  static void registerKeywords( Keywords& keys );
  Gradient(const ActionOptions&);
/// Get the number of quantities that are calculated each time
  virtual unsigned getNumberOfQuantities();
/// This just throws an error
  unsigned getCentralAtomElementIndex(); 
/// Check on pbc - is it orthorhombic
  void setupRegions();
/// Calculate whats in the volume
  void calculateAllVolumes();
  double getValueForTolerance();
  unsigned getIndexOfWeight();
};

inline
unsigned Gradient::getNumberOfQuantities(){
  return nquantities;
} 

inline
double Gradient::getValueForTolerance(){
  return 1.0;
}

inline
unsigned Gradient::getIndexOfWeight(){
  plumed_error();
  return 1;
}

}
}
#endif
