#include <QSignalBaseJunction.hpp>

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
    static_cast<Derived*>(this)->do_connect(junction);
    add_ref(std::forward<JunctionPtrT>(junction));
}

template <typename Derived>
inline void QSignalBaseJunction<Derived>::add(QSignalSource&& src) {
    static_cast<Derived*>(this)->do_connect(std::forward<QSignalSource>(src));
    _sources.emplace(std::forward<QSignalSource>(src));
}
