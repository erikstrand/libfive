#define ENABLE_FIND_BOUNDS_EXPERIMENTAL

#include <iostream>
#include "libfive.h"
#include "libfive/solve/bounds.hpp"
#include "libfive/render/brep/mesh.hpp"
#include "png_writer.h"

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


    auto r_squared = x * x + y * y;
    auto r = sqrt(r_squared);
    auto theta = atan2(y, x);

    auto addendum_circle = r - (params.pitch_radius + params.addendum_height);

    auto f1 = (params.pitch_radius + params.addendum_height) - r;
    auto f2 = min(f1, mod((3.1415926 + atan2(y, x)), params.pitch_angle) - (sqrt(pow(max(params.base_radius, r)/params.base_radius, 2)-1) - acos(params.base_radius/max(params.base_radius,r))));
    auto f3 = min(f2,-(sqrt(pow(max(params.base_radius,r)/params.base_radius,2)-1)-acos(params.base_radius/max(params.base_radius,r)))-(-(params.pitch_angle/2+2*params.involute_angle)+mod((3.1415926+atan2(y,x)), params.pitch_angle)));

    auto f4 = max(f3, params.pitch_radius - params.dedendum_height - r);
    printer(f4);
    return -f4;
}

Kernel::Tree extrude(Kernel::Tree tree, float lower, float upper) {
    return max(tree, max(lower - Kernel::Tree::Z(), Kernel::Tree::Z() - upper));
}

void render(Kernel::Tree tree, char const* filename) {
    std::cout << "render 1\n";
    auto t = std::make_shared<Kernel::Deck>(tree);
    std::cout << "render 2\n";
    Kernel::PointEvaluator e(t);
    std::cout << "render 3\n";

    auto x0 = -10.0;
    auto y0 = -10.0;
    auto x1 = 10.0;
    auto y1 = 10.0;
    auto n_pixels_per_axis = 100;

    std::cout << "render 4\n";
    PngWriter png_writer;
    std::cout << "render 5\n";
    png_writer.Allocate(n_pixels_per_axis, n_pixels_per_axis);
    std::cout << "render 6\n";
    png_writer.Clear();
    std::cout << "render 7\n";

    auto dx = (x1 - x0) / n_pixels_per_axis;
    auto dy = (y1 - y0) / n_pixels_per_axis;
    for (auto i = 0; i < n_pixels_per_axis; ++i) {
        auto x = x0 + (i + 0.5) * dx;
        for (auto j = 0; j < n_pixels_per_axis; ++j) {
            auto y = y0 + (j + 0.5) * dy;
            auto value = e.eval({x, y, 0.5});
            if (value < 0) {
                png_writer.SetPixel(i, j);
            }
            //std::cout << x << ", " << y << ": " << value << '\n';
        }
    }
    std::cout << "render 8\n";
    
    png_writer.Write(filename);
};


int main() {
    // Unlike the C bindings, the C++ interface manages memory automatically
    // through flyweight handles and RAII.  These objects will be freed
    // when they go out of scope
    auto x = Kernel::Tree::X();
    auto y = Kernel::Tree::Y();
    auto z = Kernel::Tree::Z();

    // Arithemetic is overloaded for the Kernel::Tree type
    auto sphere = (x * x) + (y * y) + (z * z) - 30;
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
    std::cout << "prerender\n";
    render(something, "gear.png");
    std::cout << "postrender\n";

    // Automatically find the bounds of the sphere, using
    // a function that's only available in the C++ API!
    auto bounds = Kernel::findBounds(something);

    // Mesh::render returns a unique_ptr, so it cleans up automatically
    Kernel::Mesh::render(something, bounds)->saveSTL("something.stl");

    return 0;
}
