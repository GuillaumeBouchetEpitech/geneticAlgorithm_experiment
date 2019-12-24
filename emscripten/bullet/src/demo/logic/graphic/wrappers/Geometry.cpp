
#include "Geometry.hpp"

#include "Shader.hpp"

#include "demo/utilities/ErrorHandler.hpp"

//

void    Geometry::initialise(Shader& shader, const t_def& def)
{
    if (_vao.isAllocated() || _vbo.isAllocated())
        D_THROW(std::runtime_error, "vao/vbo already allocated");

    if (def.vbos.empty())
        D_THROW(std::runtime_error, "empty vbo defintion");

    for (const auto& vbo : def.vbos)
    {
        const auto& attrs = vbo.attrs;

        if (attrs.empty())
            D_THROW(std::runtime_error, "empty vbo attribute defintion");

        for (const auto& attr : attrs)
            if (!shader.hasAttribute(attr.name))
                D_THROW(std::runtime_error, "attribute not found, name=\"" << attr.name << "\"");
    }

    _def = def;

    _primitiveType = def.primitiveType;

    _vao.allocate();
    _vao.bind();

    _vbo.allocate(_def.vbos.size());

    for (unsigned int ii = 0; ii < _def.vbos.size(); ++ii)
    {
        const auto& vbo = _def.vbos[ii];
        const auto& attrs = vbo.attrs;

        _vbo.bind(ii);

        unsigned int stride = vbo.stride;
        if (stride == 0)
        {
            // auto determine stride value
            for (const auto& attr : attrs)
            {
                switch (attr.type)
                {
                case e_attrType::eFloat: stride += 1; break;
                case e_attrType::eVec2f: stride += 2; break;
                case e_attrType::eVec3f: stride += 3; break;
                case e_attrType::eVec4f: stride += 4; break;
                case e_attrType::eMat3f: stride += 9; break;
                case e_attrType::eMat4f: stride += 16; break;
                }
            }
            stride *= sizeof(float);
        }

        for (unsigned int jj = 0; jj < attrs.size(); ++jj)
        {
            const auto& attr = attrs[jj];

            int rowSize = 1;
            int totalRows = 1;
            switch (attr.type)
            {
                case e_attrType::eFloat:
                    rowSize = 1;
                    break;
                case e_attrType::eVec2f:
                    rowSize = 2;
                    break;
                case e_attrType::eVec3f:
                    rowSize = 3;
                    break;
                case e_attrType::eVec4f:
                    rowSize = 4;
                    break;
                case e_attrType::eMat3f:
                    rowSize = 3;
                    totalRows = 3;
                    break;
                case e_attrType::eMat4f:
                    rowSize = 4;
                    totalRows = 4;
                    break;
            }

            GLint attrLocation = shader.getAttribute(attr.name);

            // TODO: check if the index is 0 on k>0 and assert/throw on it

            for (int kk = 0; kk < totalRows; ++kk)
            {
                int     attrId = attrLocation + kk;
                int     rowIndex = (attr.index + kk * rowSize) * sizeof(float);
                void*   rowAddr = reinterpret_cast<void*>(rowIndex);

                glEnableVertexAttribArray(attrId);
                glVertexAttribPointer(attrId, rowSize, GL_FLOAT, GL_FALSE, stride, rowAddr);
                if (vbo.instanced)
                {
                    glVertexAttribDivisor(attrId, 1);

                    if (!_isInstanced)
                        _isInstanced = true;
                }
            }
        }
    }

    _vao.unbind();
}

void Geometry::updateBuffer(int index, const void* data, int dataSize, bool dynamic /*= false*/) const
{
    if (!_vao.isAllocated() || !_vbo.isAllocated())
        D_THROW(std::runtime_error, "vao/vbo not allocated");

    _vbo.bind(index);

    GLenum usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);

    VertexBufferObject::unbind();
}

void Geometry::render() const
{
    if (!_vao.isAllocated() || !_vbo.isAllocated())
        D_THROW(std::runtime_error, "vao/vbo not allocated");

    if (_primitiveCount == 0 || (_isInstanced && _instanceCount == 0))
        return;

    _vao.bind();

    if (_isInstanced)
        glDrawArraysInstanced(_primitiveType, _primitiveStart, _primitiveCount, _instanceCount);
    else
        glDrawArrays(_primitiveType, _primitiveStart, _primitiveCount);

    _vao.unbind();
}

void Geometry::setPrimitiveStart(unsigned int start)
{
    _primitiveStart = start;
}

void Geometry::setPrimitiveCount(unsigned int count)
{
    _primitiveCount = count;
}

void Geometry::setInstancedCount(unsigned int count)
{
    _instanceCount = count;
}
