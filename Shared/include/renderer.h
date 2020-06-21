#pragma once

#include "symbolart.h"

class SarRenderer {
public:
	virtual ~SarRenderer() = 0;
	virtual void SetFlag(int flag, bool value) = 0;
	virtual void Render(SarFile& sar, int texW, int texH, void* rgbaData) = 0;
};