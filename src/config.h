#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <Eigen/Core>

using Vector3d = Eigen::Vector3d;


namespace mrtp {

class ConfigTable
{
public:
    ConfigTable() = default;
    virtual ~ConfigTable() = default;

    virtual double get_value(const std::string&, double) = 0;

    virtual Vector3d get_vector(const std::string&) = 0;
    virtual std::string get_text(const std::string&) = 0;

    virtual Vector3d get_vector(const std::string&, const Vector3d&) = 0;
};


class ConfigTableIterator
{
public:
    ConfigTableIterator() = default;
    virtual ~ConfigTableIterator() = default;

    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool is_done() = 0;
    virtual std::shared_ptr<ConfigTable> current() = 0;
};


class ConfigReader
{
public:
    ConfigReader() = default;
    virtual ~ConfigReader() = default;

    virtual std::shared_ptr<ConfigTableIterator> get_tables(const std::string&) = 0;

    virtual std::shared_ptr<ConfigTable> get_table(const std::string&) = 0;
};


std::shared_ptr<ConfigReader> open_config(const std::string&);


}

#endif // CONFIG_H
