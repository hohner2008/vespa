// Copyright Verizon Media. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "packed_labels.h"
#include <vector>

namespace vespalib::eval {

class PackedMappings {
public:
    using Address = std::vector<vespalib::stringref>;
    using InternalAddress = std::vector<uint32_t>;

    uint32_t size() const { return _num_mappings; }
    uint32_t num_sparse_dims() const { return _num_dims; }

    // returns -1 if mapping does not contain address 
    int32_t subspace_of_address(const Address &address) const;
    int32_t subspace_of_enums(const InternalAddress &address) const;
    int32_t sortid_of_address(const Address &address) const;
    int32_t sortid_of_enums(const InternalAddress &address) const;

    Address address_of_sortid(uint32_t internal_index) const;
    Address address_of_subspace(uint32_t subspace_index) const;

    /** returns sortid */
    uint32_t fill_by_subspace(uint32_t subspace_index, Address &address) const;
    uint32_t fill_by_subspace(uint32_t subspace_index, InternalAddress &address) const;

    /** returns subspace_index */
    uint32_t fill_by_sortid(uint32_t sortid, Address &address) const;
    uint32_t fill_by_sortid(uint32_t sortid, InternalAddress &address) const;

    InternalAddress enums_of_sortid(uint32_t internal_index) const;
    InternalAddress enums_of_subspace(uint32_t subspace_index) const;

    int enums_compare(const uint32_t *a, const uint32_t *b) const {
        for (size_t i = 0; i < _num_dims; ++i) {
            if (a[i] < b[i]) return -1;
            if (a[i] > b[i]) return 1;
        }
        return 0;
    }

    const PackedLabels & label_store() const { return _label_store; }
private:
    PackedMappings(uint32_t num_dims, uint32_t num_mappings,
                   ConstArrayRef<uint32_t> int_store,
                   PackedLabels label_store)
      : _num_dims(num_dims),
        _num_mappings(num_mappings),
        _int_store(int_store),
        _label_store(label_store)
    {
        validate();
    }
    friend class PackedMappingsBuilder;

    void validate() const;

    const uint32_t _num_dims;
    const uint32_t _num_mappings;
    /*
       _int_store contains data corresponding to this model:
       struct IntStore {
           // map to index in next table:
           uint32_t index_of_subspace[num_mappings];
           // sorted lexicographically by label_enums:
           struct MappingData {
               uint32_t label_enums[num_dims];
               uint32_t subspace_index;
           } mappings[num_mappings];
       };
     */
    const ConstArrayRef<uint32_t> _int_store;
    const PackedLabels _label_store;

    uint32_t offset_of_mapping_data(uint32_t idx) const {
        return (idx * (1 + _num_dims)) + _num_mappings;
    }
    uint32_t subspace_of_sortid(uint32_t internal_index) const {
        uint32_t offset = offset_of_mapping_data(internal_index);
        return _int_store[offset + _num_dims];
    }
    uint32_t sortid_of_subspace(uint32_t subspace_index) const {
        return _int_store[subspace_index];
    }
    const uint32_t * ptr_of_sortid(uint32_t internal_index) const {
        return &_int_store[offset_of_mapping_data(internal_index)];
    }
};

} // namespace
