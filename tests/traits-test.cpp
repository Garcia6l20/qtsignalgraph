#include <QSignalGraph.hpp>
#include <QCoreApplication>
#include <QTest>

#include <iostream>
#include <optional>

#include "TestObject.hpp"

using namespace qsg::details;

template <typename T>
struct mem_op_arity;

#define ARITY_REM_CTOR(...) __VA_ARGS__
#define ARITY_SPEC(cv, var)                                   \
template <typename C, typename R, typename... Args>           \
struct mem_op_arity<R (C::*) (Args... ARITY_REM_CTOR var) cv> : std::integral_constant<std::size_t, sizeof...(Args)> {};

ARITY_SPEC(const, (,...))
ARITY_SPEC(const, ())
ARITY_SPEC(, (,...))
ARITY_SPEC(, ())

#undef ARITY_SPEC

namespace detail {
    struct arbitrary {template <typename T> operator T&&();};
    enum { max_arity = 32 };
    template <std::size_t>
    using ignore = arbitrary;

    template <typename L, std::size_t... Is,
              typename U = decltype(std::declval<L>()(ignore<Is>{}...))>
    constexpr auto try_args(std::index_sequence<Is...>) {return sizeof...(Is);}

    template <std::size_t I, typename L>
    constexpr auto arity_temp(int)
      -> decltype(try_args<L>(std::make_index_sequence<I>{})) {
        return try_args<L>(std::make_index_sequence<I>{});}
    template <std::size_t I, typename L>
    constexpr std::enable_if_t<(I == max_arity), std::size_t> arity_temp(...) {
        return -1;}
    template <std::size_t I, typename L>
    constexpr std::enable_if_t<(I < max_arity), std::size_t> arity_temp(...) {
        return arity_temp<I+1, L>(0);}

    template <typename L>
    constexpr typename mem_op_arity<L>::value_type arity(int) {return mem_op_arity<L>::value;}
    template <typename L>
    constexpr auto arity(...) {return arity_temp<0, L>(0);}
}

template <typename L>
constexpr std::size_t arity(L&& l) {return detail::arity<std::decay_t<L>>(0);}

class QTraitsTest: public QObject {
    Q_OBJECT
private:
private slots:
    void nominal() {
        {
            auto lambda = []() {};
            using ftraits = function_traits<decltype(lambda)>;
            static_assert (ftraits::arity == 0);
            static_assert (std::is_same_v<ftraits::return_type, void>);
        }

        {
            auto lambda = [](int) {};
            using ftraits = function_traits<decltype(lambda)>;
            static_assert (ftraits::arity == 1);
            static_assert (std::is_same_v<ftraits::arg<0>::type, int>);
            static_assert (std::is_same_v<ftraits::return_type, void>);
        }

        {
            auto lambda = [](int&) {};
            using ftraits = function_traits<decltype(lambda)>;
            static_assert (ftraits::arity == 1);
            static_assert (std::is_same_v<ftraits::arg<0>::type, int&>);
            static_assert (std::is_same_v<ftraits::arg<0>::clean_type, int>);
            static_assert (std::is_same_v<ftraits::return_type, void>);
        }

        {
            auto lambda = [](const int&) {};
            using ftraits = function_traits<decltype(lambda)>;
            static_assert (ftraits::arity == 1);
            static_assert (std::is_same_v<ftraits::arg<0>::type, const int&>);
            static_assert (std::is_same_v<ftraits::arg<0>::clean_type, int>);
            static_assert (std::is_same_v<ftraits::return_type, void>);
        }

        {
            auto lambda = [](int&& i) { return std::forward<int>(i); };
            using ftraits = function_traits<decltype(lambda)>;
            static_assert (ftraits::arity == 1);
            static_assert (std::is_same_v<ftraits::arg<0>::type, int&&>);
            static_assert (std::is_same_v<ftraits::arg<0>::clean_type, int>);
            static_assert (std::is_same_v<ftraits::return_type, int>);
        }

        {
            auto lambda = [](auto i) { return std::forward<int>(i); };
            using ftraits = function_traits<decltype(lambda)>;
            using LType = std::remove_cv_t<std::remove_reference_t<decltype(lambda)>>;
            using CType = decltype(&LType::operator());
            static_assert (arity(lambda) == 1);
            static_assert (ftraits::arity == 1);
            static_assert (ftraits::arity == 1);
            static_assert (std::is_same_v<ftraits::arg<0>::type, int&&>);
            static_assert (std::is_same_v<ftraits::arg<0>::clean_type, int>);
            static_assert (std::is_same_v<ftraits::return_type, int>);
        }
    }
};


QTEST_MAIN(QTraitsTest)
#include "traits-test.moc"
