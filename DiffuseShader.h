#ifndef DIFFUSE_SHADER_H
#define DIFFUSE_SHADER_H

#include "zer0engine/zShader.h"

class DiffuseShader : public zer0::Shader
{ 
public:
	DiffuseShader():
		Shader("Diffuse-Shader"),
		_lightDirLoc(-1),
		_ambientColorLoc(-1),
		_lightColorLoc(-1){}

	bool init();

	/* set ambient light color */
	void setAmbientColor(const zer0::Color & c){
		glUniform3fv(_ambientColorLoc, 1, (float*)&c);	
	}

	/* set color of the directional light */
	void setLightColor(const zer0::Color & c){
		glUniform3fv(_lightColorLoc, 1, (float*)&c);	
	}

	/* set light direction */
	void setLightDir(const zer0::Vector3D & dir){
		glUniform3fv(_lightDirLoc, 1, (float*)&dir);	
	}

	/* enabled/disable lighting */
	enum LightMode{UNSHADED=0, STATIC_LIGHT=1, MATCAP=2};
	void setLightMode(LightMode mode){
		glUniform1i(_lightEnabledLoc, mode);
	}

	void setNormalOffset(float offset){
		glUniform1f(_normalOffsetLoc, offset);
	}

private:
	GLint _lightDirLoc;
	GLint _ambientColorLoc;
	GLint _lightColorLoc;
	GLint _lightEnabledLoc;
	GLint _normalOffsetLoc;
};

#endif
