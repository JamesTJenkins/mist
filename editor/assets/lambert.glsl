#type vertex
#version 460 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;

layout(push_constant) uniform PushConstants {
	mat4 ModelMatrix;
} constants;

layout(set = 0, binding = 0) uniform CameraData {
	uniform mat4 u_ViewProjectionMatrix;
} cameraData;

void main() {
	gl_Position = cameraData.u_ViewProjectionMatrix * constants.ModelMatrix * vec4(Position, 1);
	fragPosition = vec3(constants.ModelMatrix * vec4(Position, 1.0));
    fragNormal = mat3(transpose(inverse(constants.ModelMatrix))) * Normal;
}

#type fragment
#version 460 core

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 1) uniform DirectionalLightData {
	vec3 u_LightDir;
	vec3 u_LightColor;
} directionalLightData;

void main() {
    float diff = max(dot(normalize(fragNormal), normalize(directionalLightData.u_LightDir)), 0.0);
    vec3 diffuse = diff * directionalLightData.u_LightColor;
    color = vec4((diffuse * vec3(1.0)), 1.0);
}