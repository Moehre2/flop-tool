#pragma once

#include "sdfg/analysis/flop_analysis.h"
#include "sdfg/structured_control_flow/control_flow_node.h"

void traverse(sdfg::analysis::FlopAnalysis& flop_analysis, sdfg::structured_control_flow::ControlFlowNode* node);
