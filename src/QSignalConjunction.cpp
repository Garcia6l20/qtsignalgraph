#include <QSignalConjunction.hpp>

#include <memory>

QSignalConjunction::QSignalConjunction(std::set<QSignalSource>&& sources, QObject* parent):
    QObject (parent),
    _sources{}
{
    for (auto src: sources) {
        add(std::forward<QSignalSource>(src));
    }
}

QSignalConjunction::QSignalConjunction(std::initializer_list<QSignalSource>&& sources, QObject* parent):
    QSignalConjunction(std::set(std::forward<std::initializer_list<QSignalSource>>(sources)), parent)
{}

void QSignalConjunction::add(QSignalSource &&src) {
    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = src.do_connect([src, conn, this] {
        disconnect(*conn);
        _sources.erase(src);
        if (_sources.empty())
            emit done();
    });
    _sources.emplace(std::forward<QSignalSource>(src));
}


QSignalDisjunction::QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& sources, QObject* parent):
    QObject (parent),
    _sources{std::forward<std::tuple<QSignalSource, QSignalSource>>(sources)}
{
    auto done_conn = std::make_shared<QMetaObject::Connection>();
    auto failed_conn = std::make_shared<QMetaObject::Connection>();
    *done_conn = std::get<0>(_sources).do_connect([done_conn, failed_conn, this] {
        disconnect(*done_conn);
        disconnect(*failed_conn);
            emit done();
    });
    *done_conn = std::get<1>(_sources).do_connect([done_conn, failed_conn, this] {
        disconnect(*done_conn);
        disconnect(*failed_conn);
            emit failed();
    });
}
