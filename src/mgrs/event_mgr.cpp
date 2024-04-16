#include "event_mgr.hpp"
#include <mutex>
#include <string>
#include "game.hpp"
#include "utils/types.hpp"
namespace Engine {

void publish_event(const std::string &type, lua_ref_raw obj) {
    std::unique_lock<std::mutex> lock(game().get_event_manager().mtx);
    auto                        &events = game().get_event_manager().events;
    if (events.find(type) != events.end()) {
        for (auto &event : events[type]) {
            event.second.as<sol::function>()(event.first, obj);
        }
    }
}

void subscribe_event(const std::string &type, lua_ref_raw obj, lua_ref_raw callback) {
    std::unique_lock<std::mutex> lock(game().get_event_manager().mtx);
    auto                        &events = game().get_event_manager().new_events;
    events[type].push_back(std::make_pair(obj, callback));
}

void unsubscribe_event(const std::string &type, lua_ref_raw obj, lua_ref_raw callback) {
    std::unique_lock<std::mutex> lock(game().get_event_manager().mtx);
    auto                        &events = game().get_event_manager().new_events;
    if (events.find(type) != events.end()) {
        auto &event = events[type];
        for (auto it = event.begin(); it != event.end(); ++it) {
            if (it->first == obj && it->second == callback) {
                event.erase(it);
                return;
            }
        }
    }
}

void EventManager::update() {
    events = new_events;
}

}  // namespace Engine
