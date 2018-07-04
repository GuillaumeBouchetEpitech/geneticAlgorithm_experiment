

#pragma once


class Shader;

class	GeometryColor;

class	ShaderColor
{
private:
	friend GeometryColor;

private:
	Shader*	m_pShader = nullptr;

public:
	ShaderColor() = default;
	~ShaderColor();

public:
	bool	initialise();

public:
	const Shader*	getShader() const { return m_pShader; }
};
