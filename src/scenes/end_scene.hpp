#pragma once
#include "general_scene.hpp"
namespace Engine {

class EndScene : public GeneralScene {
public:
    std::shared_ptr<GeneralScene> pre_check() override;
    void                          init() override;
};

}  // namespace Engine
