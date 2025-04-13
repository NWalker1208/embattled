#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform int renderWidth;
uniform int renderHeight;
uniform float blurSize;

void main() {
  vec4 weightedAverage = vec4(0,0,0,0);
  int intBlurSize = int(ceil(abs(blurSize)));
  float weight = 1.0 / (intBlurSize * 2 + 1);
  float totalWeight = 0.0;

  for (int offset = -intBlurSize; offset <= intBlurSize; offset++) {
    weightedAverage += weight * texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y + float(offset)/renderHeight)); 
    totalWeight += weight;
  }

  finalColor = (weightedAverage / totalWeight) * colDiffuse * fragColor;
}
