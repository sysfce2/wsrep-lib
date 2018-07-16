//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

/** @file high_priority_service.hpp
 *
 * Interface for services for applying high priority transactions.
 */
#ifndef WSREP_HIGH_PRIORITY_SERVICE_HPP
#define WSREP_HIGH_PRIORITY_SERVICE_HPP

#include "server_state.hpp"

namespace wsrep
{
    class ws_handle;
    class ws_meta;
    class const_buffer;
    class transaction;
    class high_priority_service
    {
    public:
        high_priority_service(wsrep::server_state& server_state)
            : server_state_(server_state)
            , must_exit_() { }
        virtual ~high_priority_service() { }

        int apply(const ws_handle& ws_handle, const ws_meta& ws_meta,
                          const const_buffer& data)
        {
            return server_state_.on_apply(*this, ws_handle, ws_meta, data);
        }
        /**
         * Start a new transaction
         */
        virtual int start_transaction(const wsrep::ws_handle&,
                                      const wsrep::ws_meta&) = 0;

        /**
         * Return transaction object associated to high priority
         * service state.
         */
        virtual const wsrep::transaction& transaction() const = 0;

        /**
         * Adopt a transaction.
         */
        virtual void adopt_transaction(const wsrep::transaction&) = 0;

        /**
         * Apply a write set.
         *
         * A write set applying happens always
         * as a part of the transaction. The caller must start a
         * new transaction before applying a write set and must
         * either commit to make changes persistent or roll back.
         *
         */
        virtual int apply_write_set(const wsrep::ws_meta&,
                                    const wsrep::const_buffer&) = 0;

        /**
         * Append a fragment into fragment storage. This will be
         * called after a non-committing fragment belonging to
         * streaming transaction has been applied. The call will
         * not be done within an open transaction, the implementation
         * must start a new transaction and commit.
         *
         * Note that the call is not done from streaming transaction
         * context, but from applier context.
         */
        virtual int append_fragment_and_commit(
            const wsrep::ws_handle& ws_handle,
            const wsrep::ws_meta& ws_meta,
            const wsrep::const_buffer& data) = 0;

        /**
         * Remove fragments belonging to streaming transaction.
         * This method will be called within the streaming transaction
         * before commit. The implementation must not commit the
         * whole transaction. The call will be done from streaming
         * transaction context.
         *
         * @param ws_meta Write set meta data for commit fragment.
         *
         * @return Zero on success, non-zero on failure.
         */
        virtual int remove_fragments(const wsrep::ws_meta& ws_meta) = 0;

        /**
         * Commit a transaction.
         * An implementation must call
         * wsrep::client_state::prepare_for_ordering() to set
         * the ws_handle and ws_meta before the commit if the
         * commit process will go through client state commit
         * processing. Otherwise the implementation must release
         * commit order explicitly via provider.
         *
         * @param ws_handle Write set handle
         * @param ws_meta Write set meta
         *
         * @return Zero in case of success, non-zero in case of failure
         */
        virtual int commit(const wsrep::ws_handle& ws_handle,
                           const wsrep::ws_meta& ws_meta) = 0;
        /**
         * Roll back a transaction
         *
         * An implementation must call
         * wsrep::client_state::prepare_for_ordering() to set
         * the ws_handle and ws_meta before the rollback if
         * the rollback process will go through client state
         * rollback processing. Otherwise the implementation
         * must release commit order explicitly via provider.
         *
         * @param ws_handle Write set handle
         * @param ws_meta Write set meta
         *
         * @return Zero in case of success, non-zero in case of failure
         */
        virtual int rollback(const wsrep::ws_handle& ws_handle,
                             const wsrep::ws_meta& ws_meta) = 0;

        /**
         * Apply a TOI operation.
         *
         * TOI operation is a standalone operation and should not
         * be executed as a part of a transaction.
         */
        virtual int apply_toi(const wsrep::ws_meta&,
                              const wsrep::const_buffer&) = 0;

        /**
         * Actions to take after applying a write set was completed.
         */
        virtual void after_apply() = 0;

        /**
         * Store global execution context for high priority service.
         */
        virtual void store_globals() = 0;

        /**
         * Reset global execution context for high priority service.
         */
        virtual void reset_globals() = 0;

        /**
         * Switch exection context to context of orig_hps.
         *
         * @param orig_hps Original high priority service.
         */
        virtual void switch_execution_context(
            wsrep::high_priority_service& orig_hps) = 0;

        /**
         * Log a dummy write set which is either SR transaction fragment
         * or roll back fragment. The implementation must release
         * commit order inside the call.
         *
         * @params ws_handle Write set handle
         * @params ws_meta Write set meta data
         *
         * @return Zero in case of success, non-zero on failure
         */
        virtual int log_dummy_write_set(const ws_handle& ws_handle,
                                        const ws_meta& ws_meta) = 0;

        virtual bool is_replaying() const = 0;

        bool must_exit() const { return must_exit_; }

        /**
         * Debug facility to crash the server at given point.
         */
        virtual void debug_crash(const char* crash_point) = 0;
    protected:
        wsrep::server_state& server_state_;
        bool must_exit_;
    };

    class high_priority_switch
    {
    public:
        high_priority_switch(high_priority_service& orig_service,
                             high_priority_service& current_service)
            : orig_service_(orig_service)
            , current_service_(current_service)
        {
            orig_service_.reset_globals();
            current_service_.switch_execution_context(orig_service_);
            current_service_.store_globals();
        }
        ~high_priority_switch()
        {
            current_service_.reset_globals();
            orig_service_.store_globals();
        }
    private:
        high_priority_service& orig_service_;
        high_priority_service& current_service_;
    };
}

#endif // WSREP_HIGH_PRIORITY_SERVICE_HPP