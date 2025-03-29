#include "LayerStack.hpp"
#include <algorithm>

namespace mist {
	LayerStack::LayerStack() : layers(nullptr), capacity(0), size(0) {}

	LayerStack::~LayerStack() {
		Clear();
	}

	void LayerStack::PushLayer(Layer* layer) {
		if (size == capacity) {
			Resize(capacity == 0 ? 1 : capacity * 2);
		}
		layers[size++] = layer;
	}

	void LayerStack::PopLayer(Layer* layer) {
		auto it = std::find(begin(), end(), layer);
		if (it != end()) {
			std::rotate(it, it + 1, end());
			--size;
		}
	}

	void LayerStack::Clear() {
		delete[] layers;
		size = 0;
		capacity = 0;
	}

	void LayerStack::Resize(uint32_t newCapacity) {
		Layer** newLayers = new Layer*[newCapacity];
		std::copy(begin(), end(), newLayers);
		delete[] layers;
		layers = newLayers;
		capacity = newCapacity;
	}
}