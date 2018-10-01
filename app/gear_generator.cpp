#include "gear_generator.h"
#include <cmath>

void GearParams::CalculateParams() {
    pitch_radius = 0.5 * inputs.module * inputs.n_teeth;
    base_radius = std::cos(inputs.pressure_angle) * pitch_radius;
    addendum_height = inputs.addendum * inputs.module;
    dedendum_height = inputs.dedendum * inputs.module;
    involute_angle = std::tan(inputs.pressure_angle) - inputs.pressure_angle;
    pitch_angle = 6.28318530718 / inputs.n_teeth;
}

std::ostream& operator<<(std::ostream& os, GearParams const& params) {
    os << "module " << params.inputs.module << '\n';
    os << "addendum " << params.inputs.addendum << '\n';
    os << "dedendum " << params.inputs.dedendum << '\n';
    os << "pressure_angle " << params.inputs.pressure_angle << '\n';
    os << "n_teeth " << params.inputs.n_teeth << '\n';
    os << "pitch_radius " << params.pitch_radius << '\n';
    os << "base_radius " << params.base_radius << '\n';
    os << "addendum_height " << params.addendum_height << '\n';
    os << "dedendum_height " << params.dedendum_height << '\n';
    os << "involute_angle " << params.involute_angle << '\n';
    os << "pitch_angle " << params.pitch_angle << '\n';
    return os;
}

Kernel::Tree generate_gear(GearParams const& params) {
    auto x = Kernel::Tree::X();
    auto y = Kernel::Tree::Y();
    auto z = Kernel::Tree::Z();

    auto r_squared = x * x + y * y;
    auto r = sqrt(r_squared);

    auto f1 = (params.pitch_radius + params.addendum_height) - r;
    auto f2 = min(f1, mod((3.1415926 + atan2(y, x)), params.pitch_angle) - (sqrt(pow(max(params.base_radius, r)/params.base_radius, 2)-1) - acos(params.base_radius/max(params.base_radius,r))));
    auto f3 = min(f2,-(sqrt(pow(max(params.base_radius,r)/params.base_radius,2)-1)-acos(params.base_radius/max(params.base_radius,r)))-(-(params.pitch_angle/2+2*params.involute_angle)+mod((3.1415926+atan2(y,x)), params.pitch_angle)));

    auto f4 = max(f3, params.pitch_radius - params.dedendum_height - r);
    return -extrude(f4, -1, 1);
}

