#pragma once
#include <vector>
#include "Core.hpp"
#include "Layer.hpp"

namespace mist {
    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        std::vector<Layer*>::iterator begin() { return layers.begin(); }
        std::vector<Layer*>::iterator end() { return layers.end(); }
    private:
        std::vector<Layer*> layers;
        uint32_t layerInsert = 0;
    };
}