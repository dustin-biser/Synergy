// ComputeNormalAmbo.vs
#version 410

// 3D texture coordinates
in vec3 uvw;

uniform sampler3D densityTexture;

//-- Inverse dimensions of densityTexture:
uniform float inv_densityWidth;
uniform float inv_densityHeight;
uniform float inv_densityDepth;

// Surface normal and ambient occlusion factor.
// Pack data into vec4 as follows:
// RGB -> normal.xyz
// Alpha -> ambient occlusion factor
out vec4 normalAmbo;

uniform vec3 normalAmboDim;

void main() {

	vec4 step = vec4(inv_densityWidth, inv_densityHeight, inv_densityDepth, 0.0);

	// Covert uvw from normalAmboTexture space to densityTexture space.
	// DensityTexture should be at least 2 units larger in each dimension than
	// normalAmboTexture.
	vec3 texCoord;
	texCoord.x = uvw.x * (normalAmboDim.x * inv_densityWidth) + inv_densityWidth;
	texCoord.y = uvw.y * (normalAmboDim.y * inv_densityHeight) + inv_densityHeight;
	texCoord.z = uvw.z * (normalAmboDim.z * inv_densityDepth) + inv_densityDepth;

	//-- Gradient in world-space:
	vec3 gradient;
	gradient.x = (texture(densityTexture, texCoord + step.xww).r -
				 texture(densityTexture, texCoord - step.xww).r);

	gradient.y = texture(densityTexture, texCoord + step.wwz).r -
				 texture(densityTexture, texCoord - step.wwz).r;

	gradient.z = texture(densityTexture, texCoord - step.wyw).r -
				 texture(densityTexture, texCoord + step.wyw).r;

	normalAmbo.rgb = normalize(-gradient);


	// TODO Dustin compute ambient occlusion factor and pack into alpha component
	// of normalAmbo output.
	normalAmbo.a = -99;
}

