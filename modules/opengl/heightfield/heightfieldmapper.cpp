/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Contact: Martin Falk
 *
 *********************************************************************************/

#include "heightfieldmapper.h"
#include <inviwo/core/datastructures/geometry/simplemeshcreator.h>
#include <inviwo/core/datastructures/image/layerram.h>

namespace inviwo {

ProcessorClassIdentifier(HeightFieldMapper, "org.inviwo.HeightFieldMapper");
ProcessorDisplayName(HeightFieldMapper,  "Height Field Mapper");
ProcessorTags(HeightFieldMapper, Tags::GL); 
ProcessorCategory(HeightFieldMapper, "Heightfield");
ProcessorCodeState(HeightFieldMapper, CODE_STATE_EXPERIMENTAL); 

HeightFieldMapper::HeightFieldMapper()
    : Processor()
    , inport_("image.inport", false)
    , outport_("image.outport", 0, COLOR_ONLY) // &inport_
    , scalingModeProp_("scalingmode", "Scaling Mode")
    , heightRange_("heightrange", "Height Range", 0.0f, 1.0f, -1.0e1f, 1.0e1f)
    , maxHeight_("maxheight", "Maximum Height", 1.0f, 0.0f, 1.0e1f)
    , seaLevel_("sealevel", "Sea Level", 0.0f, -1.0e1f, 1.0e1f)
{
    addPort(inport_);
    addPort(outport_);

    scalingModeProp_.addOption("scaleFixed", "Fixed Range [0:1]", FIXED_RANGE);
    scalingModeProp_.addOption("scaleRange", "Data Range", DATA_RANGE);
    scalingModeProp_.addOption("scaleSeaLevel", "Sea Level", SEA_LEVEL);
    scalingModeProp_.set(FIXED_RANGE);
    scalingModeProp_.onChange(this, &HeightFieldMapper::scalingModeChanged);

    addProperty(scalingModeProp_);
    addProperty(heightRange_);
    addProperty(maxHeight_);
    addProperty(seaLevel_);

    setAllPropertiesCurrentStateAsDefault();

    // update UI state
    scalingModeChanged();
}

HeightFieldMapper::~HeightFieldMapper() {}

void HeightFieldMapper::initialize() {
    Processor::initialize();
}

void HeightFieldMapper::deinitialize() {
    Processor::deinitialize();
}

void HeightFieldMapper::process() {

    if (!inport_.isReady())
        return;

    const Image *srcImg = inport_.getData();
    if (!srcImg) {
        LogWarn("no input image");
        return;
    }

    // check the number of channels
    const DataFormatBase* format = srcImg->getDataFormat();
    int numInputChannels = format->getComponents();
    if (numInputChannels > 1) {
        //LogWarn("input image has more than one channel. Using first channel.");
    }

    glm::uvec2 dim = srcImg->getDimension();

    Image *outImg = 0; //outport_.getData();

    // check format of output image
    if (!outImg || (outImg->getDataFormat()->getId() != DataFormatEnums::FLOAT32)) {
        // replace with new floating point image
        Image *img = new Image(dim, COLOR_ONLY, DataFLOAT32::get());
        outport_.setData(img);
        outImg = img;
        //LogInfo("created float output img");
    }
    else if (outImg->getDimension() != dim) {
        // adjust dimensions of output image
        outImg->resize(dim);

        //std::ostringstream str;
        //str << "Output img resized: " << glm::to_string(outImg->getDimension())
        //    << "   port dimension: " << glm::to_string(outport_.getDimension()) << std::endl;
        //LogInfo(str.str());
    }
    

    std::ostringstream str;
    const DataFormatBase* format2 = outImg->getDataFormat();

    //str << "Input Image:"
    //    << "\ndim: " << glm::to_string(srcImg->getDimension())
    //    << "\nType: " << srcImg->getImageType()
    //    << "\nNum Color Layers: " << srcImg->getNumberOfColorLayers()
    //    << std::endl << std::endl
    //    << "Format:"
    //    << "\nName: " << format->getString()
    //    << "\nComponents: " << format->getComponents()
    //    << "Output Image:"
    //    << "\ndim: " << glm::to_string(outImg->getDimension())
    //    << "\nType: " << outImg->getImageType()
    //    << "\nNum Color Layers: " << outImg->getNumberOfColorLayers()
    //    << std::endl << std::endl
    //    << "Format:"
    //    << "\nName: " << format2->getString()
    //    << "\nComponents: " << format2->getComponents()
    //    ;

    //LogInfo(str.str());



    LayerRAM *dstLayer = outImg->getColorLayer(0)->getEditableRepresentation<LayerRAM>();
    //float *data = new float[dim.x * dim.y]; 
    float *data = static_cast<float*>(dstLayer->getData());

    // convert input image to float image
    const LayerRAM *srcLayer = srcImg->getColorLayer(0)->getRepresentation<LayerRAM>();

    // special case: input image is already FLOAT32 with 1 channel
    if ((numInputChannels == 1) && (format->getId() == DataFormatEnums::FLOAT32)) {
        const float *srcData = static_cast<const float*>(srcLayer->getData());
        std::copy(srcData, srcData + dim.x * dim.y, data);
    }
    else {
        switch (numInputChannels) {
        case 2:
            for (unsigned int y=0; y<dim.y; ++y) {
                for (unsigned int x=0; x<dim.x; ++x) {
                    data[y * dim.x + x] = srcLayer->getValueAsVec2Double(glm::uvec2(x, y)).r;
                }
            }
            break;
        case 3:
            for (unsigned int y=0; y<dim.y; ++y) {
                for (unsigned int x=0; x<dim.x; ++x) {
                    data[y * dim.x + x] = srcLayer->getValueAsVec3Double(glm::uvec2(x, y)).r;
                }
            }
            break;
        case 4:
            for (unsigned int y=0; y<dim.y; ++y) {
                for (unsigned int x=0; x<dim.x; ++x) {
                    data[y * dim.x + x] = srcLayer->getValueAsVec4Double(glm::uvec2(x, y)).r;
                }
            }
            break;
        case 1:
        default:
            for (unsigned int y=0; y<dim.y; ++y) {
                for (unsigned int x=0; x<dim.x; ++x) {
                    data[y * dim.x + x] = srcLayer->getValueAsSingleDouble(glm::uvec2(x, y));
                }
            }
            break;
        }
    }

    // rescale data set
    std::size_t numValues = dim.x * dim.y;
    // determine min/max values
    float minVal = *std::min_element(data, data + numValues);
    float maxVal = *std::max_element(data, data + numValues);

    switch (scalingModeProp_.get()) {
    case SEA_LEVEL:
        {
            // scale heightfield based on sea level and min/max height
            float sealevel = seaLevel_.get();
            float aboveSea = maxVal - sealevel;
            float belowSea = minVal - sealevel;

            float factor = maxHeight_.get() / std::max(aboveSea, std::abs(belowSea));
            for (std::size_t i=0; i<numValues; ++i) {
                data[i] = (data[i] - sealevel) * factor;
            }
        }
        break;
    case DATA_RANGE:
        {
            // scale data to [heightRange_.min : heightRange_.max]
            glm::vec2 range(heightRange_.get());

            float factor = (range.y - range.x) / (maxVal - minVal);
            for (std::size_t i=0; i<numValues; ++i) {
                data[i] = (data[i] - minVal) * factor  + range.x;
            }
        }
        break;
    case FIXED_RANGE:
    default:
        {
            // scale data to [0:1] range
            float delta = 1.0f / (maxVal - minVal);
            for (std::size_t i=0; i<numValues; ++i) {
                data[i] = (data[i] - minVal) * delta;
            }
        }
        break;
    }
}

void HeightFieldMapper::scalingModeChanged() {
    
    if ((scalingModeProp_.get() < 0) || (scalingModeProp_.get() >= NUMBER_OF_SCALING_MODES)) {
        LogError("invalid scaling property (out of range)");
        scalingModeProp_.set(DATA_RANGE);
    }

    ScalingMode mode = static_cast<ScalingMode>(scalingModeProp_.get());    
    heightRange_.setVisible(mode == DATA_RANGE);
    maxHeight_.setVisible(mode == SEA_LEVEL);
    seaLevel_.setVisible(mode == SEA_LEVEL);
}

} // namespace
