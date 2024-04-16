#pragma once

#include <string>
#include "utils/component.hpp"
namespace Engine {

/// Manages game components in the game
class ComponentManager {
public:
    /// Create a component of the given type
    ///
    /// May create a built-in component or a Lua component.
    ///
    /// In case of a lua component, the manager will first
    /// dispatch the component to a lua VM. Then the VM will initialize the component.
    component_ref create_component(const std::string &type);
};
}  // namespace Engine
