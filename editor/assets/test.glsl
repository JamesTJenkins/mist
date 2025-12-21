#type vertex
#version 460 core

layout(location = 0) in vec3 Position;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform PushConstants {
	mat4 ModelMatrix;
} constants;

layout(set = 0, binding = 0) uniform CameraData {
	uniform mat4 u_ViewProjectionMatrix;
} cameraData;

void main() {
	gl_Position = cameraData.u_ViewProjectionMatrix * constants.ModelMatrix * vec4(Position, 1);
	fragColor = Position;
}

#type fragment
#version 460 core

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 color;

void main() {
	color = vec4(fragColor, 1.0);
}