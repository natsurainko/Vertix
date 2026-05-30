//
// Created by Natsurainko on 2026/3/22.
//

#pragma once

#include "Vertix/Pool/DynamicResourcePool.hpp"
#include "Vertix/Primitive/Model.h"

namespace Vertix {
    class ModelPool : public DynamicResourcePool<Model, ModelHandle> {};
}
