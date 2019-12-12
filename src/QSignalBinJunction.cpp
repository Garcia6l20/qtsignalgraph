#include <QSignalBinJunction.hpp>
#include <QDebug>

#include <sstream>

void QSignalBinJunction::cleanup() {
    disconnect(_trueConn);
    disconnect(_falseConn);
}

QSignalBinJunction::QSignalBinJunction(QSignalSource&&true_src, QSignalSource&& false_src):
    _trueSource{true_src},
    _falseSource{false_src} {
    do_connect();
}
