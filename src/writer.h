#ifndef WRITER_H
#define WRITER_H

#include <string>
#include <memory>

#include "renderer.h"


namespace mrtp {

enum WriterType
{
    WriterType_PNG,
    WriterType_JPEG
};


class SceneWriterBase
{
public:
    SceneWriterBase(SceneRendererBase*);
    virtual ~SceneWriterBase() {}

    virtual void write_to_file(const std::string&) = 0;

protected:
    SceneRendererBase* scene_renderer_;
};


std::shared_ptr<SceneWriterBase> create_writer(SceneRendererBase*, WriterType = WriterType_PNG);


}

#endif // WRITER_H
