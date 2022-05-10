#pragma once

class ICallbacks
{
public:
	virtual void RenderSceneCB() = 0;
	virtual void IdleCB() = 0;
};
