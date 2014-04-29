//##########################################################################
//#                                                                        #
//#                               CCFBO                                    #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Library General Public License as       #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifdef USE_VLD
//VLD
#include <vld.h>
#endif

#include "ccOpenGLUtils.h"

//*********** OPENGL TEXTURES ***********//
void ccOpenGLUtils::DisplayTexture2D(GLuint tex, int w, int h)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    float halfW = (float)w*0.5f;
    float halfH = (float)h*0.5f;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-halfW, -halfH);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f( halfW, -halfH);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f( halfW,  halfH);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-halfW,  halfH);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void ccOpenGLUtils::DisplayTexture2DCorner(GLuint tex, int w, int h)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(0, 0);
    glTexCoord2f(1.0, 0.0);
    glVertex2i(w, 0);
    glTexCoord2f(1.0, 1.0);
    glVertex2i(w, h);
    glTexCoord2f(0.0, 1.0);
    glVertex2i(0, h);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

//*********** OPENGL EXTENSIONS ***********//
#ifdef CC_USE_GLEW
GLenum ccOpenGLUtils::InitGLEW()
{
    return glewInit();
}

bool ccOpenGLUtils::CheckExtension(const char *extName)
{
	if ( InitGLEW() != GLEW_OK )
		return false;

    return (glewIsSupported(extName)>0);
}
#else
bool ccOpenGLUtils::CheckExtension(const char *extName)
{
   QOpenGLContext *context = QOpenGLContext::currentContext();
   
   if ( context == NULL )
      return false;
   
   return context->hasExtension( extName );
}
#endif


bool ccOpenGLUtils::CheckShadersAvailability()
{
    bool bARBShadingLanguage       = CheckExtension("GL_ARB_shading_language_100");
    bool bARBShaderObjects         = CheckExtension("GL_ARB_shader_objects");
    bool bARBVertexShader          = CheckExtension("GL_ARB_vertex_shader");
    bool bARBFragmentShader        = CheckExtension("GL_ARB_fragment_shader");

    bool bShadersSupported = bARBShadingLanguage &&
                             bARBShaderObjects &&
                             bARBVertexShader &&
                             bARBFragmentShader;

    return bShadersSupported;
}

bool ccOpenGLUtils::CheckFBOAvailability()
{
    return CheckExtension("GL_EXT_framebuffer_object");
}

bool ccOpenGLUtils::CheckVBOAvailability()
{
    return CheckExtension("GL_ARB_vertex_buffer_object");
}
