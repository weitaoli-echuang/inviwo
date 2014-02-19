/**********************************************************************
 * Copyright (C) 2014 ContextVision AB
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Daniel J�nsson
 *
 **********************************************************************/

#ifndef IVW_VOLUME_SEPIA_SHADING_CL_H
#define IVW_VOLUME_SEPIA_SHADING_CL_H

#include <modules/cvultrasoundshading/cvultrasoundshadingmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/light/directionallight.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/advancedmaterialproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>

#include <modules/opencl/inviwoopencl.h>
#include <modules/opencl/image/layerclbase.h>
#include <modules/opencl/volume/volumeclbase.h>

namespace inviwo {

class IVW_MODULE_CVULTRASOUNDSHADING_API VolumeSepiaShadingCL: public Processor {
public:
    VolumeSepiaShadingCL();
    ~VolumeSepiaShadingCL();
    
    InviwoProcessorInfo();

    void initialize();
    void deinitialize();

protected:
    virtual void process();

    void compute( const VolumeCLBase& volumeCL, const LayerCLBase& entryPoints, const LayerCLBase& exitPoints, const LayerCLBase& transferFunctionCL, const LayerCLBase& output, const DirectionalLight* light, cl::Event* profilingEvent );

private:
    VolumeInport volumePort_;
    ImageInport entryPort_;
    ImageInport exitPort_;
    DataInport<LightSource> lightSource_;
    ImageOutport outport_;

    AdvancedMaterialProperty advancedMaterial_;
    FloatProperty samplingRate_;
    TransferFunctionProperty transferFunction_;
    IntVec2Property workGroupSize_;
    BoolProperty useGLSharing_;

    cl::Kernel* kernel_;
};

} // namespace

#endif // IVW_VOLUME_SEPIA_SHADING_CL_H