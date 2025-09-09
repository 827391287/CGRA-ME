/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/


#ifndef __MAPPING_PROCEDURES_H__
#define __MAPPING_PROCEDURES_H__

#include <CGRA/Mapping.h>
#include <CGRA/OpGraph.h>
#include <CGRA/utility/ConfigGraph.h>


/**
 * Using the information parsed from a DOT file generated from a previous
 * mapping, takes in an empty mapping by reference and maps it. Creates a
 * MappingGraph as well to be used for visualization, latency checking etc.
 * The opgraph used should be the same opgraph that was used for the mapping in
 * the DOT file.
 */
CreateMappingGraphResult createMappingGraphFromConfig(Mapping & mapping, const ConfigGraph & config, const OpGraph & opgraph, const MRRG & mrrg);


#endif // __MAPPINGPROCEDURES_H__
