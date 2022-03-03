#version 400

in vec4 vPosition;
in float height;

float seaLevel = 1.35f;
float dirtLevel = 1.60f;
float grassLevel = 2.25f;
float mountainLevel = 2.75f;
float mountainPeak = 3.25f;

vec4 BLUE = vec4(0.0f, 0.0f, 1.0f, 1.0f);
vec4 BROWN = vec4(0.6f, 0.4f, 0.2f, 1.0f);
vec4 GREEN = vec4(0.0f, 0.6f, 0.0f, 1.0f);
vec4 GREY = vec4(0.7f, 0.7f, 0.6f, 1.0f);
vec4 OFFWHITE = vec4(0.9f, 0.9f, 0.9f, 1.0f);

in vec3 normal;
in vec4 position;
out vec4 colour;

void main(void) {
	vec3 N;
	vec3 Lposition = vec3(500.0, 500.0, 800.0);	// Light position
	vec4 Mcolour;
	
	if (height <= seaLevel) {
		Mcolour = BLUE;
	}
	else if (seaLevel < height && height <= dirtLevel) {
		Mcolour = BROWN;
	}
	else if (dirtLevel < height && height <= grassLevel) {
		if ((height - dirtLevel)/(grassLevel - dirtLevel) < 0.25) {
			Mcolour = (BROWN + GREEN)/2;
		}
		else {
			Mcolour = GREEN;
		}
	}
	else if (grassLevel < height && height <= mountainLevel) {
		if ((height - grassLevel)/(mountainLevel - grassLevel) < 0.25) {
			Mcolour = (GREY + GREEN) / 2;
		}
		else {
			Mcolour = GREY;
		}
	}
	else {
		Mcolour = OFFWHITE;
	}
	
	vec4 Lcolour = vec4(0.9, 0.9, 0.9, 1.0);	// Light colour
	
	vec3 H;
	float diffuse;
	float specular;
	float n = 500.0;
	vec3 L;
	N = normalize(normal);
	L = normalize(Lposition -position.xyz);
	H = normalize(L + vec3(0.0, 0.0, 1.0));
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),n);
	}
	colour = min(0.3*Mcolour + diffuse*Mcolour*Lcolour + Lcolour*specular, vec4(1.0));
	colour.a = Mcolour.a;
}



	

