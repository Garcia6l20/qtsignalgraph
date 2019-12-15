#include <QSignalBinJunction.hpp>
#include <QSignalDisjunction.hpp>
#include <QSignalConjunction.hpp>

template <typename...Args>
QSignalBinJunctionPtr QSignalBinJunction::make(Args...args) {
    return std::make_shared<qsg::details::make_shared_enabler<QSignalBinJunction>>(std::forward<Args>(args)...);
}

void QSignalBinJunction::do_connect() {
    std::visit(qsg::details::overloaded {
        [this](QSignalSource& src) {
            _trueConn = src.do_connect([this](QVariant data) {
#ifdef QT_SIGNALGRAPH_DEBUG
                std::stringstream ss;
                ss << src;
                qDebug() << this << "success from" << QString::fromStdString(ss.str());
#endif
                cleanup();
                emit done(std::move(data));
            });
            add_auto_clean_connection(_trueConn);
        },
        [this](auto& src) {
            src->done([this](QVariant data) {
                cleanup();
                emit done(std::move(data));
            });
        },
    }, _trueSource);
    std::visit(qsg::details::overloaded{
        [this](QSignalSource& src) {
            _trueConn = src.do_connect([this](QVariant data) {
#ifdef QT_SIGNALGRAPH_DEBUG
                std::stringstream ss;
                ss << _trueSource;
                qDebug() << this << "failure from" << QString::fromStdString(ss.str());
#endif
                cleanup();
                emit failed(std::move(data));
            });
            add_auto_clean_connection(_trueConn);
        },
        [this](auto& src) {
            src->done([this](QVariant data) {
                cleanup();
                emit failed(std::move(data));
            });
        },
    }, _falseSource);
}

template <class TrueJunctionPtrT, class FalseJunctionPtrT>
QSignalBinJunction::QSignalBinJunction(TrueJunctionPtrT&&true_j, FalseJunctionPtrT&&false_j):
    _trueSource(true_j),
    _falseSource(false_j) {
    do_connect();
}

template <class TrueJunctionT>
QSignalBinJunction::QSignalBinJunction(TrueJunctionT&&true_j, QSignalSource&&false_src):
    _trueSource{true_j},
    _falseSource{false_src} {
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
