//
// Created by Natsurainko on 2026/5/9.
//

#ifndef VERTIX_IPASSBINDING_H
#define VERTIX_IPASSBINDING_H

namespace Vertix {
    struct IPassBinding {
        virtual void Target(void* pass) = 0;
        virtual void Inject(const void* resource) = 0;
        virtual ~IPassBinding() = default;
    };
}

#endif //VERTIX_IPASSBINDING_H
