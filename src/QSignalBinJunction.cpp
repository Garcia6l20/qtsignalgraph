#include <QSignalBinJunction.hpp>
#include <QDebug>

#include <sstream>

void QSignalBinJunction::cleanup() {
    disconnect(_trueConn);
    disconnect(_falseConn);
    auto reset_visitor = qsg::details::overloaded{
        [](QSignalSource&) {},
        [this](auto& junc) {
            junc->clear();
        }
    };
    std::visit(reset_visitor, _trueSource);
    std::visit(reset_visitor, _falseSource);
}

QSignalBinJunction::QSignalBinJunction(QSignalSource&&true_src, QSignalSource&& false_src):
    _trueSource{true_src},
    _falseSource{false_src} {
    do_connect();
}
