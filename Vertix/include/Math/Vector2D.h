//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_VECTOR2_H
#define VERTIX_VECTOR2_H

namespace Vertix {
    template <typename T>
    struct Vector2D {
        T X;
        T Y;
        Vector2D() : X(0), Y(0) {}
        Vector2D(T x, T y) : X(x), Y(y) {}

        template <typename TCast>
        Vector2D<TCast> Cast() const {
            return Vector2D<TCast>(static_cast<TCast>(X), static_cast<TCast>(Y));
        }

        bool operator==(const Vector2D & vector2) const {
            return X == vector2.X && Y == vector2.Y;
        }

        Vector2D operator+(const Vector2D & vector2) const {
            return Vector2D(X + vector2.X, Y + vector2.Y);
        }
        Vector2D operator-(const Vector2D & vector2) const {
            return Vector2D(X - vector2.X, Y - vector2.Y);
        }
        Vector2D operator- () const noexcept { return Vector2D(-X, -Y); }
    };
}

#endif //VERTIX_VECTOR2_H

