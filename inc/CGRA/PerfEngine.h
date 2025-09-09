/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __PERFENGINE__H__
#define __PERFENGINE__H__

#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <memory>
#include <fstream>
#include <mini.hpp>
#include <CGRA/CGRA.h>
#include <CGRA/Mapping.h>
#include <CGRA/Util.h>
#include <CGRA/TatumInterface.h>

typedef std::multimap<MRRG::NodeDescriptor, MRRG::NodeDescriptor> EdgeList;

class PerfEngine
{
    public:
        PerfEngine(std::string ini_path);
        virtual ~PerfEngine();
        void reportArea(std::shared_ptr<CGRA> target);
        void reportTiming(std::shared_ptr<CGRA> target, const Mapping& mapping);
        void reportPower(std::shared_ptr<CGRA> target);
        std::map<MRRG::NodeDescriptor, OpGraphOpCode> mapped_node2opcode;
    private:
        std::map<std::string, double> areaModels;
        std::map<std::string, double> timingModels;
        std::map<std::string, std::string> powerModels;
        void _parseAreaINI(std::string file_str);
        void _parseTimingINI(std::string file_str);
        void _parsePowerINI(std::string file_str);
        double _setModuleArea(Module* m, unsigned int level, bool last);
        EdgeList _genMappedMRRG(const OpGraph& og, std::map<const OpGraphNode*,std::vector<MRRG::NodeDescriptor>>& m);
        void reportLevelOrderArea(Module* m);
};

#endif

