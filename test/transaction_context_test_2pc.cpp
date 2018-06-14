//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#include "client_context_fixture.hpp"

//
// Test a succesful 2PC transaction lifecycle
//
BOOST_FIXTURE_TEST_CASE(transaction_context_2pc,
                        replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_preparing);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    BOOST_REQUIRE(cc.before_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    BOOST_REQUIRE(cc.ordered_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_ordered_commit);
    BOOST_REQUIRE(cc.after_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committed);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error() == wsrep::e_success);
}

//
// Test a 2PC transaction which gets BF aborted before before_prepare
//
BOOST_FIXTURE_TEST_CASE(
    transaction_context_2pc_bf_before_before_prepare,
    replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    wsrep_test::bf_abort_unordered(cc);
    BOOST_REQUIRE(cc.before_prepare());
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_abort);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(cc.before_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_aborting);
    BOOST_REQUIRE(cc.after_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_aborted);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_error);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error());
}

//
// Test a 2PC transaction which gets BF aborted before before_prepare
//
BOOST_FIXTURE_TEST_CASE(
    transaction_context_2pc_bf_before_after_prepare,
    replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_preparing);
    wsrep_test::bf_abort_unordered(cc);
    BOOST_REQUIRE(cc.after_prepare());
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_abort);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(cc.before_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_aborting);
    BOOST_REQUIRE(cc.after_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_aborted);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_error);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error());
}

//
// Test a 2PC transaction which gets BF aborted after_prepare() and
// the rollback takes place before entering before_commit().
//
BOOST_FIXTURE_TEST_CASE(
    transaction_context_2pc_bf_after_after_prepare,
    replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    wsrep_test::bf_abort_ordered(cc);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_abort);
    BOOST_REQUIRE(cc.before_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error() == wsrep::e_success);
}

//
// Test a 2PC transaction which gets BF aborted between after_prepare()
// and before_commit()
//
BOOST_FIXTURE_TEST_CASE(
    transaction_context_2pc_bf_before_before_commit,
    replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    wsrep_test::bf_abort_ordered(cc);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_abort);
    BOOST_REQUIRE(cc.before_commit());
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(tc.certified() == true);
    BOOST_REQUIRE(tc.ordered() == true);
    BOOST_REQUIRE(cc.before_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error() == wsrep::e_success);
}


//
// Test a 2PC transaction which gets BF aborted when trying to grab
// commit order.
//
BOOST_FIXTURE_TEST_CASE(
    transaction_context_2pc_bf_during_commit_order_enter,
    replicating_client_fixture_2pc)
{
    cc.start_transaction(1);
    BOOST_REQUIRE(tc.active());
    BOOST_REQUIRE(tc.id() == wsrep::transaction_id(1));
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_executing);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    sc.provider().commit_order_enter_result_ = wsrep::provider::error_bf_abort;
    BOOST_REQUIRE(cc.before_commit());
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(tc.certified() == true);
    BOOST_REQUIRE(tc.ordered() == true);
    sc.provider().commit_order_enter_result_ = wsrep::provider::success;
    BOOST_REQUIRE(cc.before_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_rollback() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_must_replay);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(tc.ordered() == false);
    BOOST_REQUIRE(tc.certified() == false);
    BOOST_REQUIRE(cc.current_error() == wsrep::e_success);
}

///////////////////////////////////////////////////////////////////////////////
//                       STREAMING REPLICATION                               //
///////////////////////////////////////////////////////////////////////////////


BOOST_FIXTURE_TEST_CASE(transaction_context_streaming_2pc_commit,
                        streaming_client_fixture_row)
{
    BOOST_REQUIRE(cc.start_transaction(1) == 0);
    BOOST_REQUIRE(cc.after_row() == 0);
    BOOST_REQUIRE(tc.streaming_context_.fragments_certified() == 1);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(cc.before_commit() == 0);
    BOOST_REQUIRE(cc.ordered_commit() == 0);
    BOOST_REQUIRE(cc.after_commit() == 0);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(sc.provider().fragments() == 2);
    BOOST_REQUIRE(sc.provider().start_fragments() == 1);
    BOOST_REQUIRE(sc.provider().commit_fragments() == 1);
}

BOOST_FIXTURE_TEST_CASE(transaction_context_streaming_2pc_commit_two_statements,
                        streaming_client_fixture_row)
{
    BOOST_REQUIRE(cc.start_transaction(1) == 0);
    BOOST_REQUIRE(cc.after_row() == 0);
    BOOST_REQUIRE(tc.streaming_context_.fragments_certified() == 1);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(cc.before_statement() == 0);
    BOOST_REQUIRE(cc.after_row() == 0);
    BOOST_REQUIRE(tc.streaming_context_.fragments_certified() == 2);
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(cc.before_commit() == 0);
    BOOST_REQUIRE(cc.ordered_commit() == 0);
    BOOST_REQUIRE(cc.after_commit() == 0);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(sc.provider().fragments() == 3);
    BOOST_REQUIRE(sc.provider().start_fragments() == 1);
    BOOST_REQUIRE(sc.provider().commit_fragments() == 1);
}

///////////////////////////////////////////////////////////////////////////////
//                              APPLYING                                     //
///////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_CASE(transaction_context_2pc_applying,
                        applying_client_fixture_2pc)
{
    BOOST_REQUIRE(cc.before_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_preparing);
    BOOST_REQUIRE(cc.after_prepare() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    BOOST_REQUIRE(cc.before_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committing);
    BOOST_REQUIRE(cc.ordered_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_ordered_commit);
    BOOST_REQUIRE(cc.after_commit() == 0);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committed);
    BOOST_REQUIRE(cc.after_statement() == wsrep::client_context::asr_success);
    BOOST_REQUIRE(tc.state() == wsrep::transaction_context::s_committed);
    BOOST_REQUIRE(tc.active() == false);
    BOOST_REQUIRE(cc.current_error() == wsrep::e_success);
}
