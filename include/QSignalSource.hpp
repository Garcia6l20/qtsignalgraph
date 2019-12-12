#pragma once

#include <QObject>
#include <QMetaMethod>
#include <QDebug>

class QSignalSource {
    typedef void(QObject::* const UnaryFunc)();
    
    template<typename T, typename R, typename...Args>
    void* member_void_cast(R(T::* f)(Args...))
    {
        union
        {
            R(T::* pf)(Args...);
            void* p;
        };
        pf = f;
        return p;
    }

public:

    template <typename Func,
              typename std::enable_if_t<QtPrivate::FunctionPointer<Func>::IsPointerToMemberFunction/* &&
                                        QtPrivate::FunctionPointer<Func>::ArgumentCount == 0*/, int> = 0>
    QSignalSource(const typename QtPrivate::FunctionPointer<Func>::Object* object, Func func):
        _object{object},
        _signal{member_void_cast(func)},
        _metaMethod{QMetaMethod::fromSignal(func)}
    {
        _do_connect = [object, func] (const QObject* target, void** target_func) {
            return QObject::connect(object, func, target, *reinterpret_cast<UnaryFunc*>(target_func));
        };
        if constexpr (QtPrivate::FunctionPointer<Func>::ArgumentCount > 0) {
            _do_connect_lambda = [object, func] (std::function<void(QVariant)> target_func) {
                return QObject::connect(object, func, target_func);
            };
        } else {
            _do_connect_lambda = [object, func](std::function<void(QVariant)> target_func) {
                return QObject::connect(object, func, [target_func] {
                    target_func({});
                });
            };
        }
    }

    QSignalSource(QSignalSource&& other) noexcept:
        _object{other._object},
        _signal{other._signal},
        _metaMethod{std::move(other._metaMethod)},
        _do_connect{std::move(other._do_connect)},
        _do_connect_lambda{std::move(other._do_connect_lambda)} {}

    QSignalSource(const QSignalSource& other) noexcept:
        _object{other._object},
        _signal{other._signal},
        _metaMethod{other._metaMethod},
        _do_connect{other._do_connect},
        _do_connect_lambda{other._do_connect_lambda} {}

    QSignalSource& operator=(const QSignalSource& other) noexcept {
        _object = other._object;
        _signal = other._signal;
        _metaMethod = other._metaMethod;
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

    const QObject* object() const { return _object; }
    const QMetaMethod& metaMethod() const { return _metaMethod; }

    friend std::ostream& operator<<(std::ostream& stream, const QSignalSource& src) {
        stream << src._object->metaObject()->className() << "::" << src._metaMethod.name().toStdString();
        return stream;
    }
private:
    const QObject* _object;
    void* _signal;
    QMetaMethod _metaMethod;
    std::function<QMetaObject::Connection(const QObject*, void**)> _do_connect;
    std::function<QMetaObject::Connection(std::function<void(QVariant)>)> _do_connect_lambda;

};
