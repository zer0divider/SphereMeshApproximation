#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include "zer0engine/zer0engine.h"

class ModelViewer : public zer0::Application{
public:
	ModelViewer();
	~ModelViewer();
	bool update()override;
	void render()override;
private:
	zer0::Mesh _box;
};

#endif
