#pragma once

#include <QSignalDetails.hpp>
#include <QSignalSource.hpp>

/**
 * @brief Base Junction class
 *
 * @note [CRTP] Curiously recurring template pattern (@see https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
 *
 * CRTP implementation for common Junction stuff.
 *
 */
template <typename Devived>
class QSignalBaseJunction:
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder {
public:
    using junction_type = Devived;
    using pointer_type = typename std::shared_ptr<junction_type>;

    template <typename...Args>
    static pointer_type make(Args...args);

    template <typename FirstJunctionT, typename...JunctionsT>
    void add(FirstJunctionT&&first, JunctionsT&&...rest);

    void add(QSignalSource&&junction);

    template <typename JunctionT>
    void add(JunctionT&&junction);

    QSignalBaseJunction(const QSignalBaseJunction&&) = delete;
    QSignalBaseJunction& operator=(const QSignalBaseJunction&&) = delete;
    QSignalBaseJunction(const QSignalBaseJunction&) = delete;
    QSignalBaseJunction& operator=(const QSignalBaseJunction&) = delete;

    template <typename Func>
    void done(Func func) {
        using func_traits = qsg::details::function_traits<Func>;
        if constexpr (func_traits::arity == 0) {
            _done = [func](QVariant&&) {
                func();
            };
        } else {
            _done = [func](QVariant&&data) {
                func(std::forward<QVariant>(data));
            };
        }
    }

    void clear() {
        _done = nullptr;
    }

protected:
    template <typename...Args>
    QSignalBaseJunction(Args&&...args);

    std::set<QSignalSource> _sources;
    std::list<QMetaObject::Connection> _conns;

    std::function<void(QVariant)> _done;

    inline void cleanup();
};

#include "QSignalBaseJunction.tpp"
