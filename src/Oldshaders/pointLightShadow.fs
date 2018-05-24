#version 330 core
in vec4 FragPos;

in vec2 texCoord;

uniform vec3 lightPos;
uniform float farPlane;
uniform sampler2D diffTex;

void main()
{
    if(texture(diffTex, texCoord).a < 0.1)
        discard;

    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / farPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
