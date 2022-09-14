#include <fstream>
#include <easylogging++.h>

#include "cpptoml.h"
#include "config.h"


namespace mrtp {

class TomlTable : public BaseTable
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

    std::string get_text(const std::string& key, const std::string& text_default) override
    {
        auto x = t_->get_as<std::string>(key);
        if (!x) {
            return text_default;
        }

        return std::string{x->data()};
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


class TomlTableIterator : public BaseTableIterator
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

    std::shared_ptr<BaseTable> current() override
    {
        return std::shared_ptr<BaseTable>(new TomlTable(*iter_));
    }

private:
    std::shared_ptr<cpptoml::table_array> ta_;
    cpptoml::table_array::iterator iter_;
};


class TomlConfig : public BaseConfig
{
public:
    TomlConfig(std::shared_ptr<cpptoml::table> toml_config)
        : c_(toml_config)
    {
    }

    ~TomlConfig() override = default;

    std::shared_ptr<BaseTableIterator> get_tables(const std::string& array_name) override
    {
        std::shared_ptr<cpptoml::table_array> table_array = c_->get_table_array(array_name);
        if (!table_array) {
            return std::shared_ptr<BaseTableIterator>();
        }

        return std::shared_ptr<BaseTableIterator>(new TomlTableIterator(table_array));
    }

    std::shared_ptr<BaseTable> get_table(const std::string& table_name) override
    {
        auto raw_table = c_->get_table(table_name);
        if (!raw_table) {
            return std::shared_ptr<BaseTable>();
        }

        return std::shared_ptr<BaseTable>(new TomlTable(raw_table));
    }

private:
    std::shared_ptr<cpptoml::table> c_;
};


std::shared_ptr<BaseConfig> open_config(const std::string& filename)
{
    std::fstream check(filename.c_str());
    if (!check.good()) {
        LOG(ERROR) << "Cannot open world file";
        return std::shared_ptr<BaseConfig>();
    }

    std::shared_ptr<cpptoml::table> config;
    try {
        config = cpptoml::parse_file(filename.c_str());
    } catch (...) {
        LOG(ERROR) << "Error parsing world file";
        return std::shared_ptr<BaseConfig>();
    }

    return std::shared_ptr<BaseConfig>(new TomlConfig(config));
}


}
