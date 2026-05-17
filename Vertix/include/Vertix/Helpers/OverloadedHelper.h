//
// Created by Natsurainko on 2026/5/15.
//

#ifndef VERTIX_OVERLOADEDHELPER_H
#define VERTIX_OVERLOADEDHELPER_H

namespace Vertix {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

#endif //VERTIX_OVERLOADEDHELPER_H
