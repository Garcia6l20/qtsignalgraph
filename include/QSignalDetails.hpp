#pragma once

#include <QObject>
#include <QMetaObject>
#include <QVariant>

#include <memory>
#include <vector>
#include <set>
#include <functional>

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

    // function traits
    namespace function_detail
    {
        template<typename Ret, typename Cls, typename IsMutable, typename IsLambda, typename... Args>
        struct types
        {
            using is_mutable = IsMutable;
            using is_lambda = IsLambda;
            static constexpr auto is_function() { return !is_lambda(); }

            enum { arity = sizeof...(Args) };

            using return_type = Ret;

            template<size_t i>
            struct arg
            {
                using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
                using clean_type = std::remove_cv_t<std::remove_reference_t<type>>;
            };

            using function_type = std::function<Ret(Args...)>;

            struct parameters_tuple_all_enabled {
                template <typename T>
                static constexpr bool enabled = false;
            };

            template <typename...ArgsT>
            struct parameters_tuple_disable {
                template <typename T>
                static constexpr bool enabled = !std::disjunction_v<std::is_same<std::remove_cv_t<std::remove_reference_t<T>>,
                                                                                 std::remove_cv_t<std::remove_reference_t<ArgsT>>>...>;
            };

            template <typename Predicate = parameters_tuple_all_enabled>
            struct parameters_tuple {

                template <typename FirstT, typename...RestT>
                static constexpr auto __make_tuple() {
                    if constexpr (!sizeof...(RestT)) {
                        if constexpr (Predicate::template enabled<FirstT>)
                            return std::make_tuple<FirstT>({});
                        else return std::tuple();
                    }
                    else {
                        if constexpr (Predicate::template enabled<FirstT>)
                            return std::tuple_cat(std::make_tuple<FirstT>({}), __make_tuple<RestT...>());
                        else return __make_tuple<RestT...>();
                    }
                }

                struct _make {
                    constexpr auto operator()() {
                        if constexpr (sizeof...(Args) == 0)
                            return std::make_tuple();
                        else return __make_tuple<Args...>();
                    }
                };

                static constexpr auto make() {
                    return _make{}();
                }

                using tuple_type = std::invoke_result_t<_make>;
            };
        };
    }

    template<class T>
    struct function_traits
        : function_traits<decltype(&std::remove_cv_t<std::remove_reference_t<T>>::operator())>
    {};

    // mutable lambda
    template<class Ret, class Cls, class... Args>
    struct function_traits<Ret(Cls::*)(Args...)>
        : function_detail::types<Ret, Cls, std::true_type, std::true_type, Args...>
    {};

    // immutable lambda
    template<class Ret, class Cls, class... Args>
    struct function_traits<Ret(Cls::*)(Args...) const>
        : function_detail::types<Ret, Cls, std::false_type, std::true_type, Args...>
    {};

    // function
    template<class Ret, class... Args>
    struct function_traits<std::function<Ret(Args...)>>
        : function_detail::types<Ret, std::nullptr_t, std::true_type, std::false_type, Args...>
    {};
}


class OptionnalVariantListenerFunction {
public:
    template <typename Func>
    void bind(Func func) {
        using func_traits = qsg::details::function_traits<Func>;
        if constexpr (func_traits::arity == 0) {
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

