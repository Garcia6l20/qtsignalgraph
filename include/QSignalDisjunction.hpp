#pragma once

#include <QObject>
#include <QSignalDetails.hpp>
#include <QSignalSource.hpp>
#include <memory>

class QSignalDisjunction;
using QSignalDisjunctionPtr = std::shared_ptr<QSignalDisjunction>;
class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalDisjunction: public QObject,
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder<QSignalDisjunction>  {
    Q_OBJECT
public:

    /**
     * @brief Create a signal disjunction
     * @param args... Disjuntion arguments
     * @tparam Args... Disjuntion argument types
     * @return The new disjunction
     */
    template <typename...Args>
    static QSignalDisjunctionPtr make(Args...args);

signals:
    void done(QVariant);
    void failed(QVariant);
protected:
    QSignalDisjunction(std::tuple<QSignalSource, QSignalSource>&& conns);
    inline QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalDisjunctionPtr&& rhs);
    inline QSignalDisjunction(QSignalConjunctionPtr&& lhs, QSignalSource&& rhs);
    inline QSignalDisjunction(QSignalSource&& success_source, QSignalSource&& failure_source);

    QSignalDisjunction(const QSignalDisjunction&&) = delete;
    QSignalDisjunction& operator=(const QSignalDisjunction&&) = delete;
    QSignalDisjunction(const QSignalDisjunction&) = delete;
    QSignalDisjunction& operator=(const QSignalDisjunction&) = delete;

private:
    void init();
    void cleanup();
    std::list<QSignalSource> _successSources;
    std::list<QSignalSource> _failureSources;
    std::list<QMetaObject::Connection> _successConns;
    std::list<QMetaObject::Connection> _failureConns;

    // frendship
    friend std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj);
};

#include "QSignalDisjunction.tpp"
