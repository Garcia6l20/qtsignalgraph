#pragma once

#include <QSignalConjunction.hpp>
#include <queue>
#include <variant>

namespace tools {
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

class QSignalGraph: public QObject {
    Q_OBJECT
private:
    using Step = std::variant<QSignalConjunction*, QSignalDisjunction*>;
    std::queue<Step> _steps;

    void add(QSignalConjunction* step) {
        step->setParent(this);
        connect(step, &QSignalConjunction::done, [this, step] {
            checkForDone(step);
        });
        _steps.push(step);
    }
    void add(QSignalDisjunction* step) {
        step->setParent(this);
        connect(step, &QSignalDisjunction::done, [this, step] {
            checkForDone(step);
        });
        connect(step, &QSignalDisjunction::failed, [this] {
            emit failed();
        });
        _steps.push(step);
    }

    template <typename StepType>
    void checkForDone(StepType* step) {
        using T = std::decay_t<decltype(step)>;
        _steps.pop();
        if (_steps.empty())
            emit done();
    }

signals:
    void done();
    void failed();

public:
    QSignalGraph* operator&&(QSignalSource&& rhs) {
        std::visit(tools::overloaded {
            [rhs = std::move(rhs)](QSignalConjunction* conj) mutable {
                conj->add(std::move(rhs));
            },
            [](QSignalDisjunction* /*disj*/) {
                throw std::runtime_error("Not implemented yet");
            },
        }, _steps.back());
        return this;
    }
    QSignalGraph* operator||(QSignalSource&& rhs) {
        std::visit(tools::overloaded {
            [this, rhs = std::move(rhs)](QSignalConjunction* conj) mutable {
                add(new QSignalDisjunction({conj, &QSignalConjunction::done}, std::move(rhs)));
            },
            [this, rhs = std::move(rhs)](QSignalDisjunction* disj) {
                throw std::runtime_error("Not implemented yet");
            },
        }, _steps.back());
        return this;
    }

    friend QSignalGraph* operator&&(QSignalSource&& lhs, QSignalSource&& rhs);
    friend QSignalGraph* operator||(QSignalSource&& lhs, QSignalSource&& rhs);
};

inline QSignalGraph* operator&&(QSignalSource&& lhs, QSignalSource&& rhs) {
    QSignalGraph* graph = new QSignalGraph;
    graph->add(new QSignalConjunction{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
    return graph;
}
inline QSignalGraph* operator&&(QSignalGraph* graph, QSignalSource&& rhs) {
    graph->operator&&(std::forward<QSignalSource>(rhs));
    return graph;
}
inline QSignalGraph* operator||(QSignalSource&& lhs, QSignalSource&& rhs) {
    QSignalGraph* graph = new QSignalGraph;
    graph->add(new QSignalDisjunction{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
    return graph;
}
inline QSignalGraph* operator||(QSignalGraph* graph, QSignalSource&& rhs) {
    graph->operator||(std::forward<QSignalSource>(rhs));
    return graph;
}
