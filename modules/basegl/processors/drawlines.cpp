/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
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
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#include "drawlines.h"
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <modules/opengl/glwrap/shader.h>
#include <modules/opengl/textureutils.h>

namespace inviwo {

ProcessorClassIdentifier(DrawLines, "org.inviwo.DrawLines");
ProcessorDisplayName(DrawLines, "Draw Lines");
ProcessorTags(DrawLines, Tags::GL);
ProcessorCategory(DrawLines, "Drawing");
ProcessorCodeState(DrawLines, CODE_STATE_STABLE);

DrawLines::DrawLines()
    : CompositeProcessorGL()
    , inport_("inport")
    , outport_("outport", &inport_, COLOR_ONLY)
    , lineSize_("lineSize", "Line Size", 1.f, 1.f, 10.f)
    , lineColor_("lineColor", "Line Color", vec4(1.f))
    , clearButton_("clearButton", "Clear Lines")
    , lines_(NULL)
    , lineRenderer_(NULL) 
    , lineShader_(NULL)
{
    addPort(inport_);
    addPort(outport_);

    addProperty(lineSize_);
    lineColor_.setSemantics(PropertySemantics::Color);
    addProperty(lineColor_);
    clearButton_.onChange(this, &DrawLines::clearLines);
    addProperty(clearButton_);

    addInteractionHandler(new DrawLinesInteractionHandler(this));
}

DrawLines::~DrawLines() {
    const std::vector<InteractionHandler*>& interactionHandlers = getInteractionHandlers();
    for (size_t i = 0; i < interactionHandlers.size(); ++i) {
        InteractionHandler* handler = interactionHandlers[i];
        removeInteractionHandler(handler);
        delete handler;
    }
}

void DrawLines::initialize() {
    CompositeProcessorGL::initialize();

    GLint aliasRange[2];
    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, aliasRange);

    lineSize_.setMinValue(aliasRange[0]);
    lineSize_.setMaxValue(aliasRange[1]);

    lineShader_ = new Shader("img_color.frag");
    lines_ = new Mesh(GeometryEnums::LINES, GeometryEnums::STRIP);
    lines_->addAttribute(new Position2dBuffer());
    lineRenderer_ = new MeshRenderer(lines_);
}

void DrawLines::deinitialize() {
    CompositeProcessorGL::deinitialize();
    delete lineShader_;
    lineShader_ = NULL;
    delete lineRenderer_;
    lineRenderer_ = NULL;
    delete lines_;
    lines_ = NULL;
}

void DrawLines::process() {
    utilgl::activateAndClearTarget(outport_);
    bool reEnableLineSmooth = false;
    if (glIsEnabled(GL_LINE_SMOOTH)) {
        glDisable(GL_LINE_SMOOTH);
        reEnableLineSmooth = true;
    }
    glLineWidth(static_cast<GLfloat>(lineSize_.get()));
    lineShader_->activate();
    lineShader_->setUniform("color_", lineColor_.get());
    lineRenderer_->render();
    lineShader_->deactivate();
    glLineWidth(1.f);
    if (reEnableLineSmooth)
        glEnable(GL_LINE_SMOOTH);
    utilgl::deactivateCurrentTarget();
    compositePortsToOutport(outport_, inport_);
}

void DrawLines::addPoint(vec2 p) {
    if (lines_)
        lines_->getAttributes(0)->getEditableRepresentation<Position2dBufferRAM>()->add(p);
}

void DrawLines::clearLines() {
    if (lines_)
        lines_->getAttributes(0)->getEditableRepresentation<Position2dBufferRAM>()->clear();
}

DrawLines::DrawLinesInteractionHandler::DrawLinesInteractionHandler(DrawLines* dfh)
    : InteractionHandler()
    , drawPosEvent(MouseEvent::MOUSE_BUTTON_LEFT, InteractionEvent::MODIFIER_CTRL)
    , drawEnableEvent_('D', InteractionEvent::MODIFIER_CTRL)
    , drawer_(dfh)
    , drawModeEnabled_(false) {}

void DrawLines::DrawLinesInteractionHandler::invokeEvent(Event* event) {
    KeyboardEvent* keyEvent = dynamic_cast<KeyboardEvent*>(event);
    if (keyEvent) {
        int button = keyEvent->button();
        int state = keyEvent->state();
        int modifier = keyEvent->modifiers();

        if (button == drawEnableEvent_.button() && modifier == drawEnableEvent_.modifiers()) {
            if (state == KeyboardEvent::KEY_STATE_PRESS) {
                drawModeEnabled_ = true;
            } else if (state == KeyboardEvent::KEY_STATE_RELEASE) {
                drawModeEnabled_ = false;
            }
        }
        return;
    }

    MouseEvent* mouseEvent = dynamic_cast<MouseEvent*>(event);
    if (drawModeEnabled_ && mouseEvent) {
        if (mouseEvent->modifiers() == drawPosEvent.modifiers() 
            && mouseEvent->button() == drawPosEvent.button()) {
                vec2 line = mouseEvent->posNormalized();
                line *= 2.f;
                line -= 1.f;
                line.y = -line.y;
                drawer_->addPoint(line);
                drawer_->invalidate(INVALID_OUTPUT);
        }
        return;
    }
}

}  // namespace
