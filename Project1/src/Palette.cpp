#include "Palette.h"

Color::RGB_t Color::Palette_t::find_closest_to(RGB_t color) const {
    if (this->empty()) return color;

    // assume `this->at(0)` is closest to `color`
    size_t closest_id = 0;
    int closest_distance = Color::euclidean_distance_square(color, this->at(0));

    for (size_t id = 1; id < this->size(); ++id) {
        int distance = Color::euclidean_distance_square(color, this->at(id));

        if (distance < closest_distance) {
            closest_id = id;
            closest_distance = distance;
        }
    }

    return this->at(closest_id);
}
