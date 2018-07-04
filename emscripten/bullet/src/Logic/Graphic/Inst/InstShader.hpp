

#pragma once


#include "../Shader.hpp"


class InstShader
{
private:
    Shader*	m_pShader = nullptr;

public:
    InstShader() = default;

public:
    bool initialise();

public:
    inline const Shader&    getShader() const { return *m_pShader; }
};
