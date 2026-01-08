#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/cocos/support/zip_support/ZipUtils.h>

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}
		return true;
	}
};
