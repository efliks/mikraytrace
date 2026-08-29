#include <fstream>
#include <sstream>
#include <limits>  // req. by cpptoml with DJGPP
#include <map>
#include <cstdlib>

#include "cpptoml.h"
#include "config.h"
#include "logger.h"


namespace mrtp {

class TomlTable : public ConfigTable
{
public:
    TomlTable(std::shared_ptr<cpptoml::table> table)
        : t_(table)
    {
    }

    ~TomlTable() override = default;

    double get_value(const std::string& key, double val_default) override
    {
        return t_->get_as<double>(key).value_or(val_default);
    }

    Vector3d get_vector(const std::string& key, const Vector3d& vec_default) override
    {
        auto x = t_->get_array_of<double>(key);
        if (!x) {
            return vec_default;
        }

        return Vector3d{x->data()};
    }

    Vector3d get_vector(const std::string& key) override
    {
        auto x = t_->get_array_of<double>(key);
        if (!x) {
            return Vector3d{};
        }

        return Vector3d{x->data()};
    }

    std::string get_text(const std::string& key) override
    {
        auto x = t_->get_as<std::string>(key);
        if (!x) {
            return std::string{};
        }

        return std::string{x->data()};
    }

private:
    std::shared_ptr<cpptoml::table> t_;
};


class TomlTableIterator : public ConfigTableIterator
{
public:
    TomlTableIterator(std::shared_ptr<cpptoml::table_array> ta)
        : ta_(ta)
    {
    }

    ~TomlTableIterator() override = default;

    void first() override
    {
       iter_ = ta_->begin();
    }

    void next() override
    {
        ++iter_;
    }

    bool is_done() override
    {
        return iter_ == ta_->end();
    }

    std::shared_ptr<ConfigTable> current() override
    {
        return std::shared_ptr<ConfigTable>(new TomlTable(*iter_));
    }

private:
    std::shared_ptr<cpptoml::table_array> ta_;
    cpptoml::table_array::iterator iter_;
};


class TomlReader : public ConfigReader
{
public:
    TomlReader(std::shared_ptr<cpptoml::table> toml_config)
        : c_(toml_config)
    {
    }

    ~TomlReader() override = default;

    std::shared_ptr<ConfigTableIterator> get_tables(const std::string& array_name) override
    {
        std::shared_ptr<cpptoml::table_array> table_array = c_->get_table_array(array_name);
        if (!table_array) {
            return std::shared_ptr<ConfigTableIterator>();
        }

        return std::shared_ptr<ConfigTableIterator>(new TomlTableIterator(table_array));
    }

private:
    std::shared_ptr<cpptoml::table> c_;
};


// ---------------------------------------------------------------- Text format
//
// A simple, line-oriented alternative to TOML. Each non-empty line describes
// one row and has the form:
//
//     type(key = value; key = value, value, value; ...)
//
// for example:
//
//     camera(center = 7, 0, 7; target = 0, 0, 3; roll = 0)
//     plane(center = 0, 0, 0; normal = 0, 0, 1; scale = 0.2)
//
// A value made of comma-separated numbers is read back as a Vector3d; a
// plain value is read back as a number or as text, depending on the getter
// used. Row types are already singular ("camera", "plane", ...), matching
// the names ConfigReader clients ask for.

namespace {

std::string trim(const std::string& text)
{
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return std::string();
    }

    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::map<std::string, std::string> parse_fields(const std::string& body)
{
    std::map<std::string, std::string> fields;

    std::stringstream stream(body);
    std::string field;
    while (std::getline(stream, field, ';')) {
        size_t eq_pos = field.find('=');
        if (eq_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(field.substr(0, eq_pos));
        std::string value = trim(field.substr(eq_pos + 1));
        fields[key] = value;
    }

    return fields;
}

} // unnamed namespace


class TextTable : public ConfigTable
{
public:
    TextTable(const std::map<std::string, std::string>& fields)
        : fields_(fields)
    {
    }

    ~TextTable() override = default;

    double get_value(const std::string& key, double val_default) override
    {
        auto it = fields_.find(key);
        if (it == fields_.end()) {
            return val_default;
        }

        return std::atof(it->second.c_str());
    }

    Vector3d get_vector(const std::string& key, const Vector3d& vec_default) override
    {
        auto it = fields_.find(key);
        if (it == fields_.end()) {
            return vec_default;
        }

        return parse_vector(it->second);
    }

    Vector3d get_vector(const std::string& key) override
    {
        return get_vector(key, Vector3d{});
    }

    std::string get_text(const std::string& key) override
    {
        auto it = fields_.find(key);
        if (it == fields_.end()) {
            return std::string{};
        }

        return it->second;
    }

private:
    static Vector3d parse_vector(const std::string& value)
    {
        double components[3] = {0, 0, 0};

        std::stringstream stream(value);
        std::string token;
        for (int i = 0; i < 3 && std::getline(stream, token, ','); ++i) {
            components[i] = std::atof(trim(token).c_str());
        }

        return Vector3d{components};
    }

    std::map<std::string, std::string> fields_;
};


class TextTableIterator : public ConfigTableIterator
{
public:
    TextTableIterator(const std::vector<std::shared_ptr<ConfigTable>>& tables)
        : tables_(tables)
    {
    }

    ~TextTableIterator() override = default;

    void first() override
    {
        iter_ = tables_.begin();
    }

    void next() override
    {
        ++iter_;
    }

    bool is_done() override
    {
        return iter_ == tables_.end();
    }

    std::shared_ptr<ConfigTable> current() override
    {
        return *iter_;
    }

private:
    std::vector<std::shared_ptr<ConfigTable>> tables_;
    std::vector<std::shared_ptr<ConfigTable>>::iterator iter_;
};


class TextReader : public ConfigReader
{
public:
    TextReader(const std::vector<std::pair<std::string, std::shared_ptr<ConfigTable>>>& rows)
        : rows_(rows)
    {
    }

    ~TextReader() override = default;

    std::shared_ptr<ConfigTableIterator> get_tables(const std::string& row_type) override
    {
        std::vector<std::shared_ptr<ConfigTable>> matches;
        for (const auto& row : rows_) {
            if (row.first == row_type) {
                matches.push_back(row.second);
            }
        }

        if (matches.empty()) {
            return std::shared_ptr<ConfigTableIterator>();
        }

        return std::shared_ptr<ConfigTableIterator>(new TextTableIterator(matches));
    }

private:
    std::vector<std::pair<std::string, std::shared_ptr<ConfigTable>>> rows_;
};


std::shared_ptr<ConfigReader> open_toml_config(const std::string& filename)
{
    std::shared_ptr<cpptoml::table> config;
    try {
        config = cpptoml::parse_file(filename);
    } catch (...) {
        LOG_ERROR("Error parsing world file");
        return std::shared_ptr<ConfigReader>();
    }

    return std::shared_ptr<ConfigReader>(new TomlReader(config));
}


std::shared_ptr<ConfigReader> open_text_config(const std::string& filename)
{
    std::ifstream in(filename);

    std::vector<std::pair<std::string, std::shared_ptr<ConfigTable>>> rows;

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        size_t open_paren = trimmed.find('(');
        size_t close_paren = trimmed.rfind(')');
        if (open_paren == std::string::npos || close_paren == std::string::npos ||
                close_paren < open_paren) {
            LOG_ERROR("Malformed line in world file: " + line);
            return std::shared_ptr<ConfigReader>();
        }

        std::string row_type = trim(trimmed.substr(0, open_paren));
        std::string body = trimmed.substr(open_paren + 1, close_paren - open_paren - 1);

        auto table = std::shared_ptr<ConfigTable>(new TextTable(parse_fields(body)));
        rows.push_back(std::make_pair(row_type, table));
    }

    return std::shared_ptr<ConfigReader>(new TextReader(rows));
}


std::shared_ptr<ConfigReader> open_config(const std::string& filename)
{
    std::fstream check(filename);
    if (!check.good()) {
        LOG_ERROR("Cannot open world file");
        return std::shared_ptr<ConfigReader>();
    }

    bool is_toml = filename.size() >= 5 &&
            filename.compare(filename.size() - 5, 5, ".toml") == 0;

    return is_toml ? open_toml_config(filename) : open_text_config(filename);
}


}
