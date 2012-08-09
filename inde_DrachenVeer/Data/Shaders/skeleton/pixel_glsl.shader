//Convert to Skeletal Chalk Drawing Based on Threshold.
#ifndef PREC
#define PREC
#endif //PREC

uniform bool use_tex;
uniform sampler2D tex;
uniform bool use_tex_matrix;
uniform PREC mat4 tex_matrix;
varying PREC vec4 varying_color;
varying PREC vec2 varying_texcoord;

vec3 process(vec3 tmp)
{
  float x = tmp.r*0.13 + tmp.g*0.57 + tmp.b*0.3;
  if(x>0.01) {
  	x = 1.0-x;
  }
  return vec3(x*0.581, x*0.902, x);
}

void main(){
  PREC vec4 tmp=varying_color;
  PREC vec4 coord = vec4(varying_texcoord, 0.0, 1.0);
  if (use_tex_matrix) {
     PREC vec4 sample;
     coord = coord * tex_matrix;
     sample = texture2D(tex, coord.st);
     tmp *= sample;
  }
  else if (use_tex) {
     PREC vec4 sample;
     sample = texture2D(tex, coord.st);
     tmp *= sample;
  }
  gl_FragColor=vec4(process(vec3(tmp.r, tmp.g, tmp.b)), tmp.a);
}

