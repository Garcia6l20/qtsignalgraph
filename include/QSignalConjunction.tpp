#include <QSignalConjunction.hpp>

template <typename...Args>
QSignalConjunctionPtr QSignalConjunction::make(Args...args) {
    return std::make_shared<qsg::details::make_shared_enabler<QSignalConjunction>>(std::forward<Args>(args)...);
}

template <typename...Args>
QSignalConjunction::QSignalConjunction(Args&&...args):
    QSignalConjunction({std::forward<Args>(args)...}){
}

inline std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj) {
    stream << '(';
    auto it = conj._sources.begin();
    auto end = conj._sources.end();
    end--;
    for (; it != end; ++it) {
        stream << *it;
        stream << " & ";
    }
    stream << *it;
    stream << ')';
    return stream;
}
