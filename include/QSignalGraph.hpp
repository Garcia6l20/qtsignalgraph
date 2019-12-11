#pragma once

#include <QSignalConjunction.hpp>
#include <QDebug>
#include <deque>
#include <variant>

namespace tools {
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

class QSignalGraph: public QObject {
    Q_OBJECT
private:
    struct Step {
        std::variant<QSignalConjunction*, QSignalDisjunction*> junction;
        std::list<QMetaObject::Connection> connections = {};
    };
    std::deque<Step> _steps;

    void add(QSignalConjunction* conj) {
        conj->setParent(this);
        Step step {conj};
        step.connections.push_back(connect(conj, &QSignalConjunction::done, [this, conj] {
            checkForDone(conj);
        }));
        _steps.emplace_back(std::move(step));
    }
    void deleteFrontStep() {
        std::visit(tools::overloaded{
            [step = _steps.front()](auto& item) {
                for (auto& conn: step.connections) {
                    QObject::disconnect(conn);
                }
                item->deleteLater();
                // delete item;
            },
        }, _steps.front().junction);
        _steps.pop_front();
    }
    void add(QSignalDisjunction* disj) {
        disj->setParent(this);
        Step step {disj};
        step.connections.push_back(connect(disj, &QSignalDisjunction::done, [this, disj] {
            checkForDone(disj);
        }));
        step.connections.push_back(connect(disj, &QSignalDisjunction::failed, [this, disj] {
            for (auto& s: _steps) {
                auto br = std::visit(tools::overloaded{
                    [&s, this](QSignalDisjunction* disj) {
                        if (auto cur = std::get_if<QSignalDisjunction*>(&s.junction); cur && *cur == disj) {
                            return true;
                        }
                        deleteFrontStep();
                        return false;
                    },
                    [this](QSignalConjunction*) {
                        deleteFrontStep();
                        return false;
                    }
                }, s.junction);
                if (br) {
                    break;
                }
            }
            deleteFrontStep();
            emit failed();
        }));
        _steps.emplace_back(std::move(step));
    }

    template <typename StepType>
    void checkForDone(StepType* step) {
        using T = std::decay_t<decltype(step)>;
        deleteFrontStep();
        if (_steps.empty())
            emit done();
    }

signals:
    void done();
    void failed();

public:

    friend std::ostream& operator<<(std::ostream& stream, const QSignalGraph* graph) {
        auto prev = graph->_steps.begin();
        auto it = prev + 1;
        auto end = graph->_steps.end();
        std::visit(tools::overloaded{
            [&stream](auto item) {
                stream << *item;
            }
        }, prev->junction);
        for (; it != end; ++it, ++prev) {
            std::visit(tools::overloaded{
               [&stream](const QSignalConjunction*) {
                   stream << " & ";
               },
               [&stream](const QSignalDisjunction*) {
                   stream << " | ";
               },
            }, prev->junction);
            std::visit(tools::overloaded{
               [&stream](auto item) {
                   stream << *item;
               }
            }, it->junction);
        }
        return stream;
    }
    friend std::ostream& operator<<(std::ostream& stream, const QSignalGraph& graph) {
        stream << &graph;
        return stream;
    }

    QSignalGraph* operator&(QSignalSource&& rhs) {
        std::visit(tools::overloaded {
            [rhs = std::move(rhs)](QSignalConjunction* conj) mutable {
                conj->add(std::move(rhs));
            },
            [](QSignalDisjunction* /*disj*/) {
                throw std::runtime_error("Not implemented yet");
            },
        }, _steps.back().junction);
        return this;
    }
    QSignalGraph* operator|(QSignalSource&& rhs) {
        std::visit(tools::overloaded {
            [this, rhs = std::move(rhs)](QSignalConjunction* conj) mutable {
                add(new QSignalDisjunction({conj, &QSignalConjunction::done}, std::move(rhs)));
            },
            [this, rhs = std::move(rhs)](QSignalDisjunction* disj) {
                throw std::runtime_error("Not implemented yet");
            },
        }, _steps.back().junction);
        return this;
    }

    friend QSignalGraph* operator&(QSignalSource&& lhs, QSignalSource&& rhs);
    friend QSignalGraph* operator|(QSignalSource&& lhs, QSignalSource&& rhs);
};

inline QSignalGraph* operator&(QSignalSource&& lhs, QSignalSource&& rhs) {
    QSignalGraph* graph = new QSignalGraph;
    graph->add(new QSignalConjunction{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
    return graph;
}
inline QSignalGraph* operator&(QSignalGraph* graph, QSignalSource&& rhs) {
    graph->operator&(std::forward<QSignalSource>(rhs));
    return graph;
}
inline QSignalGraph* operator|(QSignalSource&& lhs, QSignalSource&& rhs) {
    QSignalGraph* graph = new QSignalGraph;
    graph->add(new QSignalDisjunction{std::forward<QSignalSource>(lhs), std::forward<QSignalSource>(rhs)});
    return graph;
}
inline QSignalGraph* operator|(QSignalGraph* graph, QSignalSource&& rhs) {
    graph->operator|(std::forward<QSignalSource>(rhs));
    return graph;
}
