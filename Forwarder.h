#pragma once
#include <utility>
#include <type_traits>
#include <functional>

namespace Graphics {

//CRTP wrapper to forward functions from a wrapped object

template<typename T>
class Forwarder {
protected:
    T m_handle;

public:
    // Forward constructors
    template<typename... Args>
    Forwarder(Args&&... args) : m_handle(std::forward<Args>(args)...) {}

    // Forward all const member functions
    template<typename... Args>
    auto operator()(auto (T::* func)(Args...) const) const {
        return [this, func](Args... args) const {
            return (m_handle.*func)(std::forward<Args>(args)...);
            };
    }

    // Forward all non-const member functions
    template<typename... Args>
    auto operator()(auto (T::* func)(Args...)) {
        return [this, func](Args... args) {
            return (m_handle.*func)(std::forward<Args>(args)...);
            };
    }

    // Direct access if needed
    const T& raw() const { return m_handle; }
    T& raw() { return m_handle; }
};

} // namespace Graphics