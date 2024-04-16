#pragma once
#include <string>
#include "box2d/b2_math.h"
#include "box2d/b2_body.h"
#include "utils/builtin_component.hpp"
namespace Engine {

struct Rigidbody_Def {
    std::string body_type = "dynamic";

    bool precise = true;

    bool        has_collider  = true;
    std::string collider_type = "box";
    float       width         = 1.0f;
    float       height        = 1.0f;
    float       radius        = 0.5f;
    float       friction      = 0.3f;
    float       bounciness    = 0.3f;

    bool        has_trigger    = true;
    std::string trigger_type   = "box";
    float       trigger_width  = 1.0f;
    float       trigger_height = 1.0f;
    float       trigger_radius = 0.5f;
};

float deg2rad(float deg);
float rad2deg(float rad);

class RigidbodyComponent : public BuiltinComponent {
private:
    b2Body *body = nullptr;

public:
    float x             = 0.0f;
    float y             = 0.0f;
    float gravity_scale = 1.0f;
    float density       = 1.0f;

    float angular_friction = 0.3f;
    float rotation         = 0.0f;

    Rigidbody_Def def;
    RigidbodyComponent();
    ~RigidbodyComponent() {}
    void set_def(Rigidbody_Def &def);

    void init() override;
    void on_start() override;
    void update() override;
    void late_update() override;
    void on_destroy() override;

    // Lua bindings
    b2Vec2 get_position();
    float  get_rotation();
    void   add_force(b2Vec2);
    void   set_velocity(b2Vec2);
    void   set_position(b2Vec2);
    void   set_rotation(float);
    void   set_angular_velocity(float);
    void   set_gravity_scale(float);
    void   set_up_direction(b2Vec2);
    void   set_right_direction(b2Vec2);

    b2Vec2 get_velocity();
    float  get_angular_velocity();
    float  get_gravity_scale();
    b2Vec2 get_up_direction();
    b2Vec2 get_right_direction();
};
}  // namespace Engine
