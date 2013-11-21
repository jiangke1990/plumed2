/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.0.

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
#ifndef __PLUMED_vesselbase_GridVesselBase_h
#define __PLUMED_vesselbase_GridVesselBase_h

#include <string>
#include <cstring>
#include <vector>
#include "Vessel.h"

namespace PLMD {
namespace vesselbase {

class GridVesselBase : public Vessel {
private:
/// The number of points in the grid
 unsigned npoints;
/// The names of the various columns in the grid file
 std::vector<std::string> arg_names;
/// The minimum and maximum in the grid stored as strings 
 std::vector<std::string> str_min, str_max;
/// The minimum and maximum of the grid stored as doubles
 std::vector<double> min, max;
/// The spacing between grid points
 std::vector<double> dx;
/// The number of bins in each grid direction
 std::vector<unsigned> nbin;
/// Is this direction periodic
 std::vector<bool> pbc;
/// A tempory array that can be used to store indices for a point
 std::vector<unsigned> tmp_indices;
///  Flatten the grid and get the grid index for a point
 unsigned getIndex( const std::vector<unsigned>& indices ) const ;
protected:
/// The number of pieces of information we are storing for each 
/// point in the grid
 unsigned nper;
/// The dimensionality of the grid
 unsigned dimension;
/// The grid point that was requested last by getGridPointCoordinates
 unsigned currentGridPoint;
/// Get a description of the grid to output to the log
 std::string getGridDescription() const ;
/// Get the indices fof a point
 void getIndices( const unsigned& index, std::vector<unsigned>& indices ) const ;
/// Get the indices at which a particular point resides
 void getIndices(const std::vector<double>& x, std::vector<unsigned>& indices) const ; 

/// Operations on one of the elements of grid point i
 double getGridElement( const unsigned&, const unsigned& ) const ; 
 void setGridElement( const unsigned&, const unsigned&, const double& );
 void addToGridElement( const unsigned&, const unsigned&, const double& );

/// Operations on one of the elements of grid point specified by vector
 double getGridElement( const std::vector<unsigned>&, const unsigned& ) const ;
 void setGridElement( const std::vector<unsigned>&, const unsigned&, const double& );
 void addToGridElement( const std::vector<unsigned>&, const unsigned&, const double& );
/// Finish setup of grid object when it is storing the values of a function at various argument points
  void finishSetup( const std::vector<Value*>& arguments, const std::string& funcname, const bool usederiv=true );
/// Finish setup of grid object when it is storing the values of a more abstract quantity
  void finishSetup( const unsigned& nelem, const std::vector<std::string>& names );
public:
/// keywords
  static void registerKeywords( Keywords& keys );
/// Constructor
  GridVesselBase( const VesselOptions& );
/// Set the size of the buffer equal to nper*npoints
  virtual void resize();
/// Get the number of points in the grid
  unsigned getNumberOfPoints() const;
/// Get the coordinates for a point in the grid
  void getGridPointCoordinates( const unsigned& , std::vector<double>& );
/// Write the grid on a file
  void writeToFile( OFile& , const std::string& fmt );
};

inline
unsigned GridVesselBase::getNumberOfPoints() const {
  return npoints;
}

}
}
#endif