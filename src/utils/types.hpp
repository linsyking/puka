#pragma once

#include <optional>
#include "sol.hpp"
#include "glm/glm.hpp"

namespace Engine {
using lua_ref     = std::optional<sol::object>;
using lua_ref_raw = sol::object;
using vec2        = glm::vec2;
}  // namespace Engine
