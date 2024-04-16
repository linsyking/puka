#include "mgrs/box2d_mgr.hpp"
#include <mutex>
#include "box2d/b2_collision.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_fixture.h"
#include "game.hpp"
#include "mgrs/actor.hpp"
#include "mgrs/lua_mgr.hpp"
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "sol/sol.hpp"
namespace Engine {

Box2DManager::Box2DManager() {
    world            = std::make_shared<b2World>(b2Vec2(0.0f, 9.8f));
    contact_listener = std::make_shared<ContactListener>();
    world->SetContactListener(contact_listener.get());
    once_raycast_callback = std::make_shared<OnceRayCastCallback>();
    all_raycast_callback  = std::make_shared<AllRayCastCallback>();
}

void Box2DManager::step() {
    world->Step(1.0f / 60.0f, 8, 3);
}

void ContactListener::BeginContact(b2Contact *contact) {
    b2Fixture *a = contact->GetFixtureA();
    b2Fixture *b = contact->GetFixtureB();
    if (a->GetUserData().pointer == 0 || b->GetUserData().pointer == 0) {
        return;
    }
    Actor          *aa = reinterpret_cast<Actor *>(a->GetUserData().pointer);
    Actor          *ba = reinterpret_cast<Actor *>(b->GetUserData().pointer);
    Collision       c;
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    c.other             = ba;
    c.point             = worldManifold.points[0];
    c.relative_velocity = a->GetBody()->GetLinearVelocity() - b->GetBody()->GetLinearVelocity();
    c.normal            = worldManifold.normal;
    if (a->IsSensor()) {
        c.point  = b2Vec2(-999.0f, -999.0f);
        c.normal = b2Vec2(-999.0f, -999.0f);
        aa->call_trigger_enter(c);
    } else {
        aa->call_collision_enter(c);
    }

    c.other = aa;
    if (b->IsSensor()) {
        c.point  = b2Vec2(-999.0f, -999.0f);
        c.normal = b2Vec2(-999.0f, -999.0f);
        ba->call_trigger_enter(c);
    } else {
        ba->call_collision_enter(c);
    }
}

void ContactListener::EndContact(b2Contact *contact) {
    b2Fixture *a = contact->GetFixtureA();
    b2Fixture *b = contact->GetFixtureB();
    if (a->GetUserData().pointer == 0 || b->GetUserData().pointer == 0) {
        return;
    }
    Actor    *aa = reinterpret_cast<Actor *>(a->GetUserData().pointer);
    Actor    *ba = reinterpret_cast<Actor *>(b->GetUserData().pointer);
    Collision c;
    c.other             = ba;
    c.relative_velocity = a->GetBody()->GetLinearVelocity() - b->GetBody()->GetLinearVelocity();
    c.point             = b2Vec2(-999.0f, -999.0f);
    c.normal            = b2Vec2(-999.0f, -999.0f);
    if (a->IsSensor()) {
        aa->call_trigger_exit(c);
    } else {
        aa->call_collision_exit(c);
    }
    c.other = aa;
    if (b->IsSensor()) {
        ba->call_trigger_exit(c);
    } else {
        ba->call_collision_exit(c);
    }
}

float OnceRayCastCallback::ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                                         const b2Vec2 &normal, float fraction) {
    Box2DManager &box = game().get_box2d_manager().value();
    if (fixture->GetUserData().pointer == 0) {
        return -1;  // Ignore
    }
    Actor    *aa = reinterpret_cast<Actor *>(fixture->GetUserData().pointer);
    HitResult hit;
    hit.actor      = aa;
    hit.point      = point;
    hit.normal     = normal;
    hit.is_trigger = fixture->IsSensor();
    hit.fraction   = fraction;
    box.hit_results.clear();
    box.hit_results.push_back(hit);
    return fraction;
}

float AllRayCastCallback::ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                                        const b2Vec2 &normal, float fraction) {
    Box2DManager &box = game().get_box2d_manager().value();
    if (fixture->GetUserData().pointer == 0) {
        return -1;  // Ignore
    }
    Actor    *aa = reinterpret_cast<Actor *>(fixture->GetUserData().pointer);
    HitResult hit;
    hit.actor      = aa;
    hit.point      = point;
    hit.normal     = normal;
    hit.is_trigger = fixture->IsSensor();
    hit.fraction   = fraction;
    box.hit_results.push_back(hit);
    return 1;
}

lua_ref_raw raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
    Box2DManager &box = game().get_box2d_manager().value();
    box.hit_results.clear();
    dir.Normalize();
    b2Vec2 end = pos + dir.operator_mul(dist);
    box.world->RayCast(box.once_raycast_callback.get(), pos, end);
    if (box.hit_results.empty()) {
        return {};
    } else {
        // Create a table
        LuaRunner                   &runner = TaskManager::get_lua_runner();
        std::unique_lock<std::mutex> lock(runner.mtx.get());
        sol::table                   table = runner.state.create_table();
        table["actor"]                     = box.hit_results[0].actor;
        table["point"]      = sol::make_object(runner.state, box.hit_results[0].point);
        table["normal"]     = sol::make_object(runner.state, box.hit_results[0].normal);
        table["is_trigger"] = box.hit_results[0].is_trigger;
        return table;
    }
}

lua_ref_raw raycast_all(b2Vec2 pos, b2Vec2 dir, float dist) {
    Box2DManager &box = game().get_box2d_manager().value();
    box.hit_results.clear();
    dir.Normalize();
    b2Vec2 end = pos + dir.operator_mul(dist);
    box.world->RayCast(box.all_raycast_callback.get(), pos, end);
    // Sort by fraction
    std::sort(box.hit_results.begin(), box.hit_results.end(),
              [](const HitResult &a, const HitResult &b) { return a.fraction < b.fraction; });
    // Create a table
    LuaRunner                   &runner = TaskManager::get_lua_runner();
    std::unique_lock<std::mutex> lock(runner.mtx.get());
    sol::table                   table = runner.state.create_table();
    for (auto &hit : box.hit_results) {
        sol::table sub_table    = runner.state.create_table();
        sub_table["actor"]      = hit.actor;
        sub_table["point"]      = sol::make_object(runner.state, hit.point);
        sub_table["normal"]     = sol::make_object(runner.state, hit.normal);
        sub_table["is_trigger"] = hit.is_trigger;
        table.add(sub_table);
    }
    return table;
}

}  // namespace Engine
