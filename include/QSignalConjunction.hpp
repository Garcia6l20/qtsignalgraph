#pragma once

#include <QObject>
#include <QSignalDetails.hpp>
#include <QSignalSource.hpp>
#include <set>
#include <memory>

class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalConjunction: public QObject,
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder<QSignalConjunction> {
    Q_OBJECT

public:
    /**
     * @brief Create a signal conjunction
     * @param args... Conjuntion arguments
     * @tparam Args... Conjuntion argument types
     * @return The new conjunction
     */
    template <typename...Args>
    static QSignalConjunctionPtr make(Args...args);

signals:
    void done(QVariant);

protected:
    QSignalConjunction(std::set<QSignalSource>&& conns);
    template <typename...Args>
    QSignalConjunction(Args&&...args);
    QSignalConjunction(std::initializer_list<QSignalSource>&& sources);

    void add(QSignalSource&& src);

    QSignalConjunction(QSignalConjunction&&) = delete;
    QSignalConjunction& operator=(const QSignalConjunction&&) = delete;
    QSignalConjunction(const QSignalConjunction&) = delete;
    QSignalConjunction& operator=(const QSignalConjunction&) = delete;

    std::set<QSignalSource> _sources;

    // friendship
    friend std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj);
};

#include "QSignalConjunction.tpp"
