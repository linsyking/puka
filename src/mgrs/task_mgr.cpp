#include "mgrs/task_mgr.hpp"
#include "components/rigidbody.hpp"
#include "mgrs/lua_mgr.hpp"
#include "mgrs/lua_runner.hpp"
#include "game.hpp"
#include "task_runner/runner.hpp"
#include "utils/component_proxy.hpp"
#include "utils/dbg.hpp"
namespace Engine {

TaskManager &TaskManager::get() {
    return game().get_task_manager();
}

void TaskManager::init() {
    size_t thr_num = static_cast<size_t>(game().get_config().num_threads);
    runner::boot(thr_num);
    for (size_t i = 0; i < thr_num; ++i) {
        runners.emplace_back(i);
    }

    // Initialize VMs
    for (size_t i = 0; i < thr_num; i++) {
        runner::add_task(std::make_shared<init_vm_task>());
    }
    runner::commit();
    // Must wait for VMs to be initialized
    runner::wait();

    // Initialize tmp VM
    tmp_runner.init_vm();
}

void TaskManager::quit() {
    runner::quit();
    // Quit Lua VMs
    for (auto &r : runners) {
        r.quit();
    }
    tmp_runner.quit();
}

size_t TaskManager::find_next_vm() {
    std::lock_guard<std::mutex> lock(mtx);
    static size_t               cnt = 0;
    return cnt++ % runners.size();
}

void init_vm_task::run() {
    // Initialilze VM
    size_t     thr_id = runner::thread_num();
    LuaRunner &runner = TaskManager::get().runners[thr_id];
    runner.init_vm();
    DBGOUT("VM " << thr_id << " initialized");
}

void TaskManager::init_state(sol::state &state) {
    state.open_libraries();

    // Initialize debug utils
    auto dbg_namespace = state["Debug"].get_or_create<sol::table>();
    dbg_namespace.set_function("Log", lua_log);
    dbg_namespace.set_function("LogError", lua_log_error);

    // Actor bindings
    auto actor_type                 = state.new_usertype<Actor>("Actor");
    actor_type["GetName"]           = &Actor::get_name;
    actor_type["GetId"]             = &Actor::get_id;
    actor_type["GetComponentByKey"] = &Actor::get_component_by_key;
    actor_type["GetComponent"]      = &Actor::get_component_by_type;
    actor_type["GetComponents"]     = &Actor::get_components_by_type;
    actor_type["AddComponent"]      = &Actor::add_component;
    actor_type["RemoveComponent"]   = &Actor::remove_component;

    // Glm bindings
    auto glm_vec2 = state.new_usertype<glm::vec2>(
        "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>());
    glm_vec2["x"] = &glm::vec2::x;
    glm_vec2["y"] = &glm::vec2::y;

    auto component_proxy_type   = state.new_usertype<ComponentProxy>("ComponentProxy");
    component_proxy_type["get"] = &ComponentProxy::get;
    component_proxy_type["wb"]  = &ComponentProxy::wb;

    // Box2D bindings
    auto box2d_vec2 =
        state.new_usertype<b2Vec2>("Vector2", sol::constructors<b2Vec2(float, float)>());
    box2d_vec2["x"]         = &b2Vec2::x;
    box2d_vec2["y"]         = &b2Vec2::y;
    box2d_vec2["Normalize"] = &b2Vec2::Normalize;
    box2d_vec2["Length"]    = &b2Vec2::Length;
    box2d_vec2["__add"]     = &b2Vec2::operator_add;
    box2d_vec2["__sub"]     = &b2Vec2::operator_sub;
    box2d_vec2["__mul"]     = &b2Vec2::operator_mul;
    box2d_vec2["Distance"]  = &b2Distance;
    box2d_vec2["Dot"]       = static_cast<float (*)(const b2Vec2 &, const b2Vec2 &)>(&b2Dot);

    // Collision
    auto collision_type                 = state.new_usertype<Collision>("Collision");
    collision_type["other"]             = &Collision::other;
    collision_type["point"]             = &Collision::point;
    collision_type["relative_velocity"] = &Collision::relative_velocity;
    collision_type["normal"]            = &Collision::normal;

    auto rigidbody_type                  = state.new_usertype<RigidbodyComponent>("Rigidbody");
    rigidbody_type["GetPosition"]        = &RigidbodyComponent::get_position;
    rigidbody_type["GetRotation"]        = &RigidbodyComponent::get_rotation;
    rigidbody_type["AddForce"]           = &RigidbodyComponent::add_force;
    rigidbody_type["SetVelocity"]        = &RigidbodyComponent::set_velocity;
    rigidbody_type["SetPosition"]        = &RigidbodyComponent::set_position;
    rigidbody_type["SetRotation"]        = &RigidbodyComponent::set_rotation;
    rigidbody_type["SetAngularVelocity"] = &RigidbodyComponent::set_angular_velocity;
    rigidbody_type["SetGravityScale"]    = &RigidbodyComponent::set_gravity_scale;
    rigidbody_type["SetUpDirection"]     = &RigidbodyComponent::set_up_direction;
    rigidbody_type["SetRightDirection"]  = &RigidbodyComponent::set_right_direction;
    rigidbody_type["GetVelocity"]        = &RigidbodyComponent::get_velocity;
    rigidbody_type["GetAngularVelocity"] = &RigidbodyComponent::get_angular_velocity;
    rigidbody_type["GetGravityScale"]    = &RigidbodyComponent::get_gravity_scale;
    rigidbody_type["GetUpDirection"]     = &RigidbodyComponent::get_up_direction;
    rigidbody_type["GetRightDirection"]  = &RigidbodyComponent::get_right_direction;
    rigidbody_type["x"]                  = &RigidbodyComponent::x;
    rigidbody_type["y"]                  = &RigidbodyComponent::y;
    rigidbody_type["gravity_scale"]      = &RigidbodyComponent::gravity_scale;
    rigidbody_type["density"]            = &RigidbodyComponent::density;
    rigidbody_type["angular_friction"]   = &RigidbodyComponent::angular_friction;
    rigidbody_type["rotation"]           = &RigidbodyComponent::rotation;

    auto actor_namespace = state["Actor"].get_or_create<sol::table>();
    actor_namespace.set_function("Find", find_actor);
    actor_namespace.set_function("FindAll", find_actors);
    actor_namespace.set_function("Destroy", SceneManager::destroy_actor);
    actor_namespace.set_function("Instantiate", SceneManager::instantiate_actor);

    auto app_namespace = state["Application"].get_or_create<sol::table>();
    app_namespace.set_function("Quit", exit_now);
    app_namespace.set_function("GetFrame", Game::get_frame_num);
    app_namespace.set_function("OpenURL", open_url);

    auto input_namespace = state["Input"].get_or_create<sol::table>();
    input_namespace.set_function("GetKey", get_key);
    input_namespace.set_function("GetKeyDown", get_key_down);
    input_namespace.set_function("GetKeyUp", get_key_up);
    input_namespace.set_function("GetMouseButton", get_mouse);
    input_namespace.set_function("GetMouseButtonDown", get_mouse_down);
    input_namespace.set_function("GetMouseButtonUp", get_mouse_up);
    input_namespace.set_function("GetMousePosition", get_mouse_position);
    input_namespace.set_function("GetMouseScrollDelta", get_mouse_scroll_delta);

    auto text_namespace = state["Text"].get_or_create<sol::table>();
    text_namespace.set_function("Draw", draw_text);

    auto audio_namespace = state["Audio"].get_or_create<sol::table>();
    audio_namespace.set_function("Play", play_audio);
    audio_namespace.set_function("Halt", halt_audio);
    audio_namespace.set_function("SetVolume", set_volume);

    auto image_namespace = state["Image"].get_or_create<sol::table>();
    image_namespace.set_function("DrawUI", image_drawui);
    image_namespace.set_function("DrawUIEx", image_drawui_ex);
    image_namespace.set_function("Draw", image_draw);
    image_namespace.set_function("DrawEx", image_draw_ex);
    image_namespace.set_function("DrawPixel", image_draw_pixel);

    auto scene_namespace = state["Scene"].get_or_create<sol::table>();
    scene_namespace.set_function("Load", load_scene);
    scene_namespace.set_function("GetCurrent", get_current_scene_name);
    scene_namespace.set_function("DontDestroy", dont_destroy);

    auto camera_namespace = state["Camera"].get_or_create<sol::table>();
    camera_namespace.set_function("SetPosition", set_position);
    camera_namespace.set_function("GetPositionX", get_position_x);
    camera_namespace.set_function("GetPositionY", get_position_y);
    camera_namespace.set_function("SetZoom", set_zoom);
    camera_namespace.set_function("GetZoom", get_zoom);

    auto physics_namespace = state["Physics"].get_or_create<sol::table>();
    physics_namespace.set_function("Raycast", raycast);
    physics_namespace.set_function("RaycastAll", raycast_all);

    auto event_namespace = state["Event"].get_or_create<sol::table>();
    event_namespace.set_function("Publish", publish_event);
    event_namespace.set_function("Subscribe", subscribe_event);
    event_namespace.set_function("Unsubscribe", unsubscribe_event);

    auto thread_namespace = state["Thread"].get_or_create<sol::table>();
    thread_namespace.set_function("GetThreadId", runner::thread_num);
}

LuaRunner &TaskManager::get_lua_runner() {
    return get().runners[runner::thread_num()];
}

LuaRunner &TaskManager::get_lua_runner(size_t id) {
    return get().runners[id];
}

}  // namespace Engine
