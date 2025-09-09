#ifndef __VISUAL_H
#define __VISUAL_H

#include <string>
#include <memory>
#include <unordered_map>

#include <CGRA/CGRA.h>
#include <CGRA/Mapping.h>

void genCGRAVisual(std::string exe_path, std::shared_ptr<CGRA> cgra, int II);
void genMappingVisual(
    std::string exe_path, const MRRG& mrrg,
    const Mapping& mapping, const MappingGraph& mg,
    const CreateMappingGraphResult::ToMRRG& toMRRG
);
// Scales the float/fractional representation of a position to a pixel coordinate on the defined screen size.
std::pair<double, double> getScaledPosition(const std::pair<double, double> pos); 


#endif
