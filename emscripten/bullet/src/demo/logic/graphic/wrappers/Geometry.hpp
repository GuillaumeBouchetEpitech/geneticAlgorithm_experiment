
#pragma once

#include "VertexBufferObject.hpp"
#include "VertexArrayObject.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include <string>
#include <vector>

class ShaderProgram;

class Geometry
{
public:
    enum class AttrType
    {
        Float,
        Vec2f,
        Vec3f,
        Vec4f,
        Mat3f,
        Mat4f
    };

    struct Definition
    {
        struct Vbo
        {
            struct Attr
            {
                std::string name = "";
                AttrType    type = AttrType::Float;
                std::size_t index = 0;

                Attr() = default;

                Attr(const std::string& name, AttrType type, std::size_t index)
                    : name(name)
                    , type(type)
                    , index(index)
                {}
            };

            unsigned int    stride = 0;
            bool            instanced = false;

            std::vector<Attr> attrs;

            Vbo() = default;
        };

        std::vector<Vbo> vbos;

        GLint primitiveType = GL_LINES;
    };

private:
    Definition          _def;
    VertexArrayObject   _vao;
    VertexBufferObject  _vbo;
    GLint               _primitiveType = GL_LINES;
    unsigned int        _primitiveStart = 0;
    unsigned int        _primitiveCount = 0;
    unsigned int        _instanceCount = 0;
    bool                _isInstanced = false;

public:
    Geometry() = default;

public:
    void initialise(ShaderProgram& shader, const Definition& def);

public:
    void updateBuffer(int index, const void* data, int dataSize, bool dynamic = false) const;
    void render() const;

public:
    template <typename VertexType>
    void updateBuffer(int index, const std::vector<VertexType>& data, bool dynamic = false) const
    {
        updateBuffer(index, static_cast<const void*>(data.data()), data.size() * sizeof(VertexType), dynamic);
    }

    void setPrimitiveStart(unsigned int start);
    void setPrimitiveCount(unsigned int count);
    void setInstancedCount(unsigned int count);
};
