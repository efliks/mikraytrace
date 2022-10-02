#ifndef WRITER_H
#define WRITER_H

#include <string>
#include <memory>

#include "renderer.h"


namespace mrtp {

enum class WriterType
{
    PNG,
    JPEG
};


class SceneWriterBase
{
public:
    SceneWriterBase(SceneRendererBase*);
    SceneWriterBase() = delete;
    virtual ~SceneWriterBase() = default;

    virtual void write_to_file(const std::string&) = 0;

protected:
    SceneRendererBase* scene_renderer_;
};


std::shared_ptr<SceneWriterBase> create_writer(SceneRendererBase*, WriterType = WriterType::PNG);


}

#endif // WRITER_H
