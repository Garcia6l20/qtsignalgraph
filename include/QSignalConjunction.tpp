#include <QSignalConjunction.hpp>
#include <sstream>

void QSignalConjunction::do_connect(QSignalSource &&src) {
    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = src.do_connect([src = src, conn, this](QVariant data) {
        std::stringstream ss;
        ss << src;
        qDebug() << this << QString::fromStdString(ss.str());
        disconnect(*conn);
        _sources.erase(src);
        if (_sources.empty()) {
            cleanup();
            emit done(std::move(data));
        }
    });
    add_auto_clean_connection(*conn);
    _sources.emplace(std::forward<QSignalSource>(src));
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
