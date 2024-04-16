#include "box2d/b2_math.h"

b2Vec2 b2Vec2::operator_add(const b2Vec2 &other) const {
    return b2Vec2(x + other.x, y + other.y);
}

b2Vec2 b2Vec2::operator_sub(const b2Vec2 &other) const {
    return b2Vec2(x - other.x, y - other.y);
}

b2Vec2 b2Vec2::operator_mul(const float multiplier) const {
    return b2Vec2(x * multiplier, y * multiplier);
}
