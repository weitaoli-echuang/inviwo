#ifndef IVW_CANVASGL_H
#define IVW_CANVASGL_H

#include <modules/opengl/openglmoduledefine.h>
#include <modules/opengl/inviwoopengl.h>
#include <inviwo/core/util/canvas.h>
#include <modules/opengl/glwrap/shader.h>
#include <modules/opengl/image/imagegl.h>

namespace inviwo {

class IVW_MODULE_OPENGL_API CanvasGL : public Canvas {
public:
    CanvasGL(uvec2 dimensions);
    virtual ~CanvasGL();

    virtual void initialize();
    virtual void deinitialize();
    virtual void activate();
    void render(const Image* im, ImageLayerType layer = COLOR_LAYER);
    virtual void resize(uvec2 size);
    virtual void glSwapBuffers();
    virtual void update();

    static inline void renderImagePlaneRect(){
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, screenAlignedVerticesId_);
        glVertexPointer(2, GL_FLOAT, 0, 0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, screenAlignedTexCoordsId_);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

protected:
    virtual void initializeGL();
    virtual void initializeSquare();

    void renderColor();
    void renderDepth();
    void renderPicking();
    void renderNoise();
    void renderTexture(GLint);

private:
    static bool glewInitialized_;
    static GLuint screenAlignedVerticesId_;
    static GLuint screenAlignedTexCoordsId_;

    const ImageGL* imageGL_;
    Shader* shader_;
    Shader* noiseShader_;

};

} // namespace

#endif // IVW_CANVASGL_H
