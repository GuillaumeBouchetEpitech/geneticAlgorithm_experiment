

#pragma once


#include <Bullet/btBulletDynamicsCommon.h>


class BulletDebugDraw
	: public btIDebugDraw
{
private:
	int	m_debugMode;

public:
	BulletDebugDraw();
	~BulletDebugDraw();

public:
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;

	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;

	virtual void	reportErrorWarning(const char* warningString) override;

	virtual void	draw3dText(const btVector3& location,const char* textString) override;
	
	virtual void	setDebugMode(int debugMode) override
	{
		m_debugMode = debugMode;
	}
	
	virtual int		getDebugMode() const override
	{
		return m_debugMode;
	}

};
