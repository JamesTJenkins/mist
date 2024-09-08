#include "LayerStack.hpp"
#include <algorithm>

namespace mist {
    LayerStack::LayerStack() {}

    LayerStack::~LayerStack() {
        for (Layer* layer : layers)
            delete layer;
    }

    void LayerStack::PushLayer(Layer* layer) {
        layers.emplace(layers.begin() + layerInsert, layer);
        layerInsert++;
    }

    void LayerStack::PopLayer(Layer* layer) {
        auto it = std::find(layers.begin(), layers.end(), layer);
        if (it != layers.end()) {
            layers.erase(it);
            layerInsert--;
        }
    }
}