#include <QSignalConjunction.hpp>
#include <QDebug>

#include <memory>
#include <sstream>

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


QSignalDisjunction::QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& sources, QObject* parent):
    QObject (parent),
    _successSources{std::forward<QSignalSource>(std::get<0>(sources))},
    _failureSources{ std::forward<QSignalSource>(std::get<1>(sources)) }
{
    init();
}

void QSignalDisjunction::init() {
    for (auto& success_src : _successSources) {
        auto conn = success_src.do_connect([success_src, this](QVariant data) {
            std::stringstream ss;
            ss << success_src;
            qDebug() << this << "success from" << QString::fromStdString(ss.str());
            cleanup();
            emit done(std::move(data));
        });
        _successConns.emplace_back(conn);
        add_auto_clean_connection(conn);
    }
    for (auto& failure_src : _failureSources) {
        auto conn = failure_src.do_connect([failure_src, this](QVariant data) {
            std::stringstream ss;
            ss << failure_src;
            qDebug() << this << "failure from" << QString::fromStdString(ss.str());
            cleanup();
            emit failed(std::move(data));
        });
        _failureConns.emplace_back(conn);
        add_auto_clean_connection(conn);
    }
}

void QSignalDisjunction::cleanup() {
    for (auto& success_conn : _successConns) {
        disconnect(success_conn);
    }
    for (auto& failure_conn: _failureConns) {
        disconnect(failure_conn);
    }
}