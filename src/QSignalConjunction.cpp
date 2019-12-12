#include <QSignalConjunction.hpp>
#include <QDebug>
#include <sstream>

QSignalConjunction::QSignalConjunction(std::set<QSignalSource>&& sources):
    _sources{}
{
    for (auto src: sources) {
        add(std::forward<QSignalSource>(src));
    }
}

QSignalConjunction::QSignalConjunction(std::initializer_list<QSignalSource>&& sources):
    QSignalConjunction(std::set(std::forward<std::initializer_list<QSignalSource>>(sources)))
{}

void QSignalConjunction::add(QSignalSource &&src) {
    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = src.do_connect([src = src, conn, this](QVariant data) {
        std::stringstream ss;
        ss << src;
        qDebug() << this << QString::fromStdString(ss.str());
        disconnect(*conn);
        _sources.erase(src);
        if (_sources.empty())
            emit done(std::move(data));
    });
    add_auto_clean_connection(*conn);
    _sources.emplace(std::forward<QSignalSource>(src));
}
