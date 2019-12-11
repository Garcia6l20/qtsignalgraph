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

    friend std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj) {
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

    friend std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj) {
        stream << '(' << std::get<0>(disj._sources) << " | " << std::get<1>(disj._sources) << ')';
        return stream;
    }

signals:
    void done();
    void failed();

private:
    std::tuple<QSignalSource, QSignalSource> _sources;
};

