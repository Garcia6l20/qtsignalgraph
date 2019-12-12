#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QGraphDisjunctionTest: public QObject {
    Q_OBJECT
private slots:
    void buildDisjunction() {
        TestObject success, success2, failure;
        std::optional<bool> trigged = false;
        auto graph = (QSignalSource{&success, &TestObject::done} & QSignalSource{&success2, &TestObject::done}) ||
                     (QSignalSource{&failure, &TestObject::done});
        connect(graph.get(), &QSignalDisjunction::done, [&trigged] {
            trigged = true;
        });
        connect(graph.get(), &QSignalDisjunction::failed, [&trigged] {
            trigged = false;
        });
        success.done();
        success2.done();
        QVERIFY(trigged.has_value());
        QCOMPARE(*trigged, true);
        failure.done(); // shall not be trigged
        QCOMPARE(*trigged, true);
        success.done();
        success2.done();
    }
    void buildDisjunctionFailure() {
        TestObject success, success2, failure;
        std::optional<bool> trigged = false;
        auto graph = QSignalSource{&success, &TestObject::done} & QSignalSource{&success2, &TestObject::done} ||
                     QSignalSource{&failure, &TestObject::done};
        connect(graph.get(), &QSignalDisjunction::done, [&trigged] {
            trigged = true;
        });
        connect(graph.get(), &QSignalDisjunction::failed, [&trigged] {
            trigged = false;
        });
        failure.done();
        QVERIFY(trigged.has_value());
        QCOMPARE(*trigged, false);
        success.done();  // shall not be trigged
        success2.done();
        QCOMPARE(*trigged, false);
        success.done();
        success2.done();
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
        QObject::connect(step.get(), &QSignalDisjunction::failed, [&ok] {
            ok = false;
        });
        QCOMPARE(ok.has_value(), false);
        oksource.done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, true);
        nopesource.done(); // shall not retrig target signal
        QCOMPARE(*ok, true);
    }
};


QTEST_MAIN(QGraphDisjunctionTest)
#include "disjunction-test.moc"
