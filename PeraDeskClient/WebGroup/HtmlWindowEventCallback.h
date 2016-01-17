// (c) 2008 Kickstone Technology Ltd

#pragma once

class IHtmlWindowEventCallback {
public:
	virtual void onLoadOccured()=0;
	virtual void onUnloadOccured()=0;
	virtual void releasePointer()=0;
};
