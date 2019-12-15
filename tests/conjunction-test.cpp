#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QGraphConjunctionTest: public QObject {
    Q_OBJECT
private slots:
    void manualConjunction() {
        TestObject obj1;
        TestObject obj2;
        auto step = QSignalConjunction::make(
            QSignalSource{&obj1, &TestObject::done},
            QSignalSource{&obj2, &TestObject::done}
        );
        bool trigged = false;
        step->done([&trigged] {
            trigged = true;
        });
        obj1.done();
        QCOMPARE(trigged, false);
        obj2.done();
        QCOMPARE(trigged, true);
    }
    void buildConjunction() {
        TestObject obj1;
        TestObject obj2;
        TestObject obj3;
        bool trigged = false;
        auto conj = QSignalSource{&obj1, &TestObject::done} &
                     QSignalSource{&obj2, &TestObject::done} &
                     QSignalSource{&obj3, &TestObject::done};
        conj->done([&trigged] {
            trigged = true;
        });
        obj1.done();
        QCOMPARE(trigged, false);
        obj2.done();
        QCOMPARE(trigged, false);
        obj3.done();
        QCOMPARE(trigged, true);
        obj1.done();
        obj2.done();
    }
};


QTEST_MAIN(QGraphConjunctionTest)
#include "conjunction-test.moc"
