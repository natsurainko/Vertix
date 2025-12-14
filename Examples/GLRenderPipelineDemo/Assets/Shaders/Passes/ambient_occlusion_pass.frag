#version 450 core

in vec2 TexCoord;

layout (location = 0) out float fColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gDepth;
layout (binding = 3) uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection;

uniform int kernelSize = 32;
uniform vec3 samples[64];
uniform vec2 noiseScale;

float radius = 0.5;
float bias = 0.025;

void main() {
    vec4 fragPos = texture(gPosition, TexCoord);

    if (fragPos.a == 0.0) {
        fColor = 1.0;
        return;
    }

    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec3 randomVec = normalize(texture(texNoise, TexCoord * noiseScale).xyz);

    vec4 fragPosViewSpace = view * vec4(fragPos.xyz, 1.0);
    vec3 viewNormal = normalize(mat3(view) * normal);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPosViewSpace.xyz + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0)
            continue;

        float sampledDepth = texture(gDepth, offset.xy).r;
        if (sampledDepth >= 1.0) continue;

        // get sample depth
        vec4 samplePosWorld = texture(gPosition, offset.xy);
        vec4 samplePosView = view * vec4(samplePosWorld.xyz, 1.0);
        float sampleDepth = samplePosView.z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosViewSpace.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / float(kernelSize));

    fColor = pow(occlusion, 0.75);
}