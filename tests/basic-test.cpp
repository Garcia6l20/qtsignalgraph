#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QGraphBasiTest: public QObject {
    Q_OBJECT
private slots:
    void basicLambdaConnect() {
        TestObject obj1;
        auto d = QSignalSource(&obj1, &TestObject::done);
        bool trigged = false;
        d.do_connect([&trigged](const QVariant&) {
            trigged = true;
        });
        obj1.done();
        QCOMPARE(trigged, true);
    }
    void basicObjectConnect() {
        TestObject obj1;
        TestObject obj2;
        auto d = QSignalSource(&obj1, &TestObject::done);
        d.do_connect(&obj2, &TestObject::trigMe);
        obj1.done();
        QCOMPARE(obj2.trigged(), true);
    }
};


QTEST_MAIN(QGraphBasiTest)
#include "basic-test.moc"
