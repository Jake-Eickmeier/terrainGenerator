#version 400
/*
*  Simple vertex shader for directional light
*/

in vec4 vPosition;
in vec3 vNormal;
uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;

out float height;
out vec3 normal;
out vec4 position;

void main() {
	height = vPosition.z;
	gl_Position = projection * modelView * vPosition;
	position = vPosition;
	normal = vNormal;

}