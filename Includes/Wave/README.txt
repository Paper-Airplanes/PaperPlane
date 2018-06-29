//  使用 Wave.h

Wave(float waveWidth, float waveHeight, 
		glm::mat4 model, glm::mat4 view, glm::mat4 projection, 
		glm::vec3 _lightPos, glm::vec3 _viewPos,
		string texturePath);

//  参数
//  waveWidth
//	waveHeight：水波宽高，0-1
//  model
//  view
//  projection
//  lightPos 光源位置
//  viewPos 视角
//  texturePath 贴图位置


//  Note: 使用前需要修改Wave.h代码中着色器文件的路径
