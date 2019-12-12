#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QConnectionDescTest: public QObject {
    Q_OBJECT
private:
    struct mixed_test1_data {
        TestObject* srcOk1 = new TestObject();
        TestObject* srcOk2  = new TestObject();
        TestObject* srcFailed1  = new TestObject();
        TestObject* srcFailed2  = new TestObject();
        QSignalDisjunctionPtr graph;
        mixed_test1_data() = default;
        mixed_test1_data(mixed_test1_data&&) = default;
        mixed_test1_data(const mixed_test1_data&) = delete;
        ~mixed_test1_data() {
            srcOk1->deleteLater();
            srcOk2->deleteLater();
            srcFailed1->deleteLater();
            srcFailed2->deleteLater();
        }
    };
    auto make_complex_test() {
        auto data = std::make_shared<mixed_test1_data>();
        auto& [srcOk1, srcOk2, srcFailed1, srcFailed2, graph] = *data;
        graph = (QSignalSource{srcOk1, &TestObject::done} & QSignalSource{srcOk2, &TestObject::done}) ||
                (QSignalSource{srcFailed1, &TestObject::done} | QSignalSource{srcFailed2, &TestObject::done});
        return data;
    }
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
    void manualConjunction() {
        TestObject obj1;
        TestObject obj2;
        QSignalConjunction step {
            {&obj1, &TestObject::done},
            {&obj2, &TestObject::done},
        };
        bool trigged = false;
        QObject::connect(&step, &QSignalConjunction::done, [&trigged] {
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
        bool trigged = false;
        auto conj = QSignalSource{&obj1, &TestObject::done} &
                     QSignalSource{&obj2, &TestObject::done};
        connect(conj.get(), &QSignalConjunction::done, [&trigged] {
            trigged = true;
        });
        obj1.done();
        QCOMPARE(trigged, false);
        obj2.done();
        QCOMPARE(trigged, true);
        obj1.done();
        obj2.done();
    }
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
        QSignalDisjunction step {
            {&oksource, &TestObject::done},
            {&nopesource, &TestObject::done}
        };
        std::optional<bool> ok;
        QObject::connect(&step, &QSignalDisjunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(&step, &QSignalDisjunction::failed, [&ok] {
            ok = false;
        });
        QCOMPARE(ok.has_value(), false);
        oksource.done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, true);
        nopesource.done(); // shall not retrig target signal
        QCOMPARE(*ok, true);
    }
    void mixedTest1_Ok() {
        auto data = make_complex_test();
        auto& [srcOk1, srcOk2, srcFailed1, srcFailed2, graph] = *data;
        std::optional<bool> ok;
        QObject::connect(&*graph, &QSignalDisjunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(&*graph, &QSignalDisjunction::failed, [&ok] {
            ok = false;
        });
        QCOMPARE(ok.has_value(), false);
        srcOk1->done();
        QCOMPARE(ok.has_value(), false);
        srcOk2->done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, true);
        srcFailed1->done(); // must not trig failure
        QCOMPARE(*ok, true);
    }
    void mixedTest1_Failed1() {
        auto data = make_complex_test();
        auto& [srcOk1, srcOk2, srcFailed1, srcFailed2, graph] = *data;
        std::optional<bool> ok;
        QObject::connect(graph.get(), &QSignalDisjunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph.get(), &QSignalDisjunction::failed, [&ok] {
            ok = false;
        });
        std::cout << graph << std::endl;
        QCOMPARE(ok.has_value(), false);
        srcOk1->done();
        QCOMPARE(ok.has_value(), false);
        srcFailed1->done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, false);
        ok.reset();
        srcOk2->done(); // must not trig done
        srcOk1->done();
        QVERIFY(!ok.has_value());
    }
    void mixedTest1_Failed2() {
        auto data = make_complex_test();
        auto& [srcOk1, srcOk2, srcFailed1, srcFailed2, graph] = *data;
        std::optional<bool> ok;
        QObject::connect(graph.get(), &QSignalDisjunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph.get(), &QSignalDisjunction::failed, [&ok] {
            ok = false;
        });
        std::cout << graph << std::endl;
        QCOMPARE(ok.has_value(), false);
        srcFailed2->done();
        QCOMPARE(ok.has_value(), true);
        srcFailed1->done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, false);
    }
};


QTEST_MAIN(QConnectionDescTest)
#include "basic-test.moc"
