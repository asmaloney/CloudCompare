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

#include "ccFrameBufferObject.h"

//system
#include <assert.h>

ccFrameBufferObject::ccFrameBufferObject()
	: m_width(0)
	, m_height(0)
	, m_depthTexture(0)
	, m_colorTexture(0)
	, m_ownColorTexture(false)
	, m_fboId(0)
	, m_glFunc(nullptr)
{}

ccFrameBufferObject::~ccFrameBufferObject()
{
	reset();
}

void ccFrameBufferObject::reset()
{
	if (m_depthTexture != 0)
	{
		if (m_glFunc != nullptr)
		{
			m_glFunc->glDeleteTextures(1, &m_depthTexture);
		}
		m_depthTexture = 0;
	}

	deleteColorTexture();

	if (m_fboId != 0)
	{
#ifdef Q_OS_MAC
		mExtensionFBOFuncs.glDeleteFramebuffers(1, &m_fboId);
#else
		if (m_glFunc)
		{
			m_glFunc->glDeleteFramebuffers(1, &m_fboId);
		}
#endif
		m_fboId = 0;
	}

	m_width = m_height = 0;
}

bool ccFrameBufferObject::init(unsigned w, unsigned h, QAbstractOpenGLFunctions* glFunc)
{
	if (glFunc == nullptr)
	{
		return false;
	}

#ifdef Q_OS_MAC
	bool	init = mExtensionFBOFuncs.initializeOpenGLFunctions();
	
	if ( !init )
	{
		return false;
	}
#endif
	
	//to support reinit
	reset();

	m_width = w;
	m_height = h;
	
#ifdef Q_OS_MAC
	m_glFunc = dynamic_cast<QOpenGLFunctions_2_1 *>(glFunc);
#else
	m_glFunc = dynamic_cast<QOpenGLFunctions_3_0 *>(glFunc);
#endif
	if (glFunc == nullptr)
	{
		return false;
	}
	
	// create a framebuffer object
#ifdef Q_OS_MAC
	mExtensionFBOFuncs.glGenFramebuffers(1, &m_fboId);
#else
	m_glFunc->glGenFramebuffers(1, &m_fboId);
#endif

	return m_fboId != 0;
}

void ccFrameBufferObject::start()
{
	bindFrameBuffer( m_fboId );
}

void ccFrameBufferObject::stop()
{
	bindFrameBuffer( 0 );
}

void ccFrameBufferObject::bindFrameBuffer( GLuint framebuffer )
{
#ifdef Q_OS_MAC
	mExtensionFBOFuncs.glBindFramebuffer(GL_FRAMEBUFFER_EXT, framebuffer);
#else
	if (m_glFunc)
		m_glFunc->glBindFramebuffer(GL_FRAMEBUFFER_EXT, framebuffer);
#endif
}

GLuint ccFrameBufferObject::getID()
{
	return m_fboId;
}

void ccFrameBufferObject::deleteColorTexture()
{
	if (m_glFunc && m_colorTexture && m_glFunc->glIsTexture(m_colorTexture))
	{
		if (m_ownColorTexture)
		{
			m_glFunc->glDeleteTextures(1, &m_colorTexture);
		}
	}
	m_colorTexture = 0;
	m_ownColorTexture = false;
}

bool ccFrameBufferObject::attachColor(	GLuint texID,
										bool ownTexture/*=false*/,
										GLenum target/*=GL_TEXTURE_2D*/)
{
	if (m_fboId == 0)
	{
		assert(false);
		return false;
	}
	
	if (m_glFunc == nullptr)
	{
		assert(false);
		return false;
	}
	
	if (!m_glFunc->glIsTexture(texID))
	{
		//error or simple warning?
		assert(false);
		//return false;
	}
	
	//remove the previous texture (if any)
	deleteColorTexture();

	m_colorTexture = texID;
	m_ownColorTexture = ownTexture;

	start();

#ifdef Q_OS_MAC
	mExtensionFBOFuncs.glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID, 0);

	GLenum status = mExtensionFBOFuncs.glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
#else
	m_glFunc->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID, 0);

	GLenum status = m_glFunc->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
#endif
	
	bool success = false;
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		success = true;
		break;
	default:
		//ccLog::Warning("[FBO] Color texture %i init. error: %i", index+1, status);
		break;
	}

	stop();

	return success;
}

bool ccFrameBufferObject::initColor(	GLint internalformat,
										GLenum format,
										GLenum type,
										GLint minMagFilter /*= GL_LINEAR*/,
										GLenum target /*= GL_TEXTURE_2D*/)
{
	if (m_glFunc == nullptr)
	{
		assert(false);
		return false;
	}

	if (m_fboId == 0)
	{
		//ccLog::Warning("[FBO::initTexture] Internal error: FBO not yet initialized!");
		return false;
	}

	//even if 'attachTexture' can do this, we prefer to do it now
	//so as to release memory before creating a new texture!
	deleteColorTexture();

	//create the new texture
	GLuint texID = 0;
	m_glFunc->glGenTextures(1, &texID);

	m_glFunc->glBindTexture  (target, texID);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagFilter );
	m_glFunc->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagFilter );
	m_glFunc->glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_glFunc->glTexImage2D   (target, 0, internalformat, m_width, m_height, 0, format, type, 0);
	m_glFunc->glBindTexture  (target, 0);
	
	return attachColor(texID, true, target);
}

bool ccFrameBufferObject::initDepth(GLint wrapParam /*=GL_CLAMP_TO_BORDER*/,
									GLenum internalFormat /*=GL_DEPTH_COMPONENT24*/,
									GLint minMagFilter /*= GL_NEAREST*/,
									GLenum target/*=GL_TEXTURE_2D*/)
{
	if (m_glFunc == nullptr)
	{
		assert(false);
		return false;
	}
	
	if (m_fboId == 0)
	{
		//ccLog::Warning("[FBO::initDepth] Internal error: FBO not yet initialized!");
		return false;
	}

	start();

	if (m_glFunc->glIsTexture(m_depthTexture))
	{
		m_glFunc->glDeleteTextures(1, &m_depthTexture);
	}
	m_glFunc->glGenTextures(1, &m_depthTexture);
	m_glFunc->glBindTexture(target, m_depthTexture);

	//float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glFunc->glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapParam);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapParam);
	m_glFunc->glTexParameteri(target, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagFilter);
	m_glFunc->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagFilter);
	m_glFunc->glTexImage2D(target, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

#ifdef Q_OS_MAC
	mExtensionFBOFuncs.glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, target, m_depthTexture, 0);
#else
	m_glFunc->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, target, m_depthTexture, 0);
#endif
	
	m_glFunc->glBindTexture(target, 0);

	bool success = false;
#ifdef Q_OS_MAC
	GLenum status = mExtensionFBOFuncs.glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
#else
	GLenum status = m_glFunc->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
#endif
	
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		//ccLog::Print("[FBO] Depth init. ok");
		success = true;
		break;
	default:
		//ccLog::Warning("[FBO] Depth texture init. error: %i",status);
		break;
	}

	stop();

	return success;
}
