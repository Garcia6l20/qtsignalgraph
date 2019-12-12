#pragma once

#include <QObject>
#include <QSignalBaseJunction.hpp>
#include <QSignalSource.hpp>
#include <memory>

class QSignalDisjunction;
using QSignalDisjunctionPtr = std::shared_ptr<QSignalDisjunction>;
class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;

class QSignalDisjunction: public QObject, public QSignalBaseJunction<QSignalDisjunction> {
    Q_OBJECT

signals:
    void done(QVariant);

protected:
    using QSignalBaseJunction<QSignalDisjunction>::QSignalBaseJunction;

private:
    inline void do_connect(QSignalSource&& src);

    // frendship
    friend class QSignalBaseJunction<QSignalDisjunction>;
    friend std::ostream& operator<<(std::ostream& stream, const QSignalDisjunction& disj);
};

#include "QSignalDisjunction.tpp"
