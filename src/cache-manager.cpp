#include "cache-manager.h"

CacheManager* CacheManager::_instance = 0;

CacheManager* CacheManager::instance()
{
	if (_instance == 0) {
		_instance = new CacheManager;
	}
	return _instance;
}
