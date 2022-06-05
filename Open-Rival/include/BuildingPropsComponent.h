#ifndef BUILDING_PROPS_COMPONENT_H
#define BUILDING_PROPS_COMPONENT_H

#include "Building.h"
#include "EntityComponent.h"

namespace Rival {

    /**
     * Component containing basic building properties.
     */
    class BuildingPropsComponent : public EntityComponent {

    public:
        static const std::string key;

        BuildingPropsComponent(Building::Type type);

        Building::Type getBuildingType() const {
            return type;
        }

    private:
        Building::Type type;
    };

}  // namespace Rival

#endif  // BUILDING_PROPS_COMPONENT_H
