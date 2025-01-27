/*
 * Copyright (C) 2018-present ScyllaDB
 */

/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once

#include "sstables.hh"
#include "sstable_writer.hh"
#include "sstables_manager.hh"
#include "schema_fwd.hh"
#include "mutation_fragment.hh"
#include "metadata_collector.hh"
#include "mutation_fragment_stream_validator.hh"

namespace sstables {

struct sstable_writer::writer_impl {
    sstable& _sst;
    const schema& _schema;
    const io_priority_class& _pc;
    const sstable_writer_config _cfg;
    // NOTE: _collector and _c_stats are used to generation of statistics file
    // when writing a new sstable.
    metadata_collector _collector;
    column_stats _c_stats;
    mutation_fragment_stream_validating_filter _validator;

    writer_impl(sstable& sst, const schema& schema, const io_priority_class& pc, const sstable_writer_config& cfg)
        : _sst(sst)
        , _schema(schema)
        , _pc(pc)
        , _cfg(cfg)
        , _collector(_schema, sst.get_filename(), sst.manager().get_local_host_id())
        , _validator(format("sstable writer {}", _sst.get_filename()), _schema, _cfg.validation_level)
    {}

    virtual void consume_new_partition(const dht::decorated_key& dk) = 0;
    virtual void consume(tombstone t) = 0;
    virtual stop_iteration consume(static_row&& sr) = 0;
    virtual stop_iteration consume(clustering_row&& cr) = 0;
    virtual stop_iteration consume(range_tombstone&& rt) = 0;
    virtual stop_iteration consume_end_of_partition() = 0;
    virtual void consume_end_of_stream() = 0;
    virtual ~writer_impl() {}
};

}
