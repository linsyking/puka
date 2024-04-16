#pragma once

#include <memory>
#include <vector>
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"
#include "utils/types.hpp"

namespace Engine {

class Actor;
struct Collision {
    Actor *other = nullptr;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

struct HitResult {
    Actor *actor = nullptr;
    b2Vec2 point;
    b2Vec2 normal;
    bool   is_trigger;
    float  fraction;
};

class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact *contact) override;
    void EndContact(b2Contact *contact) override;
};

class OnceRayCastCallback : public b2RayCastCallback {
public:
    float ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal,
                        float fraction) override;
};

class AllRayCastCallback : public b2RayCastCallback {
public:
    float ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal,
                        float fraction) override;
};

class Box2DManager {
public:
    std::vector<HitResult>               hit_results;
    std::shared_ptr<b2World>             world;
    std::shared_ptr<ContactListener>     contact_listener;
    std::shared_ptr<OnceRayCastCallback> once_raycast_callback;
    std::shared_ptr<AllRayCastCallback>  all_raycast_callback;
    Box2DManager();
    void step();
};

// Lua bindings

lua_ref_raw raycast(b2Vec2, b2Vec2, float);

lua_ref_raw raycast_all(b2Vec2, b2Vec2, float);

}  // namespace Engine
