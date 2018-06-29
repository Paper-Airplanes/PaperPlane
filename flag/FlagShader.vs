#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform float waveSpeed;
uniform float waveT;
uniform float waveAltitude;
uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

void main() {
	float PI = 3.14159f;
	float waveOmega = 2 * PI / waveT;
	float waveZ = waveAltitude * sin(waveOmega * aPos.x + waveSpeed * time);

	vec3 new_aPos = vec3(aPos.x, aPos.y, waveZ);
	gl_Position = proj * view * model * vec4(new_aPos, 1.0f);
	TexCoord = aTexCoord;

	float cutK = waveAltitude * waveOmega * cos(waveOmega * aPos.x + waveSpeed * time);

	if (cutK < 0.0001f && cutK > -0.0001f) {
		Normal = vec3(0.0f, 0.0f, 1.0f);
	} else {
		float norK = -1.0f / cutK;
		if (norK < 0) {
			Normal = vec3(-1.0f, 0.0f, -norK);
		} else {
			Normal = vec3(1.0f, 0.0f, norK);
		}

	}
	Normal = mat3(transpose(inverse(model))) * Normal;
	FragPos = vec3(model * vec4(new_aPos, 1.0f));
}