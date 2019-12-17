#include <QSignalBaseJunction.hpp>


template <typename Derived>
struct QSignalBaseJunction<Derived>::accessor: Derived {
private:
    template<class Ret, class... Args>
    static auto _invoke(Derived& derived, Ret(Derived::*fcn)(Args&&...), Args&&...args) {
        return (derived.*fcn)(std::forward<Args>(args)...);
    }
public:
    template<typename T>
    static void derived_do_connect(Derived& derived, T&& src) {
        _invoke<void>(derived, &Derived::do_connect, std::forward<T>(src));
    }
};

template <typename Derived>
Derived& QSignalBaseJunction<Derived>::derived() {
    return static_cast<Derived&>(*this);
}

template <typename Derived>
template <typename...Args>
typename QSignalBaseJunction<Derived>::pointer_type QSignalBaseJunction<Derived>::make(Args...args) {
    return std::make_shared<qsg::details::make_shared_enabler<Derived>>(std::forward<Args>(args)...);
}

template <typename Derived>
template <typename...Args>
QSignalBaseJunction<Derived>::QSignalBaseJunction(Args&&...args) {
    add(std::forward<Args>(args)...);
}

template <typename Derived>
void QSignalBaseJunction<Derived>::cleanup() {
    auto_clean_connections();
}

template <typename Derived>
template <typename FirstJunctionT, typename...JunctionsT>
void QSignalBaseJunction<Derived>::add(FirstJunctionT&&first, JunctionsT&&...rest) {
    add(std::forward<FirstJunctionT>(first));
    if constexpr (sizeof...(rest)) {
        add(std::forward<JunctionsT>(rest)...);
    }
}

template <typename Derived>
template <typename JunctionPtrT>
void QSignalBaseJunction<Derived>::add(JunctionPtrT&&junction) {
    accessor::derived_do_connect(derived(), std::forward<JunctionPtrT>(junction));
    add_ref(std::forward<JunctionPtrT>(junction));
}

template <typename Derived>
inline void QSignalBaseJunction<Derived>::add(QSignalSource&& src) {
    accessor::derived_do_connect(derived(), std::forward<QSignalSource>(src));
    _sources.emplace(std::forward<QSignalSource>(src));
}
