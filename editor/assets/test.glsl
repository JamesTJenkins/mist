#type vertex
#version 460 core

layout(location = 0) in vec3 Position;

layout(set = 0, binding = 0) uniform TransformBlock {
	uniform mat4 u_ViewProjection;
	uniform mat4 u_Transform;
} transforms;

void main() {
	gl_Position = transforms.u_ViewProjection * transforms.u_Transform * vec4(Position, 1);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

void main() {
	color = vec4(1.0, 1.0, 1.0, 1.0);
}