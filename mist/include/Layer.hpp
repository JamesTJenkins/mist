#pragma once
#include "Core.hpp"
#include <SDL.h>

namespace mist {
    class MIST_API Layer {
    public:
        Layer(const std::string& name = "Layer") : name(name) {}
        virtual ~Layer() {};

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender() = 0;
        virtual void OnEvent(const SDL_Event* e) = 0;

        inline const std::string& GetName() const { return name; }
    private:
        std::string name;
    };
}