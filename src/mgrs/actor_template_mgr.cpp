#include "actor_template_mgr.hpp"
#include "actor.hpp"
#include <filesystem>
#include <iostream>
#include "consts.hpp"
#include "utils/json.hpp"
#include "yyjson/yyjson.h"

namespace Engine {

// Actor Template Manager
Actor ActorTemplateManager::get_actor_template(const std::string &name) {
    if (actor_templates.find(name) != actor_templates.end()) {
        return actor_templates[name];
    } else {
        // Load the actor from the file
        std::string file_path = actor_templates_folder + "/" + name + ".template";
        if (!std::filesystem::exists(file_path)) {
            std::cout << "error: template " << name << " is missing";
            exit(0);
        }
        yyjson_doc *doc = EngineUtils::read_json_from_file(file_path);
        Actor       actor;
        actor.load_from_value_pure(yyjson_doc_get_root(doc));
        actor_templates[name] = actor;
        yyjson_doc_free(doc);
        return actor;
    }
}
}  // namespace Engine
