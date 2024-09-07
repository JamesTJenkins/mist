#pragma once
#include "Core.hpp"
#include <vector>

namespace mist {
    enum class FrameBufferTextureFormat {
        None = 0,
        // Color formats
        RGBA8,
        BGRA8,
        RGB8,
        BGR8,
        RGBA16F,
        RGBA32F,
        RGB565,
        RGBA4,
        RG8,
        RG16F,
        R32F,
        R11F_G11F_B10F,
        RGB9_E5,
        R8,
        SR8,
        SRGB8_ALPHA8,
        SBGRA8,
        RGB10_A2,
        R16,
        // Compressed color formats
        BC1_RGB,
        BC1_RGBA,
        BC2,
        BC3,
        BC4,
        BC5,
        BC6H,
        BC7,
        ETC2_RGB,
        ETC2_RGBA1,
        ETC2_RGBA8,
        EAC_R11,
        EAC_RG11,
        ASTC_4x4,
        ASTC_5x4,
        ASTC_5x5,
        ASTC_6x5,
        ASTC_6x6,
        ASTC_8x5,
        ASTC_8x6,
        ASTC_8x8,
        ASTC_10x5,
        ASTC_10x6,
        ASTC_10x8,
        ASTC_10x10,
        ASTC_12x10,
        ASTC_12x12,
        // Depth/Stencil formats
        DEPTH16,
        DEPTH24X8,
        DEPTH32,
        DEPTH16_STENCIL8,
        DEPTH24_STENCIL8,
        DEPTH32_STENCIL8,
        STENCIL8
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
        uint32_t width = 1, height = 1;
        FrameBufferAttachmentProperties attachment;
        uint32_t samples = 1;
    };

    class MIST_API FrameBuffer {
    public:
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const = 0;
        virtual const FrameBufferProperties& GetProperties() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferProperties& properties, const uint32_t swapchainInstance = 0);
    };
}