#pragma once

#include <memory>
namespace Engine {
class GeneralScene {
public:
    /// Return false if the scene should not run
    virtual std::shared_ptr<GeneralScene> pre_check() { return nullptr; }

    virtual void                          init(){};
    virtual std::shared_ptr<GeneralScene> update() { return nullptr; };
    virtual void                          post_quit(){};
    virtual ~GeneralScene(){};
};
}  // namespace Engine
