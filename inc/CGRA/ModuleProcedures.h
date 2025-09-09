/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef CGRAME_MODULE_PROCUDURES_HPP
#define CGRAME_MODULE_PROCUDURES_HPP

#include <CGRA/Module.h>

/**
 * Determine if `m` or any submodules have any synchronous circuitry
 */
bool moduleRequiresClockPorts(const Module& m);

/**
 * Determine if `m` or any submodules have any config cells
 */
bool moduleRequiresConfigPorts(const Module& m);

#endif /* CGRAME_MODULE_PROCUDURES_HPP */
