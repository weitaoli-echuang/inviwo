#ifndef IVW_IMAGEMIXER_H
#define IVW_IMAGEMIXER_H

#include "inviwo/core/inviwo.h"
#include "inviwo/core/ports/imageport.h"
#include "modules/opengl/inviwoopengl.h"
#include "modules/opengl/processorgl.h"
#include "modules/opengl/glwrap/shader.h"

namespace inviwo {

class ImageMixer : public ProcessorGL {
public:
    ImageMixer();
    ~ImageMixer();
    virtual Processor* create() const;

    void initialize() throw (Exception);
    void deinitialize() throw (Exception);

    virtual std::string getClassName() const { return "ImageMixer"; }
    virtual std::string getCategory() const  { return "Compositer"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }

protected:
    virtual void process();

private:
    ImagePort inport0_;
    ImagePort inport1_;
    ImagePort outport_;
    FloatProperty alpha_;

    Shader* shader_;
};

} // namespace

#endif // IVW_IMAGEMIXER_H
