#ifndef IVW_GEOMETRY_H
#define IVW_GEOMETRY_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/datastructures/data.h>
#include <inviwo/core/datastructures/geometry/geometryrepresentation.h>

namespace inviwo {

class IVW_CORE_API Geometry : public Data {

public:
    Geometry();
    Geometry(GeometryRepresentation* rep);
    virtual ~Geometry();
    virtual Data* clone() const;
protected:
    void createDefaultRepresentation() const;
};

} // namespace

#endif // IVW_GEOMETRY_H
