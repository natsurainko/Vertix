//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_IFILLCONSTANTS_H
#define VERTIX_IFILLCONSTANTS_H

namespace Vertix {
    template<typename TConstants>
    struct IFillConstants {
        virtual void Fill(TConstants& out) const = 0;
        virtual ~IFillConstants() = default;
    };
}

#endif //VERTIX_IFILLCONSTANTS_H