#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "sdfg/analysis/analysis.h"
#include "sdfg/analysis/flop_analysis.h"
#include "sdfg/builder/structured_sdfg_builder.h"
#include "sdfg/codegen/dispatchers/node_dispatcher_registry.h"
#include "sdfg/serializer/json_serializer.h"
#include "traverse.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: flop-tool [path/to/sdfg.json]" << std::endl;
        return 1;
    }

    const std::string jsonFile(argv[1]);
    std::ifstream stream(jsonFile);
    if (!stream.good()) {
        std::cerr << "Could not open file: " << jsonFile << std::endl;
        return 1;
    }
    nlohmann::json json = nlohmann::json::parse(stream);

    sdfg::codegen::register_default_dispatchers();
    sdfg::serializer::register_default_serializers();

    sdfg::serializer::JSONSerializer serializer;
    auto sdfg = serializer.deserialize(json);

    sdfg::builder::StructuredSDFGBuilder builder(sdfg);
    sdfg::analysis::AnalysisManager analysis_manager(builder.subject());

    auto& flop_analysis = analysis_manager.get<sdfg::analysis::FlopAnalysis>();
    traverse(flop_analysis, &builder.subject().root());

    return 0;
}
