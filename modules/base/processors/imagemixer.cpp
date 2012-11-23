#include "imagemixer.h"

namespace inviwo {

ImageMixer::ImageMixer()
    : ProcessorGL(),
      inport0_(Port::INPORT, "inport0"),
      inport1_(Port::INPORT, "inport1"),
      outport_(Port::OUTPORT, "outport"),
      alpha_("alpha", "Alpha", 0.5f, 0.0f, 1.0f)
{
    addPort(inport0_);
    addPort(inport1_);
    addPort(outport_);
    addProperty(alpha_);
}

ImageMixer::~ImageMixer() {}

Processor* ImageMixer::create() const {
    return new ImageMixer();
}

void ImageMixer::initialize() throw (Exception) {
    ProcessorGL::initialize();
    shader_ = new Shader("img_mix.frag");
}

void ImageMixer::deinitialize() throw (Exception) {
    delete shader_;
    Processor::deinitialize();
}

void ImageMixer::process() {    
    
    Image* outImage = outport_.getData();
    ImageGL* outImageGL = outImage->getRepresentation<ImageGL>();
    ivec2 csize = outImageGL->size();

    activateTarget(outport_);
    bindColorTexture(inport0_, GL_TEXTURE0);
    bindColorTexture(inport1_, GL_TEXTURE1);

    shader_->activate();
    shader_->setUniform("inport0_", 0);
    shader_->setUniform("inport1_", 1);
    shader_->setUniform("alpha_", alpha_.get());
    shader_->setUniform("dimension_", vec2(1.f / csize[0], 1.f / csize[1]) );
    renderImagePlaneQuad();
    shader_->deactivate();

    deactivateCurrentTarget();
}

} // namespace
