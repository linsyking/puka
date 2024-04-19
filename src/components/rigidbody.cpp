#include "components/rigidbody.hpp"
#include <cstdint>
#include <mutex>
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_common.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include "game.hpp"
namespace Engine {

float deg2rad(float deg) {
    return deg * (b2_pi / 180.0f);
}

float rad2deg(float rad) {
    return rad * (180.0f / b2_pi);
}

RigidbodyComponent::RigidbodyComponent() {
    if (!game().get_box2d_manager().has_value()) {
        // Initialize Box2DManager
        game().get_box2d_manager() = Box2DManager();
    }
}

void RigidbodyComponent::init() {
    has_start       = true;
    has_update      = true;
    has_late_update = false;
}

void RigidbodyComponent::set_def(Rigidbody_Def &def) {
    this->def = def;
}

void RigidbodyComponent::on_start() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    b2BodyDef                    body_def;
    body_def.position = b2Vec2(x, y);
    if (def.body_type == "dynamic") {
        body_def.type = b2_dynamicBody;
    } else if (def.body_type == "static") {
        body_def.type = b2_staticBody;
    } else if (def.body_type == "kinematic") {
        body_def.type = b2_kinematicBody;
    }

    body_def.bullet         = def.precise;
    body_def.gravityScale   = gravity_scale;
    body_def.angularDamping = angular_friction;
    body_def.angle          = deg2rad(rotation);

    body = game().get_box2d_manager()->world->CreateBody(&body_def);

    // Basic fixture

    if (!def.has_collider && !def.has_trigger) {
        b2PolygonShape phantom_shape;
        phantom_shape.SetAsBox(def.width * 0.5f, def.height * 0.5f);

        b2FixtureDef phantom_fixture_def;
        phantom_fixture_def.shape   = &phantom_shape;
        phantom_fixture_def.density = density;

        phantom_fixture_def.isSensor        = true;
        phantom_fixture_def.filter.maskBits = 0;
        body->CreateFixture(&phantom_fixture_def);
    } else {
        if (def.has_collider) {
            b2FixtureDef fixture_def;
            fixture_def.density     = density;
            fixture_def.friction    = def.friction;
            fixture_def.restitution = def.bounciness;
            fixture_def.isSensor    = false;
            b2PolygonShape box;
            b2CircleShape  circle;
            if (def.collider_type == "box") {
                box.SetAsBox(def.width * 0.5f, def.height * 0.5f);
                fixture_def.shape = &box;
            } else if (def.collider_type == "circle") {
                circle.m_radius   = def.radius;
                fixture_def.shape = &circle;
            } else {
                game().terminate();
                throw std::runtime_error("invalid collider type");
            }
            fixture_def.filter.categoryBits = 0b0010;
            fixture_def.filter.maskBits     = 0b0010;
            fixture_def.userData.pointer    = reinterpret_cast<uintptr_t>(actor);
            body->CreateFixture(&fixture_def);
        }

        if (def.has_trigger) {
            b2FixtureDef fixture_def;
            fixture_def.density     = density;
            fixture_def.friction    = def.friction;
            fixture_def.restitution = def.bounciness;
            fixture_def.isSensor    = true;
            b2PolygonShape box;
            b2CircleShape  circle;
            if (def.trigger_type == "box") {
                box.SetAsBox(def.trigger_width * 0.5f, def.trigger_height * 0.5f);
                fixture_def.shape = &box;
            } else if (def.trigger_type == "circle") {
                circle.m_radius   = def.trigger_radius;
                fixture_def.shape = &circle;
            } else {
                game().terminate();
                throw std::runtime_error("invalid collider type");
            }
            fixture_def.filter.categoryBits = 0b0100;
            fixture_def.filter.maskBits     = 0b0100;
            fixture_def.userData.pointer    = reinterpret_cast<uintptr_t>(actor);
            body->CreateFixture(&fixture_def);
        }
    }
}

void RigidbodyComponent::update() {}

void RigidbodyComponent::late_update() {}

b2Vec2 RigidbodyComponent::get_position() {
    if (body) {
        std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
        return body->GetPosition();
    } else {
        return b2Vec2(x, y);
    }
}

float RigidbodyComponent::get_rotation() {
    if (body) {
        std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
        return rad2deg(body->GetAngle());
    } else {
        return rotation;
    }
}

void RigidbodyComponent::add_force(b2Vec2 force) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    body->ApplyForceToCenter(force, true);
}

void RigidbodyComponent::set_velocity(b2Vec2 vel) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    body->SetLinearVelocity(vel);
}

void RigidbodyComponent::set_position(b2Vec2 pos) {
    if (body) {
        std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
        body->SetTransform(pos, body->GetAngle());
    } else {
        x = pos.x;
        y = pos.y;
    }
}

void RigidbodyComponent::set_rotation(float r) {
    if (body) {
        std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
        body->SetTransform(body->GetPosition(), deg2rad(r));
    } else {
        rotation = r;
    }
}

void RigidbodyComponent::set_angular_velocity(float v) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    body->SetAngularVelocity(deg2rad(v));
}

void RigidbodyComponent::set_gravity_scale(float g) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    body->SetGravityScale(g);
}

void RigidbodyComponent::set_up_direction(b2Vec2 d) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    d.Normalize();
    float new_angle_radians = glm::atan(d.x, -d.y);
    body->SetTransform(body->GetPosition(), new_angle_radians);
}

void RigidbodyComponent::set_right_direction(b2Vec2 d) {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    d.Normalize();
    float new_angle_radians = glm::atan(d.x, -d.y) - b2_pi / 2.0f;
    body->SetTransform(body->GetPosition(), new_angle_radians);
}

b2Vec2 RigidbodyComponent::get_velocity() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    return body->GetLinearVelocity();
}

float RigidbodyComponent::get_angular_velocity() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    return rad2deg(body->GetAngularVelocity());
}

float RigidbodyComponent::get_gravity_scale() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    return body->GetGravityScale();
}

b2Vec2 RigidbodyComponent::get_up_direction() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    float                        angle = body->GetAngle();
    b2Vec2                       res   = b2Vec2(glm::sin(angle), -glm::cos(angle));
    res.Normalize();
    return res;
}

b2Vec2 RigidbodyComponent::get_right_direction() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    float                        angle = body->GetAngle();
    b2Vec2                       res   = b2Vec2(glm::cos(angle), glm::sin(angle));
    res.Normalize();
    return res;
}

void RigidbodyComponent::on_destroy() {
    std::unique_lock<std::mutex> lock(game().get_box2d_manager()->mtx.get());
    game().get_box2d_manager()->world->DestroyBody(body);
}

}  // namespace Engine
