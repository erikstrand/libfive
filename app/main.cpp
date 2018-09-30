#define ENABLE_FIND_BOUNDS_EXPERIMENTAL

#include <iostream>
#include "libfive.h"
#include "libfive/solve/bounds.hpp"
#include "libfive/render/brep/mesh.hpp"

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

    void CalculateParams() {
        pitch_radius = 0.5 * inputs.module * inputs.n_teeth;
        base_radius = std::cos(inputs.pressure_angle) * pitch_radius;
        addendum_height = inputs.addendum * inputs.module;
        dedendum_height = inputs.dedendum * inputs.module;
        involute_angle = std::tan(inputs.pressure_angle) - inputs.pressure_angle;
        pitch_angle = 6.28318530718 / inputs.n_teeth;
    }

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

auto generate_gear(GearParams const& params) {
    auto x = Kernel::Tree::X();
    auto y = Kernel::Tree::Y();
    auto z = Kernel::Tree::Z();

    // ATTEMPT 1
    // max <--> intersection    distances outside may be wrong
    // min <--> union           distances inside may be wrong
    // difference <--> max(shape, -tool)
    /*
    return sqrt(x * x) + (y * y) - 1;

    // inner cylinder?
    // must be inside tip
    // var fn = `${rp}+${ha}-Math.sqrt(X*X+Y*Y)`
    //auto fn1 = params.pitch_radius + params.addendum_height - sqrt(x * x + y * y);
    auto fn1 = sqrt(x * x + y * y) - (params.pitch_radius + params.addendum_height);
    //auto fn1 = r - (params.pitch_radius + params.addendum_height);

     // angle must be above bottom involute
     //var fn = Math.min(${fn},(Math.PI+Math.atan2(Y,X))%${ap}-(Math.sqrt(Math.pow(Math.max(${rb},Math.sqrt(X*X+Y*Y))/${rb},2)-1)-Math.acos(${rb}/Math.max(${rb},Math.sqrt(X*X+Y*Y)))))

    // distance from center in units of base radius, capped below at 1
    //auto dst1 = max(params.base_radius, r) / params.base_radius;
    // hmm
    //auto dst2 = sqrt(pow(dst1, 2) - 1);

    //auto angle1 = acos(params.base_radius / min(params.base_radius, r));
    //auto angle2 =  - angle1;
    //auto fn2 = max(fn1, (Math.PI + atan2(y, x)) % params.pitch_angle - )
    return fn1;

     // angle must be below top involute
     // var fn = `Math.min(${fn},-(Math.sqrt(Math.pow(Math.max(${rb},Math.sqrt(X*X+Y*Y))/${rb},2)-1)-Math.acos(${rb}/Math.max(${rb},Math.sqrt(X*X+Y*Y))))-(-${ap/2+2*ai}+(Math.PI+Math.atan2(Y,X))%${ap}))`

     // root circle
     // var fn = `Math.max(${fn},${rp-hd}-Math.sqrt(X*X+Y*Y))`
     */


    auto printer = [&](auto tree) {
        auto t = std::make_shared<Kernel::Deck>(tree);
        Kernel::PointEvaluator e(t);

        auto x0 = 0.01;
        auto y0 = 0.01;
        auto x1 = 20.0;
        auto y1 = 20.0;
        auto n = 3;
        auto dx = (x1 - x0) / n;
        auto dy = (y1 - y0) / n;
        for (auto i = 0; i < n; ++i) {
            for (auto j = 0; j < n; ++j) {
                auto x = x0 + i * dx;
                auto y = y0 + j * dy;
                std::cout << x << ", " << y << ": " << e.eval({x, y, 0.0}) << '\n';
            }
        }
    };


    // ATTEMPT 2
    auto r_squared = x * x + y * y;
    auto r = sqrt(r_squared);
    auto theta = atan2(y, x);

    auto addendum_circle = r - (params.pitch_radius + params.addendum_height);

    auto f1 = (params.pitch_radius + params.addendum_height) - r;
    auto f15 = mod((3.1415926 + atan2(y, x)), params.pitch_angle);
    auto f2 = min(f1, mod((3.1415926 + atan2(y, x)), params.pitch_angle) - (sqrt(pow(max(params.base_radius, sqrt(x*x+y*y))/params.base_radius, 2)-1) - acos(params.base_radius/max(params.base_radius,sqrt(x*x+y*y)))));
     auto f3 = min(f2,-(sqrt(pow(max(params.base_radius,sqrt(x*x+y*y))/params.base_radius,2)-1)-acos(params.base_radius/max(params.base_radius,sqrt(x*x+y*y))))-(-(params.pitch_angle/2+2*params.involute_angle)+mod((3.1415926+atan2(y,x)), params.pitch_angle)));
    printer(f3);




    auto dst_to_tangent = sqrt(r_squared - params.base_radius * params.base_radius);
    auto angle_to_tangent = acos(params.base_radius / r);
    auto involute = dst_to_tangent - params.base_radius * (theta + angle_to_tangent);
    return involute;

    // depends on theta, not r
    auto tooth_pos = mod((3.1415926 + atan2(y, x)), params.pitch_angle);
    // distance from center in units of base radius, capped below at 1
    auto dst1 = max(params.base_radius, r) / params.base_radius;
    // inverse of dst1
    auto dst2 = params.base_radius / max(params.base_radius, r);
    return tooth_pos + r - 5;

    auto fn1 = (params.pitch_radius + params.addendum_height) - r;
    //auto fn1 = r - (params.pitch_radius + params.addendum_height);
    auto fn2 = -min(fn1, tooth_pos - (sqrt(pow(dst1, 2) - 1) - acos(dst2)));
    //auto fn3 = min(fn2, -(sqrt(pow(max(params.base_radius, r) / params.base_radius, 2) - 1) - acos(params.base_radius / max(params.base_radius, r))) - (-(params.pitch_angle / 2 + 2 * params.involute_angle) + mod((3.1415926 + atan2(y, x)), params.pitch_angle)));
    //auto fn4 = max(fn3, (params.pitch_radius - params.dedendum_height) - r);
    return fn2;
}

Kernel::Tree extrude(Kernel::Tree tree, float lower, float upper) {
    return max(tree, max(lower - Kernel::Tree::Z(), Kernel::Tree::Z() - upper));
}

int main() {
    // Unlike the C bindings, the C++ interface manages memory automatically
    // through flyweight handles and RAII.  These objects will be freed
    // when they go out of scope
    auto x = Kernel::Tree::X();
    auto y = Kernel::Tree::Y();
    auto z = Kernel::Tree::Z();

    // Arithemetic is overloaded for the Kernel::Tree type
    auto sphere = (x * x) + (y * y) + (z * z) - 1;
    auto infinite_cylinder = sqrt(x * x) + (y * y) - 1;
    auto cylinder = extrude(infinite_cylinder, 0, 1);

    auto gear_params = GearParams{};
    gear_params.inputs.module = 1;
    gear_params.inputs.addendum = 1;
    gear_params.inputs.dedendum = 1.1;
    gear_params.inputs.pressure_angle = 0.349066;
    gear_params.inputs.n_teeth = 10;
    gear_params.CalculateParams();
    std::cout << gear_params;

    auto infinite_something = generate_gear(gear_params);
    auto something = extrude(infinite_something, 0, 1);

    // Automatically find the bounds of the sphere, using
    // a function that's only available in the C++ API!
    auto bounds = Kernel::findBounds(something);

    // Mesh::render returns a unique_ptr, so it cleans up automatically
    Kernel::Mesh::render(something, bounds)->saveSTL("something.stl");

    return 0;
}
