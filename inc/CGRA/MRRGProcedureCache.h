/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef MRRG_PROCEDURE_CACHE__H_
#define MRRG_PROCEDURE_CACHE__H_

#include <memory>

/**
 * MRRG Procedures may take this class as a parameter and cache results within the impl.
 */
class MRRGProcedureCache;
class MRRGProcedureCacheHandle {
public:
    MRRGProcedureCacheHandle();
    MRRGProcedureCacheHandle(const MRRGProcedureCacheHandle&);
    MRRGProcedureCacheHandle(MRRGProcedureCacheHandle&&);
    ~MRRGProcedureCacheHandle();

    MRRGProcedureCacheHandle& operator=(const MRRGProcedureCacheHandle&);
    MRRGProcedureCacheHandle& operator=(MRRGProcedureCacheHandle&&);

    explicit operator bool() const { return (bool)impl_ptr; }
    MRRGProcedureCache& operator*() const { return *impl_ptr; }
    MRRGProcedureCache* operator->() const { return get(); }

    MRRGProcedureCache* get() const { return impl_ptr.get(); }
    MRRGProcedureCache& getOrCreate();
    void reset() { impl_ptr.reset(); }
private:
    std::shared_ptr<MRRGProcedureCache> impl_ptr;
};

#endif /* MRRG_PROCEDURE_CACHE__H_ */
