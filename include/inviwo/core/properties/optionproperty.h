#ifndef IVW_OPTIONPROPERTY_H
#define IVW_OPTIONPROPERTY_H

#include "inviwo/core/properties/property.h"

namespace inviwo {

class OptionProperty : public Property {

public:
    OptionProperty(std::string identifier, std::string displayName, bool value);
    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& d);
};

} // namespace

#endif // IVW_OPTIONPROPERTY_H