#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QArgForwardingTest: public QObject {
    Q_OBJECT
private:
private slots:
    void nominal() {
        TestObject obj1;
        QSignalDisjunction::pointer_type graph =
                QSignalSource(&obj1, QOverload<int>::of(&TestObject::fwd)) |
                QSignalSource(&obj1, QOverload<double>::of(&TestObject::fwd));
        std::optional<std::variant<int, double>> trigged;
        graph->on_done([&trigged](const double& value) {
            trigged = value;
        });
        obj1.fwd(42.0);
        QVERIFY(trigged);
        QCOMPARE(trigged->index(), 1);
        QCOMPARE(std::get<double>(*trigged), 42.0);
    }
//    void badHandlerType() {
//        TestObject obj1;
//        QSignalDisjunction::pointer_type graph =
//                QSignalSource(&obj1, QOverload<int>::of(&TestObject::fwd)) |
//                QSignalSource(&obj1, QOverload<QString>::of(&TestObject::fwd)) |
//                QSignalSource(&obj1, QOverload<double>::of(&TestObject::fwd));
//        std::optional<std::variant<int, double, QString>> trigged;
//        graph->on_done([&trigged](const auto& value) {
//            trigged = value;
//        });
//        obj1.fwd("42.0");
//        QVERIFY(trigged);
//        QCOMPARE(trigged->index(), 1);
//        QCOMPARE(std::get<double>(*trigged), 42.0);
//    }
};


QTEST_MAIN(QArgForwardingTest)
#include "argforwarding-test.moc"
