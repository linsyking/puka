#pragma once
#include "yyjson.h"
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {
class EngineUtils {
public:
    static yyjson_doc *read_json_from_file(const std::string &path) {
        yyjson_doc *doc = yyjson_read_file(path.c_str(), 0, NULL, NULL);
        if (!doc) {
            std::cout << "error: could not read config file\n";
            exit(0);
        }
        return doc;
    }

    static void ignore_set(std::string &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_str(res);
        }
    }

    static void ignore_set(int &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_int(res);
        }
    }

    static void ignore_set(std::optional<int> &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_int(res);
        }
    }

    static void ignore_set(float &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_real(res);
        }
    }

    static void ignore_set(double &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_real(res);
        }
    }

    static void ignore_set(bool &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_bool(res);
        }
    }

    static void ignore_set(char &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            a = yyjson_get_str(res)[0];
        }
    }

    static void ignore_set(std::vector<std::string> &a, yyjson_val *obj, const char *field) {
        if (yyjson_val *res = yyjson_obj_get(obj, field)) {
            yyjson_val     *val  = nullptr;
            yyjson_arr_iter iter = yyjson_arr_iter_with(res);
            while ((val = yyjson_arr_iter_next(&iter))) {
                a.push_back(yyjson_get_str(val));
            }
        }
    }
};  // namespace EngineUtils

struct jvalue {
    enum class type {
        STRING,
        INT,
        NUMBER,
        BOOL
    };
    std::string string_v;
    bool        bool_v;
    int         int_v;
    float       float_v;
    type        type_v;

    float get_num() {
        if (type_v == type::INT) {
            return int_v;
        } else {
            return float_v;
        }
    }
};

using jmap = std::unordered_map<std::string, jvalue>;

}  // namespace Engine
