#define ENABLE_FIND_BOUNDS_EXPERIMENTAL

#include <iostream>
#include "libfive.h"
#include "libfive/solve/bounds.hpp"
#include "libfive/render/brep/mesh.hpp"

int main() {
    // Unlike the C bindings, the C++ interface manages memory automatically
    // through flyweight handles and RAII.  These objects will be freed
    // when they go out of scope
    auto x = Kernel::Tree::X();
    auto y = Kernel::Tree::Y();
    auto z = Kernel::Tree::Z();

    // Arithemetic is overloaded for the Kernel::Tree type
    auto out = (x * x) + (y * y) + (z * z) - 1;

    // Automatically find the bounds of the sphere, using
    // a function that's only available in the C++ API!
    auto bounds = Kernel::findBounds(out);

    // Mesh::render returns a unique_ptr, so it cleans up automatically
    Kernel::Mesh::render(out, bounds)->saveSTL("sphere.stl");
}

