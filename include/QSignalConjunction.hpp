#pragma once

#include <QObject>
#include <QSignalBaseJunction.hpp>
#include <QSignalSource.hpp>
#include <set>
#include <memory>

class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalConjunction: public QSignalBaseJunction<QSignalConjunction> {
private:
    using QSignalBaseJunction<QSignalConjunction>::QSignalBaseJunction;

    inline void do_connect(QSignalSource&& src);

    template <typename JunctionPtrT>
    void do_connect(JunctionPtrT&& src);

    // friendship
    friend accessor;
    friend std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj);
};

#include "QSignalConjunction.tpp"
