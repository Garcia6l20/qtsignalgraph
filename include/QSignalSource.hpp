#pragma once

#include <QObject>
#include <QMetaMethod>

class QSignalSource {
    typedef void(QObject::* const UnaryFunc)();
public:

    template <typename Func,
              typename std::enable_if_t<QtPrivate::FunctionPointer<Func>::IsPointerToMemberFunction &&
                                        QtPrivate::FunctionPointer<Func>::ArgumentCount == 0, int> = 0>
    QSignalSource(const typename QtPrivate::FunctionPointer<Func>::Object* object, Func func):
        _object{object},
        _signal{reinterpret_cast<void*>(func)},
        _signal_name{QMetaMethod::fromSignal(func).name()}
    {
        _do_connect = [object, func] (const QObject* target, void** target_func) {
            return QObject::connect(object, func, target, *reinterpret_cast<UnaryFunc*>(target_func));
        };
        _do_connect_lambda = [object, func] (std::function<void()> target_func) {
            return QObject::connect(object, func, target_func);
        };
    }

    QSignalSource(QSignalSource&& other):
        _object{other._object},
        _signal{other._signal},
        _signal_name{std::move(other._signal_name)},
        _do_connect{std::move(other._do_connect)},
        _do_connect_lambda{std::move(other._do_connect_lambda)} {}

    QSignalSource(const QSignalSource& other):
        _object{other._object},
        _signal{other._signal},
        _signal_name{other._signal_name},
        _do_connect{other._do_connect},
        _do_connect_lambda{other._do_connect_lambda} {}

    QSignalSource& operator=(const QSignalSource& other) {
        _object = other._object;
        _signal = other._signal;
        _signal_name = other._signal_name;
        _do_connect = other._do_connect;
        _do_connect_lambda = other._do_connect_lambda;
        return *this;
    }

    template <typename Func,
              typename std::enable_if_t<QtPrivate::FunctionPointer<Func>::IsPointerToMemberFunction &&
                                        QtPrivate::FunctionPointer<Func>::ArgumentCount == 0, int> = 0>
    auto do_connect(const typename QtPrivate::FunctionPointer<Func>::Object* object, Func func) const noexcept {
        return _do_connect(object, reinterpret_cast<void**>(&func));
    }

    template <typename Func>
    auto do_connect(Func func) const noexcept {
        return _do_connect_lambda(func);
    }

    /*bool operator==(const QConnectionDesc& rhs) const noexcept {
        return _object == rhs._object &&
                _signal == rhs._signal;
    }*/

    bool operator<(const QSignalSource& rhs) const noexcept {
        if (_object < rhs._object)
            return true;
        else return _signal < rhs._signal;
    }

    friend std::ostream& operator<<(std::ostream& stream, const QSignalSource& src) {
        stream << src._object->metaObject()->className() << "::" << src._signal_name.toStdString();
        return stream;
    }

private:
    const QObject* _object;
    void* _signal;
    QString _signal_name = "undefined";
    std::function<QMetaObject::Connection(const QObject*, void**)> _do_connect;
    std::function<QMetaObject::Connection(std::function<void()>)> _do_connect_lambda;

};
