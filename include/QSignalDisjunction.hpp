#pragma once

#include <QObject>
#include <QSignalBaseJunction.hpp>
#include <QSignalSource.hpp>
#include <memory>

class QSignalDisjunction;
using QSignalDisjunctionPtr = std::shared_ptr<QSignalDisjunction>;
class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalDisjunction: public QSignalBaseJunction<QSignalDisjunction> {
private:
    using QSignalBaseJunction<QSignalDisjunction>::QSignalBaseJunction;

    inline void do_connect(QSignalSource&& src);

    template <typename JunctionPtrT>
    void do_connect(JunctionPtrT&& src);

    // frendship
    friend accessor;
    friend std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj);
};

#include "QSignalDisjunction.tpp"
