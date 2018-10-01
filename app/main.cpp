#include <algorithm>
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>
#include "libfive.h"
#include "libfive/eval/eval_point.hpp"
#include "libfive/eval/deck.hpp"
#include "png_writer.h"
#include "gear_generator.h"

void render(Kernel::Tree tree, char const* filename) {
    auto t = std::make_shared<Kernel::Deck>(tree);
    Kernel::PointEvaluator e(t);

    auto x0 = -10.0f;
    auto y0 = -10.0f;
    auto x1 = 10.0f;
    auto y1 = 10.0f;
    auto n_pixels_per_axis = 50;
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
                    auto x = x0 + (i * n_samples_per_pixel_per_axis + k + 0.5f) * dx;
                    auto y = y0 + (j * n_samples_per_pixel_per_axis + l + 0.5f) * dy;
                    auto value = e.eval({x, y, 0});
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
    // Create bare random number generators.
    std::random_device rd{};
    const auto seed = rd();
    std::cout << "Using seed " << seed << '\n';
    auto rand_gen = std::mt19937(seed);

    // Create distrubutions from which to sample gear parameters.
    // For now we vary number of teeth, modulus, and angle.
    constexpr auto min_teeth = 8;
    constexpr auto max_teeth = 24;
    auto teeth_distribution = std::uniform_int_distribution<uint32_t>(min_teeth, max_teeth);
    // Last time I checked, sizeof(std::uniform_real_distribution<>) == 16.
    auto modulus_distributions = std::array<std::uniform_real_distribution<float>, max_teeth - min_teeth + 1>();
    auto min_inner_radius = 1.5f;
    auto max_outer_radius = 9.5f;
    // 0.349066 is the (for now fixed) pressure angle
    // TODO don't write these numbers twice! They will be different in a matter of days.
    // This ensures that each tooth is at least so many units (the numerator) wide.
    auto global_min_modulus = 1.5f / (3.1415926f * std::cos(0.349066f));
    for (auto n_teeth = min_teeth; n_teeth <= max_teeth; ++n_teeth) {
        auto i = n_teeth - min_teeth;
        // 1.1 is the (for now fixed) dedendum parameter
        auto min_modulus = std::max(global_min_modulus, min_inner_radius / (0.5f * n_teeth - 1.1f));
        // 1.0 is the (for now fixed) addendum parameter
        auto max_modulus = max_outer_radius / (0.5f * n_teeth + 1.0f);
        std::cout << n_teeth << " teeth, min mod " << min_modulus << " max mod " << max_modulus << '\n';
        modulus_distributions[i] = std::uniform_real_distribution<float>(min_modulus, max_modulus);
    }
    auto angle_distribution = std::uniform_real_distribution<float>(0.0, 1.0);

    // Helpful things.
    constexpr auto n_samples = 100;
    const auto gear_name = [](uint32_t teeth, float modulus, float angular_pos) -> std::string {
        std::ostringstream name;
        name << std::fixed << std::setprecision(2);
        name << "./gears/" << "gear_t" << teeth << "_m" << modulus << "_a" << angular_pos << ".png";
        return name.str();
    };

    // Sample from gear-space.
    auto gear_params = GearParams{};
    gear_params.inputs.addendum = 1;
    gear_params.inputs.dedendum = 1.1;
    gear_params.inputs.pressure_angle = 0.349066;
    for (auto i = 0; i < n_samples; ++i) {
        const auto n_teeth = teeth_distribution(rand_gen);
        const auto modulus = modulus_distributions[n_teeth - min_teeth](rand_gen);
        const auto angle = angle_distribution(rand_gen);

        gear_params.inputs.n_teeth = n_teeth;
        gear_params.inputs.module = modulus;
        gear_params.inputs.angle = angle;
        gear_params.CalculateParams();
        const auto gear = generate_gear(gear_params);

        const auto filename = gear_name(n_teeth, modulus, angle);
        render(gear, filename.c_str());

        //std::cout << filename << '\n';
        //std::cout << gear_params << '\n';
    }

    // Debug: Check that angle shifting works
    /*
    gear_params.inputs.n_teeth = 6;
    gear_params.inputs.module = 2;
    gear_params.CalculateParams();
    for (auto i = 0; i < 10; ++i) {
        const auto angle = i * 0.1;
        gear_params.inputs.angle = angle;
        auto gear = generate_gear(gear_params);
        auto filename = gear_name(6, 2, angle);
        render(gear, filename.c_str());
    }
    */

    return 0;
}

