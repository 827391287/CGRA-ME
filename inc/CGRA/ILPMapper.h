/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#pragma once

#include <CGRA/Mapper.h>

#include <set>
#include <string>

enum class ILPMapperStatus
{
    INFEASIBLE,
    TIMEOUT,
    OPTIMAL_FOUND,
    SUBOPTIMAL_FOUND,
    INTERRUPTED,
    UNLISTED_STATUS
};

class ILPMapper : public Mapper {
    public:
        enum class ConstraintGroup : int;
        using ConstraintGroupSet = std::set<ConstraintGroup>;

        ILPMapper(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args);
        ~ILPMapper() = default;

        Mapping mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string>  fix_port) override;

        ConfigStore flags = {};
        ConstraintGroupSet lazy_constraints = {};
    private:
        ILPMapperStatus GurobiMap(const OpGraph& opgraph, int II, Mapping* mapping, const MRRG& mrrg, std::unordered_map<std::string, std::string>  fix_port);

        double mipgap;
        int    solnlimit;
};
