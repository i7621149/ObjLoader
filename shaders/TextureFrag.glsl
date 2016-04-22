#version 410 core

layout(location = 0) out vec4 outColor;

in vec3 fragNormal;
in vec2 fragUV;

uniform sampler2D tex;
uniform int mode;

void main()
{
  switch(mode){
    case 0 :
    outColor = texture(tex, fragUV);
    break;
    case 1 :
      outColor = vec4(fragNormal, 1.0);
    break;
  }
  outColor = vec4(fragNormal, 1.0);

}
