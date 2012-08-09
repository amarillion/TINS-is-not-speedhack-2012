//Depth of Field: http://www.shaderx2.com
#ifndef PREC
#define PREC
#endif //PREC

uniform bool use_tex;
uniform sampler2D tex;
uniform bool use_tex_matrix;
uniform PREC mat4 tex_matrix;
varying PREC vec4 varying_color;
varying PREC vec2 varying_texcoord;
uniform float tint;

void main(){
  PREC vec4 tmp=varying_color;
  PREC vec4 coord = vec4(varying_texcoord, 0.0, 1.0);
  PREC float color;
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

  if(tmp.r == tmp.b && tmp.g == 0.0) {
      gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
      return;
  } 
 
  color = 0.35*(1.0 - tint);

  tmp.r = color;
  tmp.g = color;
  tmp.b = color;

  gl_FragColor=vec4(tmp.r, tmp.g, tmp.b, tmp.a);
}

