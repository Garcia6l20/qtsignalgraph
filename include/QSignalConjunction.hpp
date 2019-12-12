#pragma once

#include <QObject>
#include <QSignalDetails.hpp>
#include <QSignalSource.hpp>
#include <set>
#include <memory>

class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;
class QSignalDisjunction;
using QSignalDisjunctionPtr = std::shared_ptr<QSignalDisjunction>;

class QSignalConjunction: public QObject,
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder<QSignalConjunction> {
    Q_OBJECT
public:
    QSignalConjunction(std::set<QSignalSource>&& conns, QObject* parent = nullptr);
    QSignalConjunction(std::initializer_list<QSignalSource>&& sources, QObject* parent = nullptr);

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

    QSignalConjunction(QSignalConjunction&&) = delete;
    QSignalConjunction& operator=(const QSignalConjunction&&) = delete;
    QSignalConjunction(const QSignalConjunction&) = delete;
    QSignalConjunction& operator=(const QSignalConjunction&) = delete;

signals:
    void done(QVariant);

private:
    std::set<QSignalSource> _sources;
};

inline QSignalConjunctionPtr operator&(QSignalSource&& lhs, QSignalSource&& rhs) {
    return std::make_shared<QSignalConjunction>(std::set{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
}

class QSignalDisjunction: public QObject,
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder<QSignalDisjunction>  {
    Q_OBJECT
public:
    QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& conns, QObject* parent = nullptr);

    QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalDisjunctionPtr&& rhs):
        QObject(nullptr),
        _successSources{{{lhs.get(), &QSignalConjunction::done}}},
        _failureSources{{{rhs.get(), &QSignalDisjunction::done}, {rhs.get(), &QSignalDisjunction::failed}}}{
        add_ref(std::forward<QSignalConjunctionPtr>(lhs));
        add_ref(std::forward<QSignalDisjunctionPtr>(rhs));
        init();
    }

    QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalSource&& rhs) :
        QObject(nullptr),
        _successSources{ {{lhs.get(), &QSignalConjunction::done}} },
        _failureSources{ {std::forward<QSignalSource>(rhs)} }{
        add_ref(std::forward<QSignalConjunctionPtr>(lhs));
        init();
    }


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
        // stream << '(' << std::get<0>(disj._sources) << " | " << std::get<1>(disj._sources) << ')';
        return stream;
    }

    QSignalDisjunction(const QSignalDisjunction&&) = delete;
    QSignalDisjunction& operator=(const QSignalDisjunction&&) = delete;
    QSignalDisjunction(const QSignalDisjunction&) = delete;
    QSignalDisjunction& operator=(const QSignalDisjunction&) = delete;

signals:
    void done(QVariant);
    void failed(QVariant);

private:
    void init();
    void cleanup();
    std::list<QSignalSource> _successSources;
    std::list<QSignalSource> _failureSources;
    std::list<QMetaObject::Connection> _successConns;
    std::list<QMetaObject::Connection> _failureConns;
};

inline QSignalDisjunctionPtr operator|(QSignalSource&& lhs, QSignalSource&& rhs) {
    return std::make_shared<QSignalDisjunction>(std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs));
}

namespace qsg::details {
    template <typename T>
    constexpr auto is_junction_v = std::disjunction_v<
        std::is_same<T, QSignalConjunctionPtr>,
        std::is_same<T, QSignalDisjunctionPtr>,
        std::is_same<T, QSignalSource>>;
}

template <typename SuccessJunctionT, typename FailureJunctionT,
          std::enable_if_t<qsg::details::is_junction_v<SuccessJunctionT> && qsg::details::is_junction_v<FailureJunctionT>, int> = 0>
inline QSignalDisjunctionPtr operator||(SuccessJunctionT&& lhs, FailureJunctionT&& rhs) {
    return std::make_shared<QSignalDisjunction>(std::forward<SuccessJunctionT>(lhs), std::forward<FailureJunctionT>(rhs));
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalSource&& rhs) {
    auto disj = std::make_shared<QSignalDisjunction>(std::make_tuple<QSignalSource, QSignalSource>({&*lhs, &QSignalConjunction::done},
                                                                       std::forward<QSignalSource>(rhs)));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalSource&& lhs, QSignalConjunctionPtr&& rhs) {
    auto disj = std::make_shared<QSignalDisjunction>(std::make_tuple<QSignalSource, QSignalSource>(std::forward<QSignalSource>(lhs), {&*rhs, &QSignalConjunction::done}));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(rhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalConjunctionPtr&& rhs) {
    auto disj = std::make_shared<QSignalDisjunction>(std::make_tuple<QSignalSource, QSignalSource>({&*lhs, &QSignalConjunction::done}, {&*rhs, &QSignalConjunction::done}));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(rhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalDisjunctionPtr&& rhs) {
    return std::make_shared<QSignalDisjunction>(std::forward< QSignalConjunctionPtr>(lhs), std::forward<QSignalDisjunctionPtr>(rhs));
}
