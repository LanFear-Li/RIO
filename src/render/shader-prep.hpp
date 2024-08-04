#pragma once

#include <cprep/cprep.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

inline std::string read_all_from_file(const fs::path &path) {
    std::ifstream fin(path);
    fin.seekg(0, std::ios::end);
    auto length = fin.tellg();
    std::string content(length, '\0');
    fin.seekg(0, std::ios::beg);
    fin.read(content.data(), length);
    while (!content.empty() && content.back() == '\0') { content.pop_back(); }
    return content;
}

class FsShaderIncluder final : public pep::cprep::ShaderIncluder {
public:
    FsShaderIncluder(std::vector<fs::path> &&include_dirs) : include_dirs_(std::move(include_dirs)) {}

    bool require_header(std::string_view header_name, std::string_view file_path, Result &result) override {
        auto rel_path = fs::path{file_path}.parent_path() / header_name;
        if (fs::exists(rel_path)) {
            result.header_path = rel_path.string();
            result.header_content = read_file(rel_path);
            return true;
        }

        for (const auto &dir : include_dirs_) {
            auto path = dir / header_name;
            if (fs::exists(path)) {
                result.header_path = path.string();
                result.header_content = read_file(path);
                return true;
            }
        }

        return false;
    }

    void clear() override {
        sources_.clear();
    }

private:
    std::string_view read_file(const fs::path &path) {
        sources_.push_back(read_all_from_file(path));
        return sources_.back();
    }

    std::vector<fs::path> include_dirs_;
    std::vector<std::string> sources_;
};

inline std::string shader_pre_compile(std::string_view shader_path)
{
    auto source = read_all_from_file(shader_path);
    std::vector<fs::path> include_dirs{{"runtime/shader/"}};
    FsShaderIncluder includer{std::move(include_dirs)};
    pep::cprep::Preprocessor preprocessor{};

    auto prep_result = preprocessor.do_preprocess(
        shader_path, source, includer, nullptr, 0
    );
    if (!prep_result.warning.empty()) {
        std::cout << prep_result.warning << std::endl;
    }
    if (!prep_result.error.empty()) {
        std::cerr << prep_result.error << std::endl;
        return {};
    }

    return prep_result.parsed_result;
}
