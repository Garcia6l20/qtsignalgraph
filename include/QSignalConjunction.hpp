#pragma once

#include <QObject>
#include <QSignalBaseJunction.hpp>
#include <QSignalSource.hpp>
#include <set>
#include <memory>

class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalConjunction: public QObject, public QSignalBaseJunction<QSignalConjunction> {
    Q_OBJECT

signals:
    void done(QVariant);

protected:
    using QSignalBaseJunction<QSignalConjunction>::QSignalBaseJunction;

private:
    inline void do_connect(QSignalSource&& src);

    // friendship
    friend class QSignalBaseJunction<QSignalConjunction>;
    friend std::ostream& operator<<(std::ostream& stream, const QSignalConjunction& conj);
};

#include "QSignalConjunction.tpp"
