// Copyright Verizon Media. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include <vespa/eval/eval/aggr.h>
#include <vespa/eval/eval/operation.h>
#include <vespa/eval/eval/tensor_spec.h>
#include <vespa/eval/eval/value_type.h>

#include <vector>
#include <map>
#include <variant>

namespace vespalib::eval {

struct ReferenceOperations {
    using map_fun_t = vespalib::eval::operation::op1_t;
    using join_fun_t = vespalib::eval::operation::op2_t;

    // for create: mapping from cell address to index of child that computes the cell value
    using CreateSpec = std::map<TensorSpec::Address, size_t>;

    // for Peek: a verbatim label or the index of a child that computes the label value
    using LabelOrChildIndex = std::variant<TensorSpec::Label, size_t>;
    // for Peek: mapping from dimension name to verbatim label or child
    using PeekSpec = std::map<vespalib::string, LabelOrChildIndex>;

    static TensorSpec concat(const TensorSpec &a, const TensorSpec &b, const std::string &concat_dim);
    static TensorSpec create(const vespalib::string &type, const CreateSpec &spec, const std::vector<TensorSpec> &children);
    static TensorSpec join(const TensorSpec &a, const TensorSpec &b, join_fun_t function);
    static TensorSpec map(const TensorSpec &a, map_fun_t func);
    static TensorSpec merge(const TensorSpec &a, const TensorSpec &b, join_fun_t fun);
    static TensorSpec peek(const TensorSpec &param, const PeekSpec &spec, const std::vector<TensorSpec> &children);
    static TensorSpec reduce(const TensorSpec &a, const std::vector<vespalib::string> &dims, Aggr aggr);
    static TensorSpec rename(const TensorSpec &a, const std::vector<vespalib::string> &from, const std::vector<vespalib::string> &to);
};

} // namespace
