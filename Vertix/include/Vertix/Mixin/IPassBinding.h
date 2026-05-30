//
// Created by Natsurainko on 2026/5/9.
//

#pragma once

namespace Vertix {
    struct IPassBinding {
        virtual void Target(void* pass) = 0;
        virtual void InjectValue(const void* resource) = 0;
        virtual      ~IPassBinding() = default;
    };
}
