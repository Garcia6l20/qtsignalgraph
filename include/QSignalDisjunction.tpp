#include <QSignalDisjunction.hpp>
#include <QSignalConjunction.hpp>

#include <sstream>

void QSignalDisjunction::do_connect(QSignalSource&& src) {
    auto conn = src.do_connect([this
#ifdef QT_SIGNALGRAPH_DEBUG
        , src
#endif
        ](QVariant data) {
#ifdef QT_SIGNALGRAPH_DEBUG
        std::stringstream ss;
        ss << src;
        qDebug() << this << "from" << QString::fromStdString(ss.str());
#endif
        cleanup();
        emit done(std::move(data));
    });
    _conns.emplace_back(conn);
    add_auto_clean_connection(conn);

}

inline std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj) {
    // stream << '(' << std::get<0>(disj._sources) << " | " << std::get<1>(disj._sources) << ')';
    return stream;
}
