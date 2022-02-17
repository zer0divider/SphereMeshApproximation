#include "DiffuseShader.h"

static const char * VERTEX_SOURCE =
"#version 120\n"
"attribute vec3 vertex;\n"
"attribute vec4 color;\n"
"attribute vec3 normal;\n"
"varying vec4 out_color;\n"
"varying vec3 out_normal;\n"
"varying vec3 out_screen_normal;\n"
"uniform mat4 projMat;\n"
"uniform mat4 viewMat;\n"
"uniform mat4 modelMat;\n"
"uniform float normal_offset = 0.0;\n"
"void main()\n"
"{\n"
"	gl_Position = projMat * viewMat * modelMat * vec4((vertex+normal*normal_offset), 1);\n"
"	out_color = color;\n"
"	out_normal = (modelMat*vec4(normal,0)).xyz;\n"
"   out_screen_normal = (viewMat*vec4(out_normal,0)).xyz;\n"
"}\n"
;

static const char * FRAGMENT_SOURCE =
"#version 120\n"
"varying vec4 out_color;\n"
"varying vec3 out_normal;\n"
"varying vec3 out_screen_normal;\n"
"uniform vec3 ambient_light = vec3(0.3,0.3,0.3);\n"
"uniform vec3 light_dir = vec3(1,1,1);\n"
"uniform vec3 light_color = vec3(1,1,1);\n"
"uniform int light_enabled = 1;\n"
"float dot(vec3 a, vec3 b){return a.x*b.x + a.y*b.y + a.z*b.z;}\n"
"void main()\n"
"{\n"
"   if(light_enabled == 1){\n"
"       vec3 l = max(0,dot(light_dir,normalize(out_normal)))*light_color;\n"
"       gl_FragColor = vec4((ambient_light + l)*out_color.rgb, out_color.a);\n"
"   }else if(light_enabled == 2){\n"
"		float f = normalize(out_screen_normal).z;\n"
"       if(f < 0){f = -f;}\n"
"       gl_FragColor = vec4(out_color.rgb*f, out_color.a);\n"
"   }\n"
"   else{\n"
"       gl_FragColor = out_color;\n"
"   }\n"
"}\n"
;

bool DiffuseShader::init()
{
	bool r = load(VERTEX_SOURCE, FRAGMENT_SOURCE);
	if(!r){
		return false;
	}

	setVertexLocation("vertex");
	setColorLocation("color");
	setNormalLocation("normal");
	setModelMatrixLocation("modelMat");
	setViewMatrixLocation("viewMat");
	setProjectionMatrixLocation("projMat");
	setColor(zer0::Color::WHITE);

	_lightDirLoc = getLocation("light_dir", true);
	_ambientColorLoc = getLocation("ambient_light", true);
	_lightColorLoc = getLocation("light_color", true);
	_lightEnabledLoc = getLocation("light_enabled", true);
	_normalOffsetLoc = getLocation("normal_offset", true);

	return r;
}
