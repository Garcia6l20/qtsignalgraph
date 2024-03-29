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
template <typename Derived>
class QSignalBaseJunction:
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder {
public:
    using junction_type = Derived;
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
    void on_done(Func func) {
        _done.bind(func);
    }

    void clear() {
        _done.clear();
    }

protected:
    template <typename...Args>
    QSignalBaseJunction(Args&&...args);

    std::set<QSignalSource> _sources;
    std::list<QMetaObject::Connection> _conns;

    OptionnalVariantListenerFunction _done;

    inline void cleanup();

    struct accessor;

private:
    Derived& derived();
};

#include "QSignalBaseJunction.tpp"
