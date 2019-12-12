#include <QSignalBinJunction.hpp>
#include <QSignalDisjunction.hpp>
#include <QSignalConjunction.hpp>

template <typename...Args>
QSignalBinJunctionPtr QSignalBinJunction::make(Args...args) {
    return std::make_shared<qsg::details::make_shared_enabler<QSignalBinJunction>>(std::forward<Args>(args)...);
}

void QSignalBinJunction::do_connect() {
    _trueConn = _trueSource.do_connect([this](QVariant data) {
        std::stringstream ss;
        ss << _trueSource;
        qDebug() << this << "success from" << QString::fromStdString(ss.str());
        cleanup();
        emit done(std::move(data));
    });
    add_auto_clean_connection(_trueConn);
    _falseConn = _falseSource.do_connect([this](QVariant data) {
        std::stringstream ss;
        ss << _falseSource;
        qDebug() << this << "failure from" << QString::fromStdString(ss.str());
        cleanup();
        emit failed(std::move(data));
    });
    add_auto_clean_connection(_falseConn);
}

template <class TrueJunctionPtrT, class FalseJunctionPtrT>
QSignalBinJunction::QSignalBinJunction(TrueJunctionPtrT&&true_j, FalseJunctionPtrT&&false_j):
    _trueSource({true_j.get(), &std::decay_t<decltype(*true_j)>::done}),
    _falseSource({false_j.get(), &std::decay_t<decltype(*false_j)>::done}) {
    add_ref(std::move(true_j));
    add_ref(std::move(false_j));
    do_connect();
}

template <class TrueJunctionT>
QSignalBinJunction::QSignalBinJunction(TrueJunctionT&&true_j, QSignalSource&&false_src):
    _trueSource{{true_j.get(), &TrueJunctionT::done}},
    _falseSource{false_src} {
    add_ref(true_j);
    do_connect();
}

template <typename FalseJunctionT>
QSignalBinJunction::QSignalBinJunction(QSignalSource&&true_src, FalseJunctionT&& false_j):
    _trueSource{true_src},
    _falseSource{{false_j.get(), &FalseJunctionT::done}} {
    add_ref(false_j);
    do_connect();
}

inline std::ostream& operator<<(std::ostream& stream, const QSignalBinJunction& disj) {
    // stream << '(' << std::get<0>(disj._sources) << " | " << std::get<1>(disj._sources) << ')';
    return stream;
}
