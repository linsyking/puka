#pragma once

#include "sol/sol.hpp"
#include "utils/lua_component.hpp"
#include "utils/types.hpp"
namespace Engine {

class ComponentProxy {
    LuaComponent *component;
    lua_ref       ref;

    /// Whether the component resides in the current VM
    bool trivial = false;

public:
    /// Copy a lua object
    void copy(lua_ref_raw&, sol::state &, lua_ref_raw&);
    ComponentProxy(LuaComponent *c, bool trivial = false) : component(c), trivial(trivial){};

    /// Get the ref
    ///
    /// Will first unlock self VM, and acquire the component VM lock.
    ///
    /// After copying the value, release the component VM lock and lock self.
    lua_ref_raw get();

    /// Write back ref
    void wb();
};

}  // namespace Engine
