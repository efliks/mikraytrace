#ifndef CONFIG_H
#define CONFIG_H


namespace mrtp {

class RendererConfig {
public:
    RendererConfig();
    ~RendererConfig() = default;

    double field_of_vision;
    double max_distance;
    double shadow_bias;
    double ray_bias;

    unsigned int buffer_width;
    unsigned int buffer_height;

    unsigned int max_ray_depth;
    unsigned int num_threads;
};


class OptionParser {
public:
    OptionParser(RendererConfig*);
    virtual ~OptionParser() = default;
    virtual void parse(char*) = 0;

    bool is_parsed;

protected:
    RendererConfig* renderer_config_;
};


class FieldOfVisionParser : public OptionParser {
public:
    FieldOfVisionParser(RendererConfig*);
    ~FieldOfVisionParser() override = default;

    void parse(char*) override;
};


class MaxDistanceParser : public OptionParser {
public:
    MaxDistanceParser(RendererConfig*);
    ~MaxDistanceParser() override = default;

    void parse(char*) override;
};


class ShadowBiasParser : public OptionParser {
public:
    ShadowBiasParser(RendererConfig*);
    ~ShadowBiasParser() override = default;

    void parse(char*) override;
};


class RayBiasParser : public OptionParser {
public:
    RayBiasParser(RendererConfig*);
    ~RayBiasParser() override = default;

    void parse(char*) override;
};


class ResolutionParser : public OptionParser {
public:
    ResolutionParser(RendererConfig*);
    ~ResolutionParser() override = default;

    void parse(char*) override;
};


class RayDepthParser : public OptionParser {
public:
    RayDepthParser(RendererConfig*);
    ~RayDepthParser() override = default;

    void parse(char*) override;
};


class ThreadsParser : public OptionParser {
public:
    ThreadsParser(RendererConfig*);
    ~ThreadsParser() override = default;

    void parse(char*) override;
};


}

#endif // CONFIG_H
