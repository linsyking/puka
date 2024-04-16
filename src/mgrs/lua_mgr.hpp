#pragma once

#include <string>
#include "utils/lua_component.hpp"

namespace Engine {

void lua_log(const std::string &message);
void lua_log_error(const std::string &message);

void execute_component_function(LuaComponent *comp, const std::string &name);
void execute_component_function_col(LuaComponent *comp, const std::string &name, Collision &col);

void exit_now();

void sleep(int);

void open_url(const std::string &);

}  // namespace Engine
