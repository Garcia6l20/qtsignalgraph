#pragma once

#include <QObject>
#include <QSignalSource.hpp>
#include <set>

class QSignalConjunction: public QObject {
    Q_OBJECT
public:
    QSignalConjunction(std::set<QSignalSource>&& conns, QObject* parent = nullptr);
    QSignalConjunction(std::initializer_list<QSignalSource>&& sources, QObject* parent = nullptr);

    template <typename Func>
    QSignalConjunction& operator>>(Func func) {
        connect(this, &QSignalConjunction::done, func);
        return *this;
    }

    void add(QSignalSource&& src);

signals:
    void done();

private:
    std::set<QSignalSource> _sources;
};

class QSignalDisjunction: public QObject {
    Q_OBJECT
public:
    QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& conns, QObject* parent = nullptr);

    QSignalDisjunction(QSignalSource&& success_source, QSignalSource&& failure_source, QObject* parent = nullptr):
        QSignalDisjunction(std::tuple{std::forward<QSignalSource>(success_source), std::forward<QSignalSource>(failure_source)}, parent) {
    }

    template <typename DoneFunc, typename FailedFunc>
    std::tuple<QMetaObject::Connection, QMetaObject::Connection> operator>>(std::tuple<DoneFunc, FailedFunc> funcs) {
        return {
            connect(this, &QSignalDisjunction::done, std::get<0>(funcs)),
            connect(this, &QSignalDisjunction::failed, std::get<1>(funcs))
        };
    }

signals:
    void done();
    void failed();

private:
    std::tuple<QSignalSource, QSignalSource> _sources;
};

