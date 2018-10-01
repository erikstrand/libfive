#include <cstdint>
#include <ostream>
#include "libfive.h"

struct GearInputs {
    double module;
    double addendum;
    double dedendum;
    double pressure_angle;
    uint32_t n_teeth;
};

struct GearParams {
    GearParams() {}
    GearParams(GearInputs inputs): inputs(inputs) {
        CalculateParams();
    }

    void CalculateParams();

    GearInputs inputs;

    double pitch_radius;
    double base_radius;
    double addendum_height;
    double dedendum_height;
    double involute_angle;
    double pitch_angle;

    double outer_radius;
    double root_radius;
};

std::ostream& operator<<(std::ostream& os, GearParams const& params);

Kernel::Tree generate_gear(GearParams const& params);

inline Kernel::Tree extrude(Kernel::Tree tree, float lower, float upper) {
    return max(tree, max(lower - Kernel::Tree::Z(), Kernel::Tree::Z() - upper));
}

