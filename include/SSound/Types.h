#ifndef SSOUND_TYPES_H
#define SSOUND_TYPES_H

namespace SSound
{

    enum StarsResourceType
    {
        RESOURCETYPE_PIPELINE = 20000,
        RESOURCETYPE_TEXTURE
    };

    enum PipelineType
    {
        PIPELINE_NONE = 0,
        PIPELINE_COLOR,
        PIPELINE_TEXTURE,
        PIPELINE_MASKEDTEXTURE,
        PIPELINE_ALPHATEXTURE,
    };

    enum CommonFunctionType
    {
        FUNCTIONTYPE_RECT = 100000,
        FUNCTIONTYPE_PANEL,
        FUNCTIONTYPE_IMAGE,
        FUNCTIONTYPE_FRAMEIMAGE,
        FUNCTIONTYPE_TEXT,
    };

}; // namespace SSound

#endif // SSOUND_TYPES_H
