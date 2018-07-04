

#include "BulletDebugDraw.hpp"


#include "Logic/Data.hpp"

// #include <iostream>


BulletDebugDraw::BulletDebugDraw()
	: m_debugMode(0)
{}

BulletDebugDraw::~BulletDebugDraw()
{}

void	BulletDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	Data::get()->m_StackRenderer.push_vertex(
		StackRenderer::t_pos( from[0], from[1], from[2]),
		StackRenderer::t_color(color[0],color[1],color[2],1)
	);

	Data::get()->m_StackRenderer.push_vertex(
		StackRenderer::t_pos( to[0], to[1], to[2]),
		StackRenderer::t_color(color[0],color[1],color[2],1)
	);
}

void	BulletDebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{}

void	BulletDebugDraw::reportErrorWarning(const char* warningString)
{}

void	BulletDebugDraw::draw3dText(const btVector3& location,const char* textString)
{}
