#pragma once

#include "utils/lua_component.hpp"
namespace Engine {
class ComponentProxy {
    LuaComponent *component;

public:
    /// Copy a lua object to another state
    lua_ref_raw copy(lua_ref_raw, sol::state &);
    ComponentProxy(LuaComponent *c) : component(c){};
    sol::object getter(const char *);
    void        setter(const char *, sol::object);
};

}  // namespace Engine
