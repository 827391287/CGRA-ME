/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __USERMODULES__H__
#define __USERMODULES__H__

#include <CGRA/Module.h>
#include <CGRA/ModuleComposites.h>
#include <CGRA/ModuleElastic.h>
#include <CGRA/ModuleFPUnit.h>


class AdresPE : public Module
{
    public:
        std::string fu_type;
        int num_inputs;
        AdresPE(std::string name, int num_inputs, std::string fu_type, int fu_II, int latency, int II, Location Loc, bool use_op_div = false, bool predExist = false, bool reg_bypass = false, bool extra_mem = false, std::string pred_scheme = "");
        virtual std::string GenericName();
        virtual ~AdresPE() {}
};

class RIKEN_PE_Elastic: public Module
{
    public:
        int pe_conn_in;
        int pe_conn_out;
        int buffer_depth;
        RIKEN_PE_Elastic(std::string name, int pe_conn_in, int pe_conn_out, int II, int latency,  Location Loc, int type, int buffer_depth = 2, bool eb_enable = true, int contexts = 1, bool pred = false, std::string pred_type = "full");
        virtual std::string GenericName();
        virtual ~RIKEN_PE_Elastic() {}
};

class HyCUBEPE: public Module
{
    public:
        bool predExist;
        int pe_conn_in;
        int pe_conn_out;
        int pred_conn_in;
        int pred_conn_out;
        HyCUBEPE(std::string name, bool predExist, int pe_conn_in, int pe_conn_out,
                int pred_conn_in, int pre_conn_out, int fu_II, int II, int latency,  Location Loc, std::string pred_scheme = "");
        virtual std::string GenericName();
        virtual ~HyCUBEPE() {}
};

#endif

