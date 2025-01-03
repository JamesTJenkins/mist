#pragma once
#include "Core.hpp"
#include <string>
#include <SDL2/SDL.h>

namespace mist {
    class MIST_API Layer {
    public:
        Layer(const char* name = "Layer") : name(name) {}
        virtual ~Layer() {};

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender() = 0;
        virtual void OnEvent(const SDL_Event* e) = 0;

        inline const char* GetName() const { return name; }
    private:
        const char* name;
    };
}