/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <CGRA/Module.h>
#include <CGRA/ModuleComposites.h>
#include <CGRA/CGRA.h>
#include "UserModules.h"

//This is the function that will ensure that the correct instance of the class is created
template <typename T>
void addUserModule(std::string name, unsigned size, std::map<std::string, std::string> args, Module* m)
{
    std::cout << "Adding new user created module...\n";
    m->addSubModule(new T(name, size, args));
}


//The user must modify this struct in order for their changes to be seen
//Examine the readme for more details
struct userFunctions
{
    std::vector<std::string> functionNames;
    std::vector<void (*)(std::string, unsigned, std::map<std::string, std::string>, Module*)>functionVector;
    userFunctions()
        : functionNames()
        , functionVector()
    {
        //functionVector.push_back(addUserModule<YourClassHere>);
        //functionNames.push_back("YourXMLIdentifierHere");
    }
};

