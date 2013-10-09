/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2013 The plumed team
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
#include "CLTool.h"
#include "CLToolRegister.h"
#include "tools/Tools.h"
//#include "PlumedConfig.h"
#include "core/ActionRegister.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace PLMD {
namespace cltools{

//+PLUMEDOC TOOLS gentemplate
/*
gentemplate is a tool that you can use to construct template inputs for the various
actions

The templates generated by this tool are primarily for use with Toni Giorgino's vmd gui.  It may be
useful however to use this tool as a quick aid memoir.

\par Examples

The following generates template input for the action DISTANCE.
\verbatim
plumed gentemplate --action DISTANCE
\endverbatim
 

*/
//+ENDPLUMEDOC

class GenTemplate:
public CLTool
{
public:
  static void registerKeywords( Keywords& keys );
  GenTemplate(const CLToolOptions& co );
  int main(FILE* in, FILE*out,Communicator& pc);
  string description()const{
    return "print out a template input for a particular action";
  }
};

PLUMED_REGISTER_CLTOOL(GenTemplate,"gentemplate")

void GenTemplate::registerKeywords( Keywords& keys ){
  CLTool::registerKeywords( keys );
  keys.add("optional","--action","print the template for this particular action");
  keys.addFlag("--list",false,"print a list of the available actions");
  keys.addFlag("--include-optional",false,"also print optional modifiers");
}

GenTemplate::GenTemplate(const CLToolOptions& co ):
CLTool(co)
{
  inputdata=commandline;
}

int GenTemplate::main(FILE* in, FILE*out,Communicator& pc){

 std::string action; 
 bool list_templates=false;
 parseFlag("--list",list_templates);

 if(list_templates) {
   std::cerr<<actionRegister()<<"\n"; 
   return 0;
 } else if(parse("--action",action)) {
   bool include_optional;
   parseFlag("--include-optional",include_optional);
   if( !actionRegister().printTemplate(action,include_optional) ){
       error("there is no registered action named " + action);  
       return 1; 
   }
 } else return 1;



 return 0;
}
}

} // End of namespace
