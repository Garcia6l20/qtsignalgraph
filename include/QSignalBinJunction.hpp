#pragma once

#include <QObject>
#include <QSignalDetails.hpp>
#include <QSignalSource.hpp>
#include <memory>
#include <variant>

class QSignalDisjunction;
using QSignalDisjunctionPtr = std::shared_ptr<QSignalDisjunction>;
class QSignalConjunction;
using QSignalConjunctionPtr = std::shared_ptr<QSignalConjunction>;
class QSignalBinJunction;
using QSignalBinJunctionPtr = std::shared_ptr<QSignalBinJunction>;

class QSignalBinJunction: public QObject,
        public qsg::details::shared_ref_holder,
        public qsg::details::auto_cleaned_connection_holder  {
    Q_OBJECT
public:

    /**
     * @brief Create a signal disjunction
     * @param args... Disjuntion arguments
     * @tparam Args... Disjuntion argument types
     * @return The new disjunction
     */
    template <typename...Args>
    static QSignalBinJunctionPtr make(Args...args);

    template <typename Func>
    void on_done(Func func) {
        _done.bind(func);
    }

    template <typename Func>
    void on_failed(Func func) {
        _failed.bind(func);
    }
protected:
    OptionnalVariantListenerFunction _done;
    OptionnalVariantListenerFunction _failed;

    QSignalBinJunction(QSignalSource&&true_src, QSignalSource&& false_src);

    template <typename TrueJunctionT>
    QSignalBinJunction(TrueJunctionT&&true_j, QSignalSource&&false_src);

    template <typename FalseJunctionT>
    QSignalBinJunction(QSignalSource&&true_src, FalseJunctionT&& false_j);

    template <class TrueJunctionT, class FalseJunctionT>
    QSignalBinJunction(TrueJunctionT&&, FalseJunctionT&&);


    QSignalBinJunction(const QSignalBinJunction&&) = delete;
    QSignalBinJunction& operator=(const QSignalBinJunction&&) = delete;
    QSignalBinJunction(const QSignalBinJunction&) = delete;
    QSignalBinJunction& operator=(const QSignalBinJunction&) = delete;

private:
    inline void do_connect();
    void cleanup();
    using source_variant = std::variant<QSignalSource, QSignalConjunctionPtr, QSignalDisjunctionPtr>;
    source_variant _trueSource;
    source_variant _falseSource;
    QMetaObject::Connection _trueConn;
    QMetaObject::Connection _falseConn;

    // frendship
    friend std::ostream& operator<<(std::ostream& stream, const QSignalBinJunction& disj);
};

#include "QSignalBinJunction.tpp"
