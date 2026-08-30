#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "shrptr.h"
#include "vector3.h"

namespace mrtp {

class ConfigTable
{
public:
    virtual ~ConfigTable() {}

    virtual double get_value(const std::string&, double) = 0;

    virtual Vector3d get_vector(const std::string&) = 0;
    virtual std::string get_text(const std::string&) = 0;

    virtual Vector3d get_vector(const std::string&, const Vector3d&) = 0;
};


class ConfigTableIterator
{
public:
    virtual ~ConfigTableIterator() {}

    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool is_done() = 0;
    virtual shared_ptr<ConfigTable> current() = 0;
};


class ConfigReader
{
public:
    virtual ~ConfigReader() {}

    virtual shared_ptr<ConfigTableIterator> get_tables(const std::string&) = 0;
};


shared_ptr<ConfigReader> open_config(const std::string&);


}

#endif // CONFIG_H
