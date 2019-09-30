#ifndef DAUNTLESS_ASS_GEAR_ELEMENTS_HPP
#define DAUNTLESS_ASS_GEAR_ELEMENTS_HPP

namespace Gear
{
enum Element { All = 0, Elementless, Fire, Ice, Shock, Terra, Umbral, Radiant };
struct Elements
{
    int Fire = 0;
    int Ice = 0;
    int Shock = 0;
    int Terra = 0;
    int Radiant = 0;
    int Umbral = 0;
};
} // namespace Gear

#endif // DAUNTLESS_ASS_GEAR_ELEMENTS_HPP