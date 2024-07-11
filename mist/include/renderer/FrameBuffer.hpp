#pragma once
#include "Core.hpp"
#include <vector>

namespace mist {
    enum class FrameBufferTextureFormat {
        None = 0,
        // Color
        RGBA8,
        RINT,
        // Depth/Stencil
        DEPTH24STENCIL8
    };

    struct MIST_API FrameBufferTextureProperties {
        FrameBufferTextureProperties() = default;
        FrameBufferTextureProperties(FrameBufferTextureFormat format) : textureFormat(format) {}

        FrameBufferTextureFormat textureFormat = FrameBufferTextureFormat::None;
    };

    struct MIST_API FrameBufferAttachmentProperties {
		FrameBufferAttachmentProperties() = default;
		FrameBufferAttachmentProperties(std::initializer_list<FrameBufferTextureProperties> attachments) : attachments(attachments) {}

		std::vector<FrameBufferTextureProperties> attachments;
	};

    struct MIST_API FrameBufferProperties {
        uint32_t width, height;
        FrameBufferAttachmentProperties attachment;
        uint32_t samples = 1;
    };

    class MIST_API FrameBuffer {
    public:
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const = 0;
        virtual const FrameBufferProperties& GetProperties() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferProperties& properties, const uint32_t swapchainInstance = 0);
        static bool IsDepthFormat(FrameBufferTextureFormat format);
    };
}