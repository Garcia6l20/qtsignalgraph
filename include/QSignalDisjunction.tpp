#include <QSignalDisjunction.hpp>
#include <QSignalConjunction.hpp>

#include <sstream>

void QSignalDisjunction::do_connect(QSignalSource&& src) {
    auto conn = src.do_connect([src, this](QVariant data) {
        std::stringstream ss;
        ss << src;
        qDebug() << this << "from" << QString::fromStdString(ss.str());
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
