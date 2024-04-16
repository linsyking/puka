#pragma once

#include <unordered_map>
#include "mgrs/actor.hpp"
namespace Engine {
class ActorTemplateManager {
private:
    std::unordered_map<std::string, Actor> actor_templates;

public:
    ActorTemplateManager() {}
    Actor get_actor_template(const std::string &name);
};

}  // namespace Engine
