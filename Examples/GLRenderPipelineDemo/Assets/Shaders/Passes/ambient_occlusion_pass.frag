#version 450 core

in vec2 TexCoord;

layout (location = 0) out float fColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection;

uniform int kernelSize = 32;
uniform vec3 samples[64];
uniform vec2 noiseScale;

float radius = 2.5;
float bias = 0.025;

void main() {
    vec4 fragPos = texture(gPosition, TexCoord);
    if (fragPos.a < 0) return;
    vec3 fragPosWorld = fragPos.rgb;
    float linearDepth = fragPos.a;

    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec3 randomVec = normalize(texture(texNoise, TexCoord * noiseScale).xyz);

    vec3 fragPosView = (view * vec4(fragPosWorld.xyz, 1.0)).xyz;
    vec3 normalView = normalize(mat3(view) * normal);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normalView * dot(randomVec, normalView));
    vec3 bitangent = cross(normalView, tangent);
    mat3 TBN = mat3(tangent, bitangent, normalView);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = fragPosView + TBN * samples[i] * radius; // from tangent to view-space
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = projection * vec4(samplePos, 1.0);// from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        if (any(lessThan(offset.xy, vec2(0.0))) || any(greaterThan(offset.xy, vec2(1.0)))) 
            continue;

        // get sample depth
        float neighborLinearDepth = texture(gPosition, offset.xy).a;
        if (neighborLinearDepth < 0) continue;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(linearDepth - neighborLinearDepth));
        if (-samplePos.z >= neighborLinearDepth + bias)
            occlusion += rangeCheck;
    }
    occlusion = 1.0 - (occlusion / float(kernelSize));

    fColor = pow(occlusion, 0.35);
}