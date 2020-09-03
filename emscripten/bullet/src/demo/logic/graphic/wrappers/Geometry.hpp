
#pragma once

#include "VertexBufferObject.hpp"
#include "VertexArrayObject.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include <string>
#include <vector>

class Shader;

class Geometry
{
public:
    enum class e_attrType
    {
        eFloat,
        eVec2f,
        eVec3f,
        eVec4f,
        eMat3f,
        eMat4f
    };

    struct t_def
    {
        struct t_vbo
        {
            struct t_attr
            {
                std::string name = "";
                e_attrType  type = e_attrType::eFloat;
                std::size_t index = 0;

                t_attr() = default;
            };

            unsigned int    stride = 0;
            bool            instanced = false;

            std::vector<t_attr> attrs;

            t_vbo() = default;
        };

        std::vector<t_vbo> vbos;

        GLint primitiveType = GL_LINES;
    };

private:
    t_def               _def;
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
    void initialise(Shader& shader, const t_def& def);

public:
    void updateBuffer(int index, const void* data, int dataSize, bool dynamic = false) const;
    void render() const;

public:
    template <typename ElemType>
    void updateBuffer(int index, const std::vector<ElemType>& data, bool dynamic = false) const
    {
        updateBuffer(index, static_cast<const void*>(data.data()), data.size() * sizeof(ElemType), dynamic);
    }

    void setPrimitiveStart(unsigned int start);
    void setPrimitiveCount(unsigned int count);
    void setInstancedCount(unsigned int count);
};
