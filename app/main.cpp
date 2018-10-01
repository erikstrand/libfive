#include <iostream>
#include "libfive.h"
#include "libfive/eval/eval_point.hpp"
#include "libfive/eval/deck.hpp"
#include "png_writer.h"
#include "gear_generator.h"

void render(Kernel::Tree tree, char const* filename) {
    auto t = std::make_shared<Kernel::Deck>(tree);
    Kernel::PointEvaluator e(t);

    auto x0 = -10.0;
    auto y0 = -10.0;
    auto x1 = 10.0;
    auto y1 = 10.0;
    auto n_pixels_per_axis = 500;

    PngWriter png_writer;
    png_writer.Allocate(n_pixels_per_axis, n_pixels_per_axis);
    png_writer.SetAllPixelsBlack();

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
    
    png_writer.Write(filename);
};


int main() {
    auto gear_params = GearParams{};
    gear_params.inputs.module = 1.5;
    gear_params.inputs.addendum = 1;
    gear_params.inputs.dedendum = 1.1;
    gear_params.inputs.pressure_angle = 0.349066;
    gear_params.inputs.n_teeth = 10;
    gear_params.CalculateParams();
    std::cout << gear_params;

    auto gear = generate_gear(gear_params);
    render(gear, "gear.png");

    return 0;
}

