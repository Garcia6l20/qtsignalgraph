#include <QSignalDisjunction.hpp>
#include <QDebug>

#include <sstream>

QSignalDisjunction::QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& sources):
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
