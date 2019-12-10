#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>

#include "TestObject.hpp"

class QConnectionDescTest: public QObject {
    Q_OBJECT
private:
    struct mixed_test1_data {
        TestObject* srcOk1 = new TestObject();
        TestObject* srcOk2  = new TestObject();
        TestObject* srcFailed  = new TestObject();
        QSignalGraph * graph;
        mixed_test1_data() = default;
        mixed_test1_data(mixed_test1_data&&) = default;
        mixed_test1_data(const mixed_test1_data&) = delete;
        ~mixed_test1_data() {
            srcOk1->deleteLater();
            srcOk2->deleteLater();
            srcFailed->deleteLater();
        }
    };
    auto make_complex_test() {
        mixed_test1_data data;
        auto& [srcOk1, srcOk2, srcFailed, graph] = data;
        graph = QSignalSource{srcOk1, &TestObject::done} && QSignalSource{srcOk2, &TestObject::done} ||
                QSignalSource{srcFailed, &TestObject::done};
        return data;
    }
private slots:
    void basicLambdaConnect() {
        TestObject obj1;
        auto d = QSignalSource(&obj1, &TestObject::done);
        bool trigged = false;
        d.do_connect([&trigged] {
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
        auto graph = QSignalSource{&obj1, &TestObject::done} &&
                     QSignalSource{&obj2, &TestObject::done};
        connect(graph, &QSignalGraph::done, [&trigged] {
            trigged = true;
        });
        obj1.done();
        QCOMPARE(trigged, false);
        obj2.done();
        QCOMPARE(trigged, true);
        graph->deleteLater();
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
        auto& [srcOk1, srcOk2, srcFailed, graph] = data;
        std::optional<bool> ok;
        QObject::connect(graph, &QSignalGraph::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph, &QSignalGraph::failed, [&ok] {
            ok = false;
        });
        QCOMPARE(ok.has_value(), false);
        srcOk1->done();
        QCOMPARE(ok.has_value(), false);
        srcOk2->done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, true);
    }
    void mixedTest1_Failed1() {
        auto data = make_complex_test();
        auto& [srcOk1, srcOk2, srcFailed, graph] = data;
        std::optional<bool> ok;
        QObject::connect(graph, &QSignalGraph::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph, &QSignalGraph::failed, [&ok] {
            ok = false;
        });
        QCOMPARE(ok.has_value(), false);
        srcOk1->done();
        QCOMPARE(ok.has_value(), false);
        srcFailed->done();
        QCOMPARE(ok.has_value(), true);
        QCOMPARE(*ok, false);
    }
};


QTEST_MAIN(QConnectionDescTest)
#include "basic-test.moc"
