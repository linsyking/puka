#include "lua_mgr.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include "game.hpp"
#include "mgrs/box2d_mgr.hpp"
#include "utils/dbg.hpp"
#include "utils/lua_component.hpp"

namespace Engine {

void exit_now() {
    game().terminate();
}

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void open_url(const std::string &url) {
#ifdef _WIN32
    std::string command = "start " + url;
#else
#ifdef __APPLE__
    std::string command = "open " + url;
#else
    std::string command = "xdg-open " + url;
#endif
#endif
    system(command.c_str());
}

void lua_log(const std::string &message) {
    std::unique_lock<std::mutex> lock(cout_mtx);
    std::cout << message << "\n";
}

void lua_log_error(const std::string &message) {
    std::unique_lock<std::mutex> lock(cout_mtx);
    std::cerr << message << "\n";
}

void report_err(const std::string &actor, const std::runtime_error &e) {
    std::string err = e.what();
    std::replace(err.begin(), err.end(), '\\', '/');
    std::cout << "\033[31m" << actor << " : " << err << "\033[0m\n";
}

void execute_component_function(LuaComponent *comp, const std::string &name) {
    try {
        comp->ref_tbl.value()[name](comp->ref_tbl.value());
    } catch (std::runtime_error &e) {
        report_err(comp->actor_name, e);
    }
}

void execute_component_function_col(LuaComponent *comp, const std::string &name, Collision &col) {
    try {
        comp->ref_tbl.value()[name](comp->ref_tbl.value(), col);
    } catch (std::runtime_error &e) {
        report_err(comp->actor_name, e);
    }
}

}  // namespace Engine
