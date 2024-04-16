#include "config.hpp"
#include <iostream>
#include <filesystem>
#include "consts.hpp"
#include "utils/dbg.hpp"
#include "yyjson.h"
#include "utils/json.hpp"

namespace Engine {
Config load_config() {
    Config      c;
    yyjson_doc *doc  = EngineUtils::read_json_from_file(conf_game);
    yyjson_val *root = yyjson_doc_get_root(doc);
    EngineUtils::ignore_set(c.game_title, root, "game_title");
    if (yyjson_val *res = yyjson_obj_get(root, "initial_scene")) {
        c.initial_scene = yyjson_get_str(res);
    } else {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
    EngineUtils::ignore_set(c.num_threads, root, "num_threads");
    if (c.num_threads <= 0) {
        std::cout << "error: num_threads must be greater than 0";
        exit(0);
    }
    DBGOUT("using " << c.num_threads << " threads\n");
    // Load rendering config
    if (std::filesystem::exists(conf_rendering)) {
        yyjson_doc *rdoc  = EngineUtils::read_json_from_file(conf_rendering);
        yyjson_val *rroot = yyjson_doc_get_root(rdoc);
        EngineUtils::ignore_set(c.x_resolution, rroot, "x_resolution");
        EngineUtils::ignore_set(c.y_resolution, rroot, "y_resolution");
        EngineUtils::ignore_set(c.clear_color.r, rroot, "clear_color_r");
        EngineUtils::ignore_set(c.clear_color.g, rroot, "clear_color_g");
        EngineUtils::ignore_set(c.clear_color.b, rroot, "clear_color_b");
        EngineUtils::ignore_set(c.zoom_factor, rroot, "zoom_factor");
        EngineUtils::ignore_set(c.cam_ease_factor, rroot, "cam_ease_factor");
        yyjson_doc_free(rdoc);
    }
    yyjson_doc_free(doc);
    return c;
}
}  // namespace Engine
