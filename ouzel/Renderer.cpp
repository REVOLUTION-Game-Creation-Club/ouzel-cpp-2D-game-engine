// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"
#include "Node.h"
#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "EventHander.h"
#include "SceneManager.h"
#include "MeshBuffer.h"
#include "EventDispatcher.h"
#include "RenderTarget.h"

namespace ouzel
{
    Renderer::Renderer()
    {
        
    }

    Renderer::~Renderer()
    {
        
    }

    bool Renderer::init(const Size2& size, bool resizable, bool fullscreen, Driver driver)
    {
        _driver = driver;
        _size = size;
        _resizable = resizable;
        _fullscreen = fullscreen;

        return true;
    }
    
    void Renderer::clear()
    {
        _drawCallCount = 0;
    }
    
    void Renderer::flush()
    {
    }
    
    void Renderer::resize(const Size2& size)
    {
        if (_size != size)
        {
            _size = size;
            Engine::getInstance()->getSceneManager()->recalculateProjection();
            
            WindowEventPtr event = std::make_shared<WindowEvent>();
            event->type = Event::Type::WINDOW_SIZE_CHANGE;
            event->size = _size;
            event->title = _title;
            
            Engine::getInstance()->getEventDispatcher()->dispatchEvent(event, Engine::getInstance()->getRenderer());
        }
    }
    
    void Renderer::setTitle(const std::string& title)
    {
        if (_title != title)
        {
            _title = title;
            
            WindowEventPtr event = std::make_shared<WindowEvent>();
            event->type = Event::Type::WINDOW_TITLE_CHANGE;
            event->size = _size;
            event->title = _title;
            
            Engine::getInstance()->getEventDispatcher()->dispatchEvent(event, Engine::getInstance()->getRenderer());
        }
    }
    
    TexturePtr Renderer::createTexture(const Size2& size, bool dynamic, bool mipmaps)
    {
        TexturePtr texture(new Texture());
        texture->init(size, dynamic);
        
        return texture;
    }
    
    bool Renderer::activateTexture(const TexturePtr& texture, uint32_t layer)
    {
        _activeTextures[layer] = texture;
        
        return true;
    }
    
    TexturePtr Renderer::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
    {
        TexturePtr texture(new Texture());
        
        if (!texture->initFromFile(filename, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    TexturePtr Renderer::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
    {
        TexturePtr texture(new Texture());
        
        if (!texture->initFromData(data, size, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    RenderTargetPtr Renderer::createRenderTarget(const Size2& size, bool depthBuffer)
    {
        RenderTargetPtr renderTarget(new RenderTarget());
        
        if (!renderTarget->init(size, depthBuffer))
        {
            renderTarget.reset();
        }
        
        return renderTarget;
    }
    
    bool Renderer::activateRenderTarget(const RenderTargetPtr& renderTarget)
    {
        _activeRenderTarget = renderTarget;
        
        return true;
    }
    
    ShaderPtr Renderer::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        ShaderPtr shader(new Shader());
        
        if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    ShaderPtr Renderer::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        ShaderPtr shader(new Shader());
        
        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    bool Renderer::activateShader(const ShaderPtr& shader)
    {
        _activeShader = shader;
        
        return true;
    }
    
    MeshBufferPtr Renderer::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        MeshBufferPtr meshBuffer(new MeshBuffer());
        
        if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            meshBuffer.reset();
        }
        
        return meshBuffer;
    }
    
    bool Renderer::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode)
    {
        OUZEL_UNUSED(drawMode);
        
        if (_activeShader)
        {
            if (meshBuffer->getVertexAttributes() != _activeShader->getVertexAttributes())
            {
                return false;
            }
            
            if (indexCount > meshBuffer->getIndexCount())
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        
        _drawCallCount++;
        
        return true;
    }
    
    Vector2 Renderer::viewToScreenLocation(const Vector2& position)
    {
        float x = 2.0f * position.x / _size.width - 1.0f;
        float y = 2.0f * (_size.height - position.y) / _size.height - 1.0f;
        
        return Vector2(x, y);
    }
    
    Vector2 Renderer::screenToViewLocation(const Vector2& position)
    {
        float x = (position.x + 1.0f) / 2.0f * _size.width;
        float y = _size.height - (position.y + 1.0f) / 2.0f * _size.height;
        
        return Vector2(x, y);
    }
    
    bool Renderer::saveScreenshot(const std::string& filename)
    {
        OUZEL_UNUSED(filename);
        
        return true;
    }
}
