#pragma once

#include <QMetaObject>

#include <memory>
#include <vector>
#include <set>

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
    template <typename Derived>
    struct auto_cleaned_connection_holder {
        ~auto_cleaned_connection_holder() {
            auto_clean_connections();
        }
        void auto_clean_connections() {
            for (auto& conn : _auto_cleaned_connections)
                static_cast<Derived*>(this)->disconnect(conn);
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
