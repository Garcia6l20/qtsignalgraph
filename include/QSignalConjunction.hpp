#pragma once

#include <QObject>
#include <QSignalBaseJunction.hpp>
#include <QSignalSource.hpp>
#include <set>
#include <memory>

class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalConjunction: public QSignalBaseJunction<QSignalConjunction> {
protected:
    using QSignalBaseJunction<QSignalConjunction>::QSignalBaseJunction;

private:
    inline void do_connect(QSignalSource&& src);

    template <typename JunctionPtrT>
    void do_connect(JunctionPtrT&& src);

    // friendship
    friend class QSignalBaseJunction<QSignalConjunction>;
    friend std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj);
};

#include "QSignalConjunction.tpp"
