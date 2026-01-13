#include "ObjectHelper.hpp"

using namespace obj_helper;

static ObjectHelper* s_shared_helper = nullptr;

ObjectHelper const* obj_helper::get_shared_helper() {
    if (!s_shared_helper) {
        s_shared_helper = new ObjectHelper();
    }
    return s_shared_helper;
}
