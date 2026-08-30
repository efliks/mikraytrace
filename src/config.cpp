#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <cstdlib>

#include "config.h"


namespace mrtp {

// ---------------------------------------------------------------- Text format
//
// Each non-empty line describes one row and has the form:
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

    double get_value(const std::string& key, double val_default) // override
    {
        std::map<std::string, std::string>::iterator it = fields_.find(key);
        if (it == fields_.end()) {
            return val_default;
        }

        return std::atof(it->second.c_str());
    }

    Vector3d get_vector(const std::string& key, const Vector3d& vec_default) // override
    {
        std::map<std::string, std::string>::iterator it = fields_.find(key);
        if (it == fields_.end()) {
            return vec_default;
        }

        return parse_vector(it->second);
    }

    Vector3d get_vector(const std::string& key) // override
    {
        return get_vector(key, Vector3d());
    }

    std::string get_text(const std::string& key) // override
    {
        std::map<std::string, std::string>::iterator it = fields_.find(key);
        if (it == fields_.end()) {
            return std::string();
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

        return Vector3d(components);
    }

    std::map<std::string, std::string> fields_;
};


class TextTableIterator : public ConfigTableIterator
{
public:
    TextTableIterator(const std::vector<std::shared_ptr<ConfigTable> >& tables)
        : tables_(tables)
    {
    }

    void first() // override
    {
        iter_ = tables_.begin();
    }

    void next() // override
    {
        ++iter_;
    }

    bool is_done() // override
    {
        return iter_ == tables_.end();
    }

    std::shared_ptr<ConfigTable> current() // override
    {
        return *iter_;
    }

private:
    std::vector<std::shared_ptr<ConfigTable> > tables_;
    std::vector<std::shared_ptr<ConfigTable> >::iterator iter_;
};


class TextReader : public ConfigReader
{
public:
    TextReader(const std::vector<std::pair<std::string, std::shared_ptr<ConfigTable> > >& rows)
        : rows_(rows)
    {
    }

    std::shared_ptr<ConfigTableIterator> get_tables(const std::string& row_type) // override
    {
        std::vector<std::shared_ptr<ConfigTable> > matches;
        for (std::vector<std::pair<std::string, std::shared_ptr<ConfigTable> > >::const_iterator it = rows_.begin();
             it != rows_.end(); ++it) {
            if (it->first == row_type) {
                matches.push_back(it->second);
            }
        }

        if (matches.empty()) {
            return std::shared_ptr<ConfigTableIterator>();
        }

        return std::shared_ptr<ConfigTableIterator>(new TextTableIterator(matches));
    }

private:
    std::vector<std::pair<std::string, std::shared_ptr<ConfigTable> > > rows_;
};


std::shared_ptr<ConfigReader> open_config(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.good()) {
        std::cerr << "ERROR: Cannot open world file" << std::endl;
        return std::shared_ptr<ConfigReader>();
    }

    std::vector<std::pair<std::string, std::shared_ptr<ConfigTable> > > rows;

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
            std::cerr << "ERROR: Malformed line in world file: " << line << std::endl;
            return std::shared_ptr<ConfigReader>();
        }

        std::string row_type = trim(trimmed.substr(0, open_paren));
        std::string body = trimmed.substr(open_paren + 1, close_paren - open_paren - 1);

        std::shared_ptr<ConfigTable> table = std::shared_ptr<ConfigTable>(new TextTable(parse_fields(body)));
        rows.push_back(std::make_pair(row_type, table));
    }

    return std::shared_ptr<ConfigReader>(new TextReader(rows));
}


}
