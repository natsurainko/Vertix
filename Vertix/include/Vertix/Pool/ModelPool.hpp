//
// Created by Natsurainko on 2026/3/22.
//

#ifndef VERTIX_MODELPOOL_HPP
#define VERTIX_MODELPOOL_HPP

#include "Vertix/Pool/DynamicResourcePool.hpp"
#include "Vertix/Primitive/Model.h"

namespace Vertix {
    class ModelPool : public DynamicResourcePool<Model, ModelHandle> {};
}

#endif //VERTIX_MODELPOOL_HPP
