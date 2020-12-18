#pragma once

#include <string>
#include <typeinfo>

namespace Window
{
    struct Window_Type
    {
    public:
        const std::string_view _name;
        const uint32_t _channels;
        const std::string_view _type;
        const std::string_view _description;

        std::string name() const
        {
            return std::string(_name);
        }

        uint32_t channels() const
        {
            return _channels;
        }

        std::string type() const
        {
            return std::string(_type);
        }

        std::string description() const
        {
            return std::string(_description);
        }
    };

    static constexpr Window_Type create(const std::string_view name, const uint32_t channels, const std::string_view type, const std::string_view description = "")
    {
        return Window_Type{ name, channels, type, description };
    }

    template<uint32_t channels, typename type>
    static constexpr Window_Type create(const std::string_view name, const std::string_view description = "")
    {
        return create( name, channels, std::string_view(typeid(type).name()) , description );
    }

    template<typename type>
    static constexpr Window_Type create(const std::string_view name, const uint32_t channels, const std::string_view description = "")
    {
        return create( name, channels, std::string_view(typeid(type).name()) , description );
    }

    template<uint32_t channels>
    static constexpr Window_Type create(const std::string_view name, const std::string_view type, const std::string_view description = "")
    {
        return create( name, channels, type , description );
    }

    static const Window_Type HEIGHT_MAP = create<1, double>("height map");
    static const Window_Type TERRAIN = create<4, double>("terrain");
    static const Window_Type NORMAL = create<3, double>("normal");
    static const Window_Type WATER = create<3, double>("water");
    static const Window_Type WATER_PLACED = create<1, double>("water placed");
    static const Window_Type WATER_POOL = create<1, double>("water pool");
    static const Window_Type WATER_STREAM = create<2, double>("water stream");
    static const Window_Type CARRIED_SOIL = create<1, double>("carried soil");
    static const Window_Type DROPLET = create<1, double>("droplet");
    static const Window_Type DROPLET_POSITIONS = create<3, double>("droplet positions");
    static const Window_Type DROPLET_SPEED = create<3, double>("droplet speed");
    static const Window_Type DROPLET_VOLUME = create<1, double>("droplet volume");
    static const Window_Type SOIL_PICKED = create<1, double>("soil picked");
    static const Window_Type SOIL_DROPPED = create<1, double>("soil dropped");
    static const Window_Type SEDIMENT_MOVE = create<1, double> ("sediment move");
    static const Window_Type SEED = create<1, double> ("seed");
    static const Window_Type DEAD = create<1, double> ("dead");
    static const Window_Type EVAPORATE = create<1, double> ("evaporate");
};