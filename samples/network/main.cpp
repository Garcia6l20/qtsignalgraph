#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSignalGraph.hpp>
#include <QCommandLineParser>

int main(int argc, char** argv) {

    static_assert(qsg::details::is_junction_v<QSignalConjunctionPtr>, "");
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption{"url1", "The first url to fetch", "url1", "http://google.com"});
    parser.addOption(QCommandLineOption{"url2", "The second url to fetch", "url2", "http://cppreference.com"});
    parser.process(app);
    auto url1 = parser.value("url1");
    auto url2 = parser.value("url1");
    QNetworkAccessManager netmanager;
    QNetworkRequest req1(url1);
    QNetworkRequest req2(url2);
    auto reply1 = netmanager.get(req1);
    auto reply2 = netmanager.get(req2);
    auto error_sig = QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error);
    auto finish_sig = &QNetworkReply::finished;
    auto graph = (QSignalSource{reply1, finish_sig} & QSignalSource{reply2, finish_sig})
              || (QSignalSource{reply1, error_sig} | QSignalSource{reply2, error_sig});
    QObject::connect(graph.get(), &QSignalDisjunction::done, [url1, reply1, url2, reply2] {
        qInfo().noquote() << QString("from %1: ").arg(url1);
        qInfo().noquote() << reply1->readAll();
        qInfo().noquote() << QString("from %1: ").arg(url2);
        qInfo().noquote() << reply2->readAll();
        qApp->quit();
    });
    QObject::connect(graph.get(), &QSignalDisjunction::failed, [url1, reply1, url2, reply2] {
        qCritical().noquote() << "failure...";
        qInfo().noquote() << QString("from %1: ").arg(url1);
        qInfo().noquote() << reply1->errorString();
        qInfo().noquote() << QString("from %1: ").arg(url2);
        qInfo().noquote() << reply2->errorString();
        qApp->exit(-1);
    });
    return app.exec();
}
