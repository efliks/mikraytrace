#include <fstream>

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

    std::shared_ptr<ConfigTable> get_table(const std::string& table_name) override
    {
        auto raw_table = c_->get_table(table_name);
        if (!raw_table) {
            return std::shared_ptr<ConfigTable>();
        }

        return std::shared_ptr<ConfigTable>(new TomlTable(raw_table));
    }

private:
    std::shared_ptr<cpptoml::table> c_;
};


std::shared_ptr<ConfigReader> open_config(const std::string& filename)
{
    std::fstream check(filename);
    if (!check.good()) {
        LOG_ERROR("Cannot open world file");
        return std::shared_ptr<ConfigReader>();
    }

    std::shared_ptr<cpptoml::table> config;
    try {
        config = cpptoml::parse_file(filename);
    } catch (...) {
        LOG_ERROR("Error parsing world file");
        return std::shared_ptr<ConfigReader>();
    }

    return std::shared_ptr<ConfigReader>(new TomlReader(config));
}


}
