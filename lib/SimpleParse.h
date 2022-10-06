#pragma once

#include <functional>

template <typename T>
struct M {
    bool success;
    T value;

    static New(T otherValue) {
        return M<T> {
            value;
        }
    }

    template <typename S, typename R>
    static Bind(
        std::function<M<S>(M<T>)> first,
        std::function<M<R>(M<S>)> secnd
    ) {
        return [=](M<T> t) -> M<R> {
            return secnd(first(t));
        }
    }

    template <typename S, typename R>
    static And(
        std::function<M<S>(M<T>)> first,
        std::function<M<R>(M<S>)> secnd
    ) {
        return [=](M<T> t) -> M<R> {
            M<S> r1 = first(t);

            return r1.success
                ? secnd(r1)
                : 

            return first(secnd(t));
        }
    }
};
