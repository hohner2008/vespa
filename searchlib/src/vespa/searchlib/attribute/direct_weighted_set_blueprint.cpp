// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "direct_weighted_set_blueprint.h"
#include "direct_weighted_set_blueprint.hpp"
#include <vespa/searchlib/queryeval/dot_product_search.h>
#include <vespa/searchlib/queryeval/weighted_set_term_search.h>

namespace search::attribute {

template class DirectWeightedSetBlueprint<queryeval::WeightedSetTermSearch>;
template class DirectWeightedSetBlueprint<queryeval::DotProductSearch>;

}

