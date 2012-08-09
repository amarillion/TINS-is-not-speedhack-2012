//From Filmic Tonemapping, Uncharted: http://filmicgames.com/archives/75 
#ifndef PREC
#define PREC
#endif //PREC

uniform bool use_tex;
uniform sampler2D tex;
uniform bool use_tex_matrix;
uniform PREC mat4 tex_matrix;
varying PREC vec4 varying_color;
varying PREC vec2 varying_texcoord;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;
float exposurebias = 2.0;
float multiplier = 16.0;
float power = 0.454;

vec3 tonemap(vec3 x)
{
return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 process(vec3 tmp)
{
  PREC vec3 newcur;
  newcur = tonemap(exposurebias*multiplier*tmp)/tonemap(vec3(W,W,W));
  return vec3(pow(newcur.r, power), pow(newcur.g, power), pow(newcur.b, power));
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
