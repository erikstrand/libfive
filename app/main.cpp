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
    auto n_pixels_per_axis = 100;
    auto n_samples_per_pixel_per_axis = 3;
    float n_samples_per_pixel = n_samples_per_pixel_per_axis * n_samples_per_pixel_per_axis;
    auto n_samples_per_axis = n_pixels_per_axis * n_samples_per_pixel_per_axis;

    PngWriter png_writer;
    png_writer.Allocate(n_pixels_per_axis, n_pixels_per_axis);
    png_writer.SetAllPixelsBlack();

    auto dx = (x1 - x0) / n_samples_per_axis;
    auto dy = (y1 - y0) / n_samples_per_axis;
    for (auto i = 0; i < n_pixels_per_axis; ++i) {
        for (auto j = 0; j < n_pixels_per_axis; ++j) {
            auto n_inside = 0;
            for (auto k = 0; k < n_samples_per_pixel_per_axis; ++k) {
                for (auto l = 0; l < n_samples_per_pixel_per_axis; ++l) {
                    auto x = x0 + (i * n_samples_per_pixel_per_axis + k + 0.5) * dx;
                    auto y = y0 + (j * n_samples_per_pixel_per_axis + l + 0.5) * dy;
                    auto value = e.eval({x, y, 0.0});
                    if (value < 0) {
                        ++n_inside;
                    }
                }
            }
            uint8_t brightness = static_cast<float>(n_inside) / n_samples_per_pixel  * 255;
            png_writer.SetPixel(i, j, brightness);
        }
    }
    
    png_writer.Write(filename);
};


int main() {
    auto gear_params = GearParams{};
    gear_params.inputs.module = 1.2;
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

