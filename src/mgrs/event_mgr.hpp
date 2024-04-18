#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "utils/types.hpp"

namespace Engine {

class EventManager {
public:
    std::unordered_map<std::string, std::vector<std::pair<lua_ref_raw, lua_ref_raw>>> events;
    std::unordered_map<std::string, std::vector<std::pair<lua_ref_raw, lua_ref_raw>>> new_events;
    void                                                                              update();
    std::mutex                                                                        mtx;
    void                                                                              quit();
};

// Lua bindings

void publish_event(const std::string &type, lua_ref_raw obj);
void subscribe_event(const std::string &type, lua_ref_raw obj, lua_ref_raw callback);
void unsubscribe_event(const std::string &type, lua_ref_raw obj, lua_ref_raw callback);

}  // namespace Engine
