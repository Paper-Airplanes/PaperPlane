#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float curTime;

void main() {
	//  L：波长
	float L[] = {0.1, 0.1};
	//  A：振幅
	float A[] = {0.01, 0.002};
	//  S：波移动的速度
	float S[] = {0.3, 0.4};
	//  Q：陡度系数
	float Q[] = {0.5, 0.35};
	//  D：波的传播方向
	vec2 D[] = {vec2(1, 0), vec2(0, 1)};

	float x = aPos.x;
	float y = aPos.y;
	float z = 0;

	float Normalx = 0.0f;
	float Normaly = 0.0f;
	float Normalz = 1.0f;

	for (int i = 0; i < 2; i++) {
	    // calc wave model
		float dotv = dot(vec2(x, y), D[i]);

		float w = 2.0 / L[i];
		float fi = S[i] * w;
		float fixt = fi * curTime;

		z += A[i] * sin(w * dotv + fixt);
		x += Q[i] * A[i] * D[i].x * cos(w * dotv + fixt);
		y += Q[i] * A[i] * D[i].y * cos(w * dotv + fixt);


		// calc wave normal
		vec3 P = vec3(x, y, z);
		vec3 D3 = vec3(D[i], 0.0f);
		float dotDP = dot(D3, P);

		float WA = w * A[i];
		float fS = sin(w * dotDP + fixt);
		float fC = cos(w * dotDP + fixt);

		Normalx -= D[i].x * WA * fC;
		Normaly -= D[i].y * WA * fC;
		Normalz -= Q[i] * WA * fS;

	}

	Normal = vec3(Normalx, Normaly, Normalz);
	Normal = mat3(transpose(inverse(model))) * Normal;
	gl_Position = projection * view * model * vec4(x, y, z, 1.0f);
	FragPos = vec3(model * vec4(x, y, z, 1.0f));
	texCoord = aTexCoord;
}
