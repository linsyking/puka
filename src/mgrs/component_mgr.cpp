#include "component_mgr.hpp"
#include "components/rigidbody.hpp"
#include "game.hpp"
#include "utils/component.hpp"
#include <memory>
#include <string>

namespace Engine {

component_ref ComponentManager::create_component(const std::string &type) {
    if (type == "Rigidbody") {
        // Built-in component
        auto comp  = std::make_shared<RigidbodyComponent>();
        comp->type = type;
        return comp;
    } else {
        // Lua component, find a lua VM to create the component
        size_t run_id = game().get_task_manager().find_next_vm();
        return game().get_task_manager().runners[run_id].create_component(type);
    }
}

}  // namespace Engine
