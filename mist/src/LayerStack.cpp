#include "LayerStack.hpp"

namespace mist {
    LayerStack::LayerStack() {}

    LayerStack::~LayerStack() {}

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