#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <Eigen/Core>

using Vector3d = Eigen::Vector3d;


namespace mrtp {

class BaseTable
{
public:
    BaseTable() = default;
    virtual ~BaseTable() = default;

    virtual double get_value(const std::string&, double) = 0;

    virtual Vector3d get_vector(const std::string&) = 0;
    virtual std::string get_text(const std::string&) = 0;

    virtual Vector3d get_vector(const std::string&, const Vector3d&) = 0;
    virtual std::string get_text(const std::string&, const std::string&) = 0;
};


class BaseTableIterator
{
public:
    BaseTableIterator() = default;
    virtual ~BaseTableIterator() = default;

    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool is_done() = 0;
    virtual std::shared_ptr<BaseTable> current() = 0;
};


class BaseConfig
{
public:
    BaseConfig() = default;
    virtual ~BaseConfig() = default;

    virtual std::shared_ptr<BaseTableIterator> get_tables(const std::string&) = 0;

    virtual std::shared_ptr<BaseTable> get_table(const std::string&) = 0;
};


std::shared_ptr<BaseConfig> open_config(const std::string&);


}

#endif // CONFIG_H
