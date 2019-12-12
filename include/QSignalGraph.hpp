#pragma once

#include <QSignalDetails.hpp>
#include <QSignalConjunction.hpp>
#include <QSignalDisjunction.hpp>

inline QSignalConjunctionPtr operator&(QSignalSource&& lhs, QSignalSource&& rhs) {
    return QSignalConjunction::make(std::set{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
}

inline QSignalDisjunctionPtr operator|(QSignalSource&& lhs, QSignalSource&& rhs) {
    return QSignalDisjunction::make(std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs));
}

namespace qsg::details {
    template <typename T>
    constexpr auto is_junction_v = std::disjunction_v<
        std::is_same<T, QSignalConjunctionPtr>,
        std::is_same<T, QSignalDisjunctionPtr>,
        std::is_same<T, QSignalSource>>;
}

template <typename SuccessJunctionT, typename FailureJunctionT,
          std::enable_if_t<qsg::details::is_junction_v<SuccessJunctionT> && qsg::details::is_junction_v<FailureJunctionT>, int> = 0>
inline QSignalDisjunctionPtr operator||(SuccessJunctionT&& lhs, FailureJunctionT&& rhs) {
    return QSignalDisjunction::make(std::forward<SuccessJunctionT>(lhs), std::forward<FailureJunctionT>(rhs));
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalSource&& rhs) {
    auto disj = QSignalDisjunction::make(std::make_tuple<QSignalSource, QSignalSource>({&*lhs, &QSignalConjunction::done},
                                                                       std::forward<QSignalSource>(rhs)));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalSource&& lhs, QSignalConjunctionPtr&& rhs) {
    auto disj = QSignalDisjunction::make(std::make_tuple<QSignalSource, QSignalSource>(std::forward<QSignalSource>(lhs), {&*rhs, &QSignalConjunction::done}));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(rhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalConjunctionPtr&& rhs) {
    auto disj = QSignalDisjunction::make(std::make_tuple<QSignalSource, QSignalSource>({&*lhs, &QSignalConjunction::done}, {&*rhs, &QSignalConjunction::done}));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(lhs));
    disj->add_ref(std::forward<QSignalConjunctionPtr>(rhs));
    return disj;
}

inline QSignalDisjunctionPtr operator|(QSignalConjunctionPtr&& lhs, QSignalDisjunctionPtr&& rhs) {
    return QSignalDisjunction::make(std::forward< QSignalConjunctionPtr>(lhs), std::forward<QSignalDisjunctionPtr>(rhs));
}
