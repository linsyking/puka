#pragma once

#include "sol/sol.hpp"
#include "utils/lua_component.hpp"
#include "utils/types.hpp"
namespace Engine {

class ComponentProxy {
    /// Whether the component resides in the current VM
    bool trivial = false;

public:
    LuaComponent *component = nullptr;
    lua_ref       ref;
    /// Copy a lua object
    static lua_ref_raw copy(lua_ref_raw &obj, sol::state &target);
    ComponentProxy(LuaComponent *c, bool trivial = false) : trivial(trivial), component(c){};

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
