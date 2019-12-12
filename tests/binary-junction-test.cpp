#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

class QSignalGraphTest: public QObject {
    Q_OBJECT
private:
    struct mixed_test1_data {
        TestObject* srcOk1 = new TestObject();
        TestObject* srcOk2  = new TestObject();
        TestObject* srcFailed1  = new TestObject();
        TestObject* srcFailed2  = new TestObject();
        QSignalBinJunctionPtr graph;
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
    void mixedTest1_Ok() {
        auto data = make_complex_test();
        auto& [srcOk1, srcOk2, srcFailed1, srcFailed2, graph] = *data;
        std::optional<bool> ok;
        QObject::connect(&*graph, &QSignalBinJunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(&*graph, &QSignalBinJunction::failed, [&ok] {
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
        QObject::connect(graph.get(), &QSignalBinJunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph.get(), &QSignalBinJunction::failed, [&ok] {
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
        QObject::connect(graph.get(), &QSignalBinJunction::done, [&ok] {
            ok = true;
        });
        QObject::connect(graph.get(), &QSignalBinJunction::failed, [&ok] {
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


QTEST_MAIN(QSignalGraphTest)
#include "binary-junction-test.moc"
