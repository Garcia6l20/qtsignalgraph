#pragma once

#include <QObject>
#include <QDebug>

class TestObject : public QObject
{
    Q_OBJECT
public:
    explicit TestObject(QObject *parent = nullptr);

    bool trigged() const noexcept { return _trigged; }

signals:
    void done();
    void fwd(int);
    void fwd(double);
    void fwd(QString);

public slots:
    void reset() noexcept { _trigged = false; }

public slots:
    void trigMe() {
        _trigged = true;
    }
private:
    bool _trigged = false;
};
