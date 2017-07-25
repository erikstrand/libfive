#pragma once

#include "ao/tree/tree.hpp"
#include "ao/render/brep/region.hpp"

namespace Kernel {

class Contours {
public:
    static std::unique_ptr<Contours> render(const Tree t, const Region<2>& r,
                                            const float max_err=1e-6);

    /*
     *  Saves the contours to an SVG file
     */
    bool saveSVG(const std::string& filename);

    /*  Contours in 2D space  */
    std::vector<std::vector<Eigen::Vector2f>> contours;

    /*  Optional bounding box */
    Region<2> bbox;
};

}   // namespace Kernel