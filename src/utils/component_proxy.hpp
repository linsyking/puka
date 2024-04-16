#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "sol/sol.hpp"
#include "utils/lua_component.hpp"
namespace Engine {

struct ProxyData;
using table_key_t = std::variant<std::string, int>;

class ComponentProxy {
    LuaComponent *component;

public:
    /// Copy a lua object to proxydata
    void        load_to_proxy(lua_ref_raw, ProxyData &);
    lua_ref_raw save_proxy_data(sol::state &, ProxyData &);
    ComponentProxy(LuaComponent *c) : component(c){};
    sol::object getter(const std::string &);
    void        setter(const std::string &, sol::object);
    static void get_table_keys(sol::table &t, std::vector<std::string> &);
    static void register_usertype(sol::usertype<ComponentProxy> &,
                                  const std::vector<std::string> &);
};

struct ProxyData {
    std::variant<int, double, bool, std::string, ComponentProxy,
                 std::unordered_map<table_key_t, ProxyData>>
        data;
};

}  // namespace Engine
