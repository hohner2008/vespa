// Copyright Yahoo. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "compact_buffer_candidates.h"
#include <algorithm>

namespace vespalib::datastore {

CompactBufferCandidates::CompactBufferCandidates(uint32_t num_buffers, uint32_t max_buffers, double ratio, size_t slack)
    : _candidates(),
      _used(0),
      _dead(0),
      _max_buffers(std::max(max_buffers, 1u)),
      _ratio(ratio),
      _slack(slack)
{
    _candidates.reserve(num_buffers);
}

CompactBufferCandidates::~CompactBufferCandidates() = default;

void
CompactBufferCandidates::add(uint32_t buffer_id, size_t used, size_t dead)
{
    _candidates.emplace_back(buffer_id, used, dead);
    _used += used;
    _dead += dead;
}

void
CompactBufferCandidates::select(std::vector<uint32_t>& buffers)
{
    if (_candidates.empty()) {
        return;
    }
    if (_candidates.size() > _max_buffers) {
        std::nth_element(_candidates.begin(), _candidates.begin() + (_max_buffers - 1), _candidates.end());
        _candidates.resize(_max_buffers);
    }
    std::sort(_candidates.begin(), _candidates.end());
    size_t remaining_used = _used;
    size_t remaining_dead = _dead;
    for (auto& candidate : _candidates) {
        buffers.emplace_back(candidate.get_buffer_id());
        remaining_used -= candidate.get_used();
        remaining_dead -= candidate.get_dead();
        if ((remaining_dead < _slack) || (remaining_dead <= remaining_used * _ratio)) {
            break;
        }
    }
}

}
