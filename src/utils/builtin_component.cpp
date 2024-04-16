#include "builtin_component.hpp"
#include <mutex>
#include "mgrs/task_mgr.hpp"
namespace Engine {

BuiltinComponent::BuiltinComponent() {
    onstart_task     = std::make_shared<builtin_onstart_task>(this);
    update_task      = std::make_shared<builtin_update_task>(this);
    late_update_task = std::make_shared<builtin_late_update_task>(this);
}

bool BuiltinComponent::is_enabled() {
    std::unique_lock<std::mutex> _(mtx);
    return enabled;
};
void BuiltinComponent::set_enabled(bool enabled) {
    std::unique_lock<std::mutex> _(mtx);
    this->enabled = enabled;
};

}  // namespace Engine
