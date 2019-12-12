#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QGraphDisjunctionTest: public QObject {
    Q_OBJECT
private slots:
    void simle() {
        TestObject success, success2, success3;
        std::optional<bool> trigged = false;
        auto graph = QSignalSource{&success, &TestObject::done} |
                     QSignalSource{&success2, &TestObject::done} |
                     QSignalSource{&success3, &TestObject::done};
        connect(graph.get(), &QSignalDisjunction::done, [&trigged] {
            trigged = true;
        });
        success2.done();
        QVERIFY(trigged.has_value());
        QCOMPARE(*trigged, true);
    }
    void manualDisjunction() {
        TestObject oksource;
        TestObject nopesource;
        auto step = QSignalDisjunction::make(
            QSignalSource{&oksource, &TestObject::done},
            QSignalSource{&nopesource, &TestObject::done}
        );
        std::optional<bool> ok;
        QObject::connect(step.get(), &QSignalDisjunction::done, [&ok](const QVariant&) {
            ok = true;
        });
        QCOMPARE(ok.has_value(), false);
        oksource.done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, true);
    }
};


QTEST_MAIN(QGraphDisjunctionTest)
#include "disjunction-test.moc"
