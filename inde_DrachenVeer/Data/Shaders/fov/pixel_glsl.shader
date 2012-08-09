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
uniform float blurriness;

vec4 getColor(sampler2D tex, vec4 coord)
{
  PREC vec4 v[12];
  PREC vec4 color = texture2D(tex, coord.st);
  PREC float dx, dy;
  int index;

  if(color.r == color.b && color.g == 0.0) {
      return color;
  } 

  //Scaled by a factor of 30%.
  dx = 0.020;
  dy = 0.020;

  v[ 0] = vec4(-0.326212 , -0.40581   , 0.0, 0.0);
  v[ 1] = vec4(-0.840144 , -0.07358   , 0.0, 0.0);
  v[ 2] = vec4(-0.695914 , 0.457137   , 0.0, 0.0);
  v[ 3] = vec4(-0.203345 , 0.62716    , 0.0, 0.0);
  v[ 4] = vec4(0.96234   , -0.194983  , 0.0, 0.0); 
  v[ 5] = vec4(0.473434  , -0.480026  , 0.0, 0.0); 
  v[ 6] = vec4(0.519456  , 0.767022   , 0.0, 0.0); 
  v[ 7] = vec4(0.185461  , -0.893124  , 0.0, 0.0); 
  v[ 8] = vec4(0.507431  , 0.064425   , 0.0, 0.0); 
  v[ 9] = vec4(0.89642   , 0.412458   , 0.0, 0.0); 
  v[10] = vec4(-0.32194  , -0.932615  , 0.0, 0.0); 
  v[11] = vec4(-0.791559 , -0.59771   , 0.0, 0.0);

  color = color * (1.0 - blurriness) * 12.0;
  index = int(blurriness*12.0);
  while(index >= 0) {
	PREC vec4 coordRand = vec4(coord.x + (v[index].x * dx), coord.y + (v[index].y * dy), 0.0, 1.0);
        color += texture2D(tex, coordRand.st);
	index = index - 1;
  }
  return color / 12.0;
}

void main(){
  PREC vec4 tmp=varying_color;
  PREC vec4 tmpRand=varying_color;
  PREC vec4 coord = vec4(varying_texcoord, 0.0, 1.0);
  PREC vec4 offsetCoord;
  PREC vec4 coordRand;
  if (use_tex_matrix) {
     PREC vec4 sample;

     coord = coord * tex_matrix;
     sample = getColor(tex, coord);
     tmp *= sample;
  }
  else if (use_tex) {
     PREC vec4 sample;
     PREC vec4 sampleRand;

     sample = getColor(tex, coord);
     tmp *= sample;
  }

  if(tmp.r == tmp.b && tmp.g == 0.0) {
      gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
      return;
  } 

  gl_FragColor=vec4(tmp.r, tmp.g, tmp.b, tmp.a);
}

