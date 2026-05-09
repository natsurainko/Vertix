//
// Created by Natsurainko on 2026/5/9.
//

#ifndef VERTIX_IPASSBINDING_H
#define VERTIX_IPASSBINDING_H

namespace Vertix {
    struct IPassBinding {
        virtual void Inject(void* pass, void* resource) = 0;
        virtual ~IPassBinding() = default;
    };
}

#endif //VERTIX_IPASSBINDING_H
