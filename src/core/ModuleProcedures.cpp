#include <CGRA/ModuleProcedures.h>

bool moduleRequiresClockPorts(const Module& m) {
    if (m.addsSynchronousCircuitry()) {
        return true;
    }

    for (const auto& nameAndSubmodule : m.submodules) {
        if (moduleRequiresClockPorts(*nameAndSubmodule.second)) {
            return true;
        }
    }

    return false;
}

bool moduleRequiresConfigPorts(const Module& m) {
    if (m.hasConfigCells() || (bool)dynamic_cast<const ConfigCell*>(&m)) {
        return true;
    }

    for (const auto& nameAndSubmodule : m.submodules) {
        if (moduleRequiresConfigPorts(*nameAndSubmodule.second)) {
            return true;
        }
    }

    return false;
}
