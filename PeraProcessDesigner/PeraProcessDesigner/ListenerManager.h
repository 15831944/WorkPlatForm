#pragma once

template<class T>
class ListenerManager
{
public:
	ListenerManager(void);
	virtual ~ListenerManager(void);

	vector<T*> m_vListeners;

	void AddListener(T* p)
	{
		m_vListeners.push_back(p);
	}
};

template<class T>
ListenerManager<T>::ListenerManager(void)
{
}

template<class T>
ListenerManager<T>::~ListenerManager(void)
{
}