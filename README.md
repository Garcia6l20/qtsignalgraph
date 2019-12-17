# QSignalGraph

> Qt signal graph-like composition library

## Usage

- Signal conjunction:
    > Signal conjunctions can be interpreted like a logical *and*:
    ```cpp
    auto src1 = new QObject();
    auto src2 = new QObject();
    auto src3 = new QObject();
    auto graph = QSignalSource{src1, &QObject::destroyed} &
        QSignalSource{src2, &QObject::destroyed} &
        QSignalSource{src3, &QObject::destroyed};
    graph->on_done([] {
        qDebug() << "All objects have been destroyed";
    });
    ```

- Signal disjunctions:
    > Signal disjunctions can be interpreted like a logical *or*:
    ```cpp
    auto src1 = new QObject();
    auto src2 = new QObject();
    auto src3 = new QObject();
    auto graph = QSignalSource{src1, &QObject::destroyed} |
        QSignalSource{src2, &QObject::destroyed} |
        QSignalSource{src3, &QObject::destroyed};
    graph->on_done([] {
        qDebug() << "An object have been destroyed";
    });
    ```

- Binary junction:
    ```cpp
    auto src1 = new QObject();
    auto src2 = new QObject();
    auto src3 = new QObject();
    auto graph = (QSignalSource{src1, &QObject::destroyed} & QSignalSource{src2, &QObject::destroyed}) ||
        QSignalSource{src3, &QObject::destroyed};
    graph->on_done([] {
        qDebug() << "Src1 and src2 object have been destroyed";
    });
    graph->on_failed([] {
        qDebug() << "Src3 object have been destroyed";
    });
    ```
