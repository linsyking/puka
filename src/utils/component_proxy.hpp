#pragma once

#include <string>
#include <vector>
#include "sol/sol.hpp"
#include "utils/lua_component.hpp"
#include "utils/types.hpp"
namespace Engine {

class ComponentProxy {
    LuaComponent *component;
    lua_ref ref;

public:
    /// Copy a lua object
    lua_ref_raw copy(lua_ref_raw, sol::state &);
    ComponentProxy(LuaComponent *c) : component(c){};

    /// Get the ref
    ///
    /// Will first unlock self VM, and acquire the component VM lock.
    ///
    /// After copying the value, release the component VM lock and lock self.
    lua_ref_raw get();

    /// Write back ref
    void wb();

    static void get_table_keys(sol::table &t, std::vector<std::string> &);
    static void register_usertype(sol::usertype<ComponentProxy> &,
                                  const std::vector<std::string> &);
};

}  // namespace Engine
