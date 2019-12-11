#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSignalGraph.hpp>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QNetworkAccessManager netmanager;
    QNetworkRequest google_req(QUrl("http://google.com"));
    QNetworkRequest cppref_req(QUrl("http://cppreference.com"));
    auto google_reply = netmanager.get(google_req);
    auto cppref_reply = netmanager.get(cppref_req);
    auto graph = (QSignalSource{google_reply, &QNetworkReply::finished} & QSignalSource{cppref_reply, &QNetworkReply::finished});
    QObject::connect(graph, &QSignalGraph::done, [google_reply, cppref_reply] {
        qDebug() << "from google: " << google_reply->readAll();
        qDebug() << "from cppref: " << cppref_reply->readAll();
        qApp->quit();
    });
    return app.exec();
}
