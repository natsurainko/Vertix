//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_VECTOR2_H
#define VERTIX_VECTOR2_H

#define VERTIX_VECTOR2D_EXPAND(value) (value).X, (value).Y

namespace Vertix {
    template <typename T>
    struct Vector2D {
        T X;
        T Y;
        constexpr Vector2D() noexcept : X(0), Y(0) {}
        constexpr Vector2D(T x, T y) noexcept : X(x), Y(y) {}

        template <typename TCast>
        Vector2D<TCast> Cast() const {
            return Vector2D<TCast>(static_cast<TCast>(X), static_cast<TCast>(Y));
        }

        bool operator==(const Vector2D & vector2) const noexcept {
            return X == vector2.X && Y == vector2.Y;
        }

        Vector2D operator+(const Vector2D & vector2) const noexcept {
            return Vector2D(X + vector2.X, Y + vector2.Y);
        }
        Vector2D operator-(const Vector2D & vector2) const noexcept {
            return Vector2D(X - vector2.X, Y - vector2.Y);
        }
        Vector2D operator*(const T & number) const noexcept {
            return Vector2D(X * number, Y * number);
        }
        Vector2D operator- () const noexcept { return Vector2D(-X, -Y); }

        static const Vector2D Zero;
    };

    template <typename T>
    constexpr Vector2D<T> Vector2D<T>::Zero{ T(0), T(0) };
}

#endif //VERTIX_VECTOR2_H
