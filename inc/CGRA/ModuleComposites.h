/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __MODULE_COMPOSITES__H__
#define __MODULE_COMPOSITES__H__

#include <CGRA/Module.h>

class MemPort : public Module
{
    public:
        MemPort(std::string name, Location, int num_connections, int size, int num_const_addresses, bool pred = false, int II = 1);
        virtual std::string GenericName();
        virtual ~MemPort();
    private:
        int num_connections; // number of muxed inputs to this memory unit
        int num_const_addresses; //< number of unique const addresses supported
};

class IOPort : public Module
{
    public:
        IOPort(std::string name, Location, int num_inputs, int size = DEFAULT_SIZE);
        virtual std::string GenericName();
        virtual ~IOPort();
    private:
        int num_inputs; // number of muxed inputs to this IO
};

class SimpleFU : public Module
{
    public:
        SimpleFU(std::string name, Location, CGRABlockType blockType = STANDARD_NOBYPASS);
        virtual std::string GenericName();
        virtual ~SimpleFU();
        CGRABlockType blockType;
};

#endif

