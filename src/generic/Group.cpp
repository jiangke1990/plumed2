/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2011-2014 The plumed team
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

#include "core/ActionRegister.h"
#include "core/ActionAtomistic.h"
#include "core/Atoms.h"
#include "tools/IFile.h"
#include "tools/Tools.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

namespace PLMD{
namespace generic{

//+PLUMEDOC GENERIC GROUP
/*
Define a group of atoms so that a particular list of atoms can be referenced with a single label
in definitions of CVs or virtual atoms. 

Atoms can be listed as comma separated numbers (i.e. 1,2,3,10,45,7,9,..) , simple positive ranges
(i.e. 20-40), ranges with a stride either positive or negative (i.e. 20-40:2 or 80-50:-2) or as
combinations of all the former methods (1,2,4,5,10-20,21-40:2,80-50:-2). 

Moreover, lists can be imported from ndx files (GROMACS format). Use NDX_FILE to set the name of 
the index file and NDX_GROUP to set the name of the group to be imported (default is first one).

It is also possible to remove atoms from a list and or sort them using keywords REMOVE, SORT, and UNIQUE.
The flow is the following:
- If ATOMS is present take the ordered list of atoms from the ATOMS keyword.
- If NDX_FILE is present append the list from the the gromacs group.
- If REMOVE is present remove the first occurence of each of these atoms from the list.
  If one tries to remove an atom that was not listed plumed adds a notice in the output.
- If SORT is present resulting list is sorted.
- If UNIQUE is present the resuling list is sorted and duplicate elements are removed.

Notice that this command just creates a shortcut, and does not imply any real calculation.
It is just convenient to better organize input files. Might be used in combination with
the \ref INCLUDE command so as to store long group definitions in a separate file.


\par Examples

This command create a group of atoms containing atoms 1,4,7,11 and 14 (labeled 'o'), and another containing
atoms 2,3,5,6,8,9,12,13 (labeled 'h'):
\verbatim
o: GROUP ATOMS=1,4,7,11,14
h: GROUP ATOMS=2,3,5,6,8,9,12,13
# compute the coordination among the two groups
c: COORDINATION GROUPA=o GROUPB=h R_0=0.3
# same could have been obtained without GROUP, just writing:
# c: COORDINATION GROUPA=1,4,7,11,14 GROUPB=2,3,5,6,8,9,12,13

# print the coordination on file 'colvar'
PRINT ARG=c FILE=colvar
\endverbatim
(see also \ref COORDINATION and \ref PRINT)

Groups can be conveniently stored in a separate file.
E.g. one could create a file named 'groups.dat' which reads
\verbatim
o: GROUP ATOMS=1,4,7,11,14
h: GROUP ATOMS=2,3,5,6,8,9,12,13
\endverbatim
and then include it in the main 'plumed.dat' file
\verbatim
INCLUDE FILE=groups.dat
# compute the coordination among the two groups
c: COORDINATION GROUPA=o GROUPB=h R_0=0.3
# print the coordination on file 'colvar'
PRINT ARG=c FILE=colvar
\endverbatim
(see also \ref INCLUDE, \ref COORDINATION, and \ref PRINT).
The groups.dat file could be very long and include lists of thousand atoms without cluttering the main plumed.dat file.

A GROMACS index file can also be imported
\verbatim
# import group named 'protein' from file index.ndx
pro: GROUP NDX_FILE=index.ndx NDX_GROUP=protein
# dump all the atoms of the protein on a trajectory file
DUMPATOMS ATOMS=pro FILE=traj.gro
\endverbatim
(see also \ref DUMPATOMS)

A list can be edited with REMOVE
\verbatim
# take one atom every three
ox: GROUP ATOMS=1-90:3
# take the remaining atoms
hy: GROUP ATOMS=1-90 REMOVE=ox
DUMPATOMS ATOMS=ox FILE=ox.gro
DUMPATOMS ATOMS=hy FILE=hy.gro
\endverbatim
(see also \ref DUMPATOMS)


*/
//+ENDPLUMEDOC

class Group:
  public ActionAtomistic
{

public:
  Group(const ActionOptions&ao);
  ~Group();
  static void registerKeywords( Keywords& keys );
  void calculate(){}
  void apply(){}
};

PLUMED_REGISTER_ACTION(Group,"GROUP")

Group::Group(const ActionOptions&ao):
  Action(ao),
  ActionAtomistic(ao)
{
  vector<AtomNumber> atoms;
  parseAtomList("ATOMS",atoms);
  std::string ndxfile,ndxgroup;
  parse("NDX_FILE",ndxfile);
  parse("NDX_GROUP",ndxgroup);
  if(ndxfile.length()>0 && atoms.size()>0) error("either use explicit atom list or import from index file");
  if(ndxfile.length()==0 && ndxgroup.size()>0) error("NDX_GROUP can be only used is NDX_FILE is also used");

  if(ndxfile.length()>0){
    if(ndxgroup.size()>0) log<<"  importing group '"+ndxgroup+"'";
    else                  log<<"  importing first group";
    log<<" from index file "<<ndxfile<<"\n";
    
    IFile ifile;
    ifile.open(ndxfile);
    std::string line;
    std::string groupname;
    bool firstgroup=true;
    bool groupfound=false;
    while(ifile.getline(line)){
      std::vector<std::string> words=Tools::getWords(line);
      if(words.size()>=3 && words[0]=="[" && words[2]=="]"){
        if(groupname.length()>0) firstgroup=false;
        groupname=words[1];
        if(groupname==ndxgroup || ndxgroup.length()==0) groupfound=true;
      } else if(groupname==ndxgroup || (firstgroup && ndxgroup.length()==0)){
        for(unsigned i=0;i<words.size();i++){
          AtomNumber at; Tools::convert(words[i],at);
          atoms.push_back(at);
        }
      }
    }
    if(!groupfound) error("group has not been found in index file");
  }

  std::vector<AtomNumber> remove;
  parseAtomList("REMOVE",remove);
  if(remove.size()>0){
    std::vector<AtomNumber> notfound;
    log<<"  removing these atoms from the list:";
    for(unsigned i=0;i<remove.size();i++){
      std::vector<AtomNumber>::iterator it = find(atoms.begin(),atoms.end(),remove[i]);
      if(it!=atoms.end()){
        log<<" "<<(*it).serial();
        atoms.erase(it);
      } else notfound.push_back(remove[i]);
    }
    log<<"\n";
    if(notfound.size()>0){
      log<<"  the following atoms were not found:";
      for(unsigned i=0;i<notfound.size();i++) log<<" "<<notfound[i].serial();
      log<<"\n";
    }
  }

  bool sortme=false;
  parseFlag("SORT",sortme);
  if(sortme){
    log<<"  atoms are sorted\n";
    sort(atoms.begin(),atoms.end());
  }
  bool unique=false;
  parseFlag("UNIQUE",unique);
  if(unique){
    log<<"  sorting atoms and removing duplicates\n";
    Tools::removeDuplicates(atoms);
  }

  this->atoms.insertGroup(getLabel(),atoms);
  log.printf("  list of atoms ");
  for(unsigned i=0;i<atoms.size();i++) log<<" "<<atoms[i].serial();
  log.printf("\n");
}

void Group::registerKeywords( Keywords& keys ){
  Action::registerKeywords( keys );
  ActionAtomistic::registerKeywords( keys );
  keys.add("atoms", "ATOMS", "the numerical indexes for the set of atoms in the group");
  keys.add("atoms", "REMOVE","remove these atoms from the list");
  keys.addFlag("SORT",false,"sort the resulting list");
  keys.addFlag("UNIQUE",false,"sort atoms and remove duplicated ones");
  keys.add("optional", "NDX_FILE", "the name of index file (gromacs syntax)");
  keys.add("optional", "NDX_GROUP", "the name of the group to be imported (gromacs syntax) - first group found is used by default");
}

Group::~Group(){
  atoms.removeGroup(getLabel());
}

}
}

