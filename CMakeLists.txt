cmake_minimum_required(VERSION 3.20)
project(puka)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -Wall -Wextra -pedantic -O3")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BACKWARDCPP dev_ext/backwardcpp/backward.cpp)
elseif()
    set(BACKWARDCPP)
endif()
add_executable(puka
    src/game.cpp
    src/main.cpp
    src/chore.cpp
    src/render.cpp
    src/config.cpp
    src/utils/camera.cpp
    src/utils/vector2.cpp
    src/utils/component.cpp
    src/utils/lua_component.cpp
    src/utils/builtin_component.cpp
    src/utils/component_proxy.cpp
    src/mgrs/actor.cpp
    src/mgrs/scene_mgr.cpp
    src/mgrs/actor_template_mgr.cpp
    src/mgrs/image_mgr.cpp
    src/mgrs/text_mgr.cpp
    src/mgrs/audio_mgr.cpp
    src/mgrs/input_mgr.cpp
    src/mgrs/component_mgr.cpp
    src/mgrs/box2d_mgr.cpp
    src/mgrs/event_mgr.cpp
    src/mgrs/task_mgr.cpp
    src/mgrs/lua_mgr.cpp
    src/mgrs/lua_runner.cpp
    src/components/rigidbody.cpp
    src/scenes/main_scene.cpp
    src/scenes/end_scene.cpp
    ${BACKWARDCPP}
)

include_directories("ext/include")
include_directories("ext/include/sol")
include_directories("ext/include/lua")
include_directories("ext/include/task_runner")
include_directories("ext/include/yyjson")
include_directories("src")

add_subdirectory("ext")

target_link_libraries(puka debug SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer bfd clua box2d trunner yyjson)

target_link_libraries(puka optimized SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer clua box2d trunner yyjson)
