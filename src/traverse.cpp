#include "traverse.h"
#include <cassert>
#include <cstddef>
#include <iostream>

#include "sdfg/analysis/flop_analysis.h"
#include "sdfg/codegen/utils.h"
#include "sdfg/element.h"
#include "sdfg/structured_control_flow/block.h"
#include "sdfg/structured_control_flow/control_flow_node.h"
#include "sdfg/structured_control_flow/for.h"
#include "sdfg/structured_control_flow/if_else.h"
#include "sdfg/structured_control_flow/map.h"
#include "sdfg/structured_control_flow/return.h"
#include "sdfg/structured_control_flow/sequence.h"
#include "sdfg/structured_control_flow/while.h"

void print_debug_info(sdfg::codegen::PrettyPrinter& stream, const sdfg::DebugInfo& debug_info) {
    if (!debug_info.has()) {
        return;
    }
    stream << debug_info.filename() << ":" << debug_info.start_line() << "-" << debug_info.end_line() << " ";
}

void print_node(
    sdfg::codegen::PrettyPrinter& stream,
    sdfg::analysis::FlopAnalysis& flop_analysis,
    sdfg::structured_control_flow::ControlFlowNode* node,
    const char* name
) {
    stream << name << " ";
    print_debug_info(stream, node->debug_info());
    if (flop_analysis.contains(node)) {
        auto flop = flop_analysis.get(node);
        if (flop.is_null()) {
            stream << "NULL";
        } else {
            stream << flop->__str__();
        }
    } else {
        stream << "NOT MEASURED";
    }
    stream << std::endl;
}

void traverse(
    sdfg::codegen::PrettyPrinter& stream,
    sdfg::analysis::FlopAnalysis& flop_analysis,
    sdfg::structured_control_flow::ControlFlowNode* node
) {
    if (dynamic_cast<sdfg::structured_control_flow::Block*>(node)) {
        print_node(stream, flop_analysis, node, "Block");
    } else if (auto* for_loop = dynamic_cast<sdfg::structured_control_flow::For*>(node)) {
        print_node(stream, flop_analysis, node, "For");
        stream.setIndent(stream.indent() + 2);
        traverse(stream, flop_analysis, &for_loop->root());
        stream.setIndent(stream.indent() - 2);
    } else if (auto* if_else = dynamic_cast<sdfg::structured_control_flow::IfElse*>(node)) {
        print_node(stream, flop_analysis, node, "IfElse");
        stream.setIndent(stream.indent() + 2);
        for (size_t i = 0; i < if_else->size(); i++) {
            stream << if_else->at(i).second->__str__() << std::endl;
            stream.setIndent(stream.indent() + 2);
            traverse(stream, flop_analysis, &if_else->at(i).first);
            stream.setIndent(stream.indent() - 2);
        }
        stream.setIndent(stream.indent() - 2);
    } else if (auto* map = dynamic_cast<sdfg::structured_control_flow::Map*>(node)) {
        print_node(stream, flop_analysis, node, "Map");
        stream.setIndent(stream.indent() + 2);
        traverse(stream, flop_analysis, &map->root());
        stream.setIndent(stream.indent() - 2);
    } else if (dynamic_cast<sdfg::structured_control_flow::Return*>(node)) {
        print_node(stream, flop_analysis, node, "Return");
    } else if (auto* sequence = dynamic_cast<sdfg::structured_control_flow::Sequence*>(node)) {
        print_node(stream, flop_analysis, node, "Sequence");
        stream.setIndent(stream.indent() + 2);
        for (size_t i = 0; i < sequence->size(); i++) {
            traverse(stream, flop_analysis, &sequence->at(i).first);
        }
        stream.setIndent(stream.indent() - 2);
    } else if (auto* while_loop = dynamic_cast<sdfg::structured_control_flow::While*>(node)) {
        print_node(stream, flop_analysis, node, "While");
        stream.setIndent(stream.indent() + 2);
        traverse(stream, flop_analysis, &while_loop->root());
        stream.setIndent(stream.indent() - 2);
    } else if (dynamic_cast<sdfg::structured_control_flow::Break*>(node)) {
        print_node(stream, flop_analysis, node, "Break");
    } else if (dynamic_cast<sdfg::structured_control_flow::Continue*>(node)) {
        print_node(stream, flop_analysis, node, "Continue");
    } else {
        assert(false);
    }
}

void traverse(sdfg::analysis::FlopAnalysis& flop_analysis, sdfg::structured_control_flow::ControlFlowNode* node) {
    sdfg::codegen::PrettyPrinter stream;
    if (flop_analysis.precise()) {
        stream << "Precise" << std::endl;
    } else {
        stream << "Approximation" << std::endl;
    }
    traverse(stream, flop_analysis, node);
    std::cout << stream.str() << std::endl;
}
