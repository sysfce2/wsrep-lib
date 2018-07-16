//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#ifndef WSREP_DB_HIGH_PRIORITY_SERVICE_HPP
#define WSREP_DB_HIGH_PRIORITY_SERVICE_HPP

#include "wsrep/high_priority_service.hpp"

namespace db
{
    class server;
    class client;
    class high_priority_service : public wsrep::high_priority_service
    {
    public:
        high_priority_service(db::server& server, db::client& client);
        int start_transaction(const wsrep::ws_handle&,
                              const wsrep::ws_meta&) override;
        const wsrep::transaction& transaction() const override;
        void adopt_transaction(const wsrep::transaction&) override;
        int apply_write_set(const wsrep::ws_meta&,
                            const wsrep::const_buffer&) override;
        int append_fragment_and_commit(
            const wsrep::ws_handle&,
            const wsrep::ws_meta&, const wsrep::const_buffer&)
            override
        { return 0; }
        int remove_fragments(const wsrep::ws_meta&) override
        { return 0; }
        int commit(const wsrep::ws_handle&, const wsrep::ws_meta&) override;
        int rollback(const wsrep::ws_handle&, const wsrep::ws_meta&) override;
        int apply_toi(const wsrep::ws_meta&, const wsrep::const_buffer&) override;
        void after_apply() override;
        void store_globals() override { }
        void reset_globals() override { }
        void switch_execution_context(wsrep::high_priority_service&) override
        { }
        int log_dummy_write_set(const wsrep::ws_handle&,
                                const wsrep::ws_meta&) override
        { return 0; }
        bool is_replaying() const override;
        void debug_crash(const char*) override { }
    private:
        high_priority_service(const high_priority_service&);
        high_priority_service& operator=(const high_priority_service&);
        db::server& server_;
        db::client& client_;
    };
}

#endif // WSREP_DB_HIGH_PRIORITY_SERVICE_HPP
