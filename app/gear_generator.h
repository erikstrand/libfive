#include <cstdint>
#include <ostream>
#include "libfive.h"

struct GearInputs {
    float module;
    float addendum;
    float dedendum;
    float pressure_angle;
    // value between 0 and 1, interpolates between original position and minimum angular
    // displacement that achieves symmetry
    uint32_t n_teeth;
};

struct GearParams {
    GearParams() {}
    GearParams(GearInputs inputs): inputs(inputs) {
        CalculateParams();
    }

    void CalculateParams();
    float Width();

    GearInputs inputs;

    float pitch_radius;
    float base_radius;
    float addendum_height;
    float dedendum_height;
    float involute_angle;
    float pitch_angle;

    float outer_radius;
    float root_radius;
};

std::ostream& operator<<(std::ostream& os, GearParams const& params);

Kernel::Tree generate_gear(GearParams const& params);

inline Kernel::Tree extrude(Kernel::Tree tree, float lower, float upper) {
    return max(tree, max(lower - Kernel::Tree::Z(), Kernel::Tree::Z() - upper));
}

