//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#ifndef WSREP_TEST_CLIENT_CONTEXT_FIXTURE_HPP
#define WSREP_TEST_CLIENT_CONTEXT_FIXTURE_HPP

#include "fake_server_context.hpp"
#include "fake_client_context.hpp"


#include <boost/test/unit_test.hpp>

namespace
{
    struct replicating_client_fixture_sync_rm
    {
        replicating_client_fixture_sync_rm()
            : sc("s1", "s1", wsrep::server_context::rm_sync)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct replicating_client_fixture_async_rm
    {
        replicating_client_fixture_async_rm()
            : sc("s1", "s1", wsrep::server_context::rm_async)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct replicating_client_fixture_autocommit
    {
        replicating_client_fixture_autocommit()
            : sc("s1", "s1", wsrep::server_context::rm_sync)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating, true)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct applying_client_fixture
    {
        applying_client_fixture()
            : sc("s1", "s1",
                 wsrep::server_context::rm_async)
            , cc(sc,
                 wsrep::client_id(1),
                 wsrep::client_context::m_applier)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            wsrep::ws_handle ws_handle(1, (void*)1);
            wsrep::ws_meta ws_meta(wsrep::gtid(wsrep::id("1"), wsrep::seqno(1)),
                                   wsrep::stid(sc.id(), 1, cc.id()),
                                   wsrep::seqno(0),
                                   wsrep::provider::flag::start_transaction |
                                   wsrep::provider::flag::commit);
            BOOST_REQUIRE(cc.start_transaction(ws_handle, ws_meta) == 0);
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(cc.start_transaction() == 0);
            BOOST_REQUIRE(tc.active() == true);
            BOOST_REQUIRE(tc.certified() == true);
            BOOST_REQUIRE(tc.ordered() == true);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct streaming_client_fixture_row
    {
        streaming_client_fixture_row()
            : sc("s1", "s1", wsrep::server_context::rm_sync)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
            cc.enable_streaming(wsrep::transaction_context::streaming_context::row, 1);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct streaming_client_fixture_byte
    {
        streaming_client_fixture_byte()
            : sc("s1", "s1", wsrep::server_context::rm_sync)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
            cc.enable_streaming(wsrep::transaction_context::streaming_context::row, 1);
        }
        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };

    struct streaming_client_fixture_statement
    {
        streaming_client_fixture_statement()
            : sc("s1", "s1", wsrep::server_context::rm_sync)
            , cc(sc, wsrep::client_id(1),
                 wsrep::client_context::m_replicating)
            , tc(cc.transaction())
        {
            BOOST_REQUIRE(cc.before_command() == 0);
            BOOST_REQUIRE(cc.before_statement() == 0);
            // Verify initial state
            BOOST_REQUIRE(tc.active() == false);
            BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
            cc.enable_streaming(wsrep::transaction_context::streaming_context::row, 1);
        }

        wsrep::fake_server_context sc;
        wsrep::fake_client_context cc;
        const wsrep::transaction_context& tc;
    };
}
#endif // WSREP_TEST_CLIENT_CONTEXT_FIXTURE_HPP