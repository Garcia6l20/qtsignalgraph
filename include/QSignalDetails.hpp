#pragma once

#include <QObject>
#include <QMetaObject>
#include <QVariant>

#include <memory>
#include <vector>
#include <set>
#include <details/function_traits.hpp>

namespace qsg::details {

    struct shared_ref_holder {
        template <typename T>
        void add_ref(std::shared_ptr<T>&& ref) {
            _refs.emplace(std::forward<std::shared_ptr<T>>(ref));
        }
        void clear_refs() {
            _refs.clear();
        }
    private:
        std::set<std::shared_ptr<void>> _refs;
    };

    struct auto_cleaned_connection_holder {
        ~auto_cleaned_connection_holder() {
            auto_clean_connections();
        }
        void auto_clean_connections() {
            for (auto& conn : _auto_cleaned_connections)
                QObject::disconnect(conn);
            _auto_cleaned_connections.clear();
        }
        void add_auto_clean_connection(QMetaObject::Connection conn) {
            _auto_cleaned_connections.emplace_back(std::move(conn));
        }
        std::vector<QMetaObject::Connection> _auto_cleaned_connections;
    };
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

    template <typename Class>
    struct make_shared_enabler : public Class {
        template <typename... Args>
        make_shared_enabler(Args &&... args) :
            Class(std::forward<Args>(args)...)
        {}
    };
}

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v
    <T, std::void_t<decltype(sizeof(T))>> = true;

class OptionnalVariantListenerFunction {
public:
    template <typename Func>
    void bind(Func func) {
        using func_traits = qsg::details::function_traits<Func>;
        /*if constexpr (!is_type_complete_v<func_traits>) {
            static_assert (false, "unimplemented");
            _func = [func](QVariant&& data) {
                func(data.toSt);
            };
        } else */if constexpr (func_traits::arity == 0) {
            _func = [func](QVariant&&) {
                func();
            };
        } else if constexpr (std::is_same_v<typename func_traits::template arg<0>::clean_type, QVariant>) {
            _func = [func](QVariant&& data) {
                func(std::forward<QVariant>(data));
            };
        } else {
            _func = [func](QVariant&& data) {
                func(std::forward<QVariant>(data).value<typename func_traits::template arg<0>::clean_type>());
            };
        }
    }
    void clear() {
        _func = nullptr;
    }
    void operator()(QVariant&& data) {
        if (_func)
            _func(std::forward<QVariant>(data));
    }
    operator bool() {
        return _func != nullptr;
    }
private:
    std::function<void(QVariant)> _func;
};

