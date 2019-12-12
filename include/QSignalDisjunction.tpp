#include <QSignalDisjunction.hpp>
#include <QSignalConjunction.hpp>

template <typename...Args>
QSignalDisjunctionPtr QSignalDisjunction::make(Args...args) {
    return std::make_shared<qsg::details::make_shared_enabler<QSignalDisjunction>>(std::forward<Args>(args)...);
}

QSignalDisjunction::QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalDisjunctionPtr&& rhs):
    QObject(nullptr),
    _successSources{{{lhs.get(), &QSignalConjunction::done}}},
    _failureSources{{{rhs.get(), &QSignalDisjunction::done}, {rhs.get(), &QSignalDisjunction::failed}}}{
    add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    add_ref(std::forward<QSignalDisjunctionPtr>(rhs));
    init();
}

QSignalDisjunction::QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalSource&& rhs) :
    QObject(nullptr),
    _successSources{ {{lhs.get(), &QSignalConjunction::done}} },
    _failureSources{ {std::forward<QSignalSource>(rhs)} }{
    add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    init();
}

QSignalDisjunction::QSignalDisjunction(QSignalSource&& success_source, QSignalSource&& failure_source):
    QSignalDisjunction(std::tuple{std::forward<QSignalSource>(success_source), std::forward<QSignalSource>(failure_source)}) {
}

inline std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj) {
    // stream << '(' << std::get<0>(disj._sources) << " | " << std::get<1>(disj._sources) << ')';
    return stream;
}
