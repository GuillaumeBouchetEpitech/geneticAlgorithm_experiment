

#include "collision.hpp"


#include <algorithm> // std::min/max


//
// UNUSED AS SLOWER

// bool CollisionLineSeg_BBox(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
// {
// 	t_vec2f minAB(std::min(A.x, B.x), std::min(A.y, B.y));
// 	t_vec2f minOP(std::min(O.x, P.x), std::min(O.y, P.y));
// 	t_vec2f maxAB(std::max(A.x, B.x), std::max(A.y, B.y));
// 	t_vec2f maxOP(std::max(O.x, P.x), std::max(O.y, P.y));

// 	return !(maxAB.x < minOP.x ||
// 			 minAB.x > maxOP.x ||
// 			 maxAB.y < minOP.y ||
// 			 minAB.y > maxOP.y);
// }

// UNUSED AS SLOWER
//

bool CollisionLineSeg(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	t_vec2f AO,AP,AB;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	AP.x = P.x - A.x;
	AP.y = P.y - A.y;
	AO.x = O.x - A.x;
	AO.y = O.y - A.y;

	return ((AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0f);
}

float CollisionSegmentSegment_partial(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	// THIS IS SLOWER
	// if (!CollisionLineSeg_BBox(A,B,O,P))
	// 	return 1.0f;
	// /THIS IS SLOWER

	if (!CollisionLineSeg(A,B,O,P))
		return 1.0f;

	t_vec2f AB,OP;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	OP.x = P.x - O.x;
	OP.y = P.y - O.y;

	return -(A.x * OP.y - O.x * OP.y - OP.x * A.y + OP.x * O.y) / (AB.x * OP.y - AB.y * OP.x);
}

bool CollisionSegmentSegment(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	float k = CollisionSegmentSegment_partial(A, B, O, P);

	return (!(k < 0.0f || k > 1.0f));
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

bool	CollisionPointCercle(const t_vec2f& P, const t_vec2f& C, float radius)
{
	int d2 = (P.x - C.x) * (P.x - C.x) + (P.y - C.y) * (P.y - C.y);

	return (d2 <= radius * radius);
}

bool	CollisionLineCercle_BBox(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	t_vec2f minAB(std::min(A.x, B.x), std::min(A.y, B.y));
	t_vec2f maxAB(std::max(A.x, B.x), std::max(A.y, B.y));
	t_vec2f minC(C.x - radius, C.y - radius);
	t_vec2f maxC(C.x + radius, C.y + radius);

	return !(maxAB.x < minC.x ||
			 minAB.x > maxC.x ||
			 maxAB.y < minC.y ||
			 minAB.y > maxC.y);
}

bool	CollisionLineCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	if (!CollisionLineCercle_BBox(A,B,C,radius))
		return false;

	t_vec2f	u(B.x - A.x, B.y - A.y);
	t_vec2f	AC(C.x - A.x, C.y - A.y);

	float numerateur = u.x * AC.y - u.y * AC.x;   // norme du vecteur v

	if (numerateur < 0)
		numerateur = -numerateur ;   // valeur absolue ; si c'est négatif, on prend l'opposé.

	float denominateur = sqrtf(u.x * u.x + u.y * u.y);  // norme de u
	float CI = numerateur / denominateur;

	return (CI < radius);
}

bool	CollisionSegmentCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	if (CollisionLineCercle(A, B, C, radius) == false)
		return false;  // si on ne touche pas la droite, on ne touchera jamais le segment

	t_vec2f AB, AC, BC;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	AC.x = C.x - A.x;
	AC.y = C.y - A.y;
	BC.x = C.x - B.x;
	BC.y = C.y - B.y;

	float pscal1 = AB.x * AC.x + AB.y * AC.y;  // produit scalaire
	float pscal2 = (-AB.x) * BC.x + (-AB.y) * BC.y;  // produit scalaire

	if (pscal1 >= 0 && pscal2 >= 0)
		return true;   // I entre A et B, ok.

	// dernière possibilité, A ou B dans le cercle
	if (CollisionPointCercle(A, C, radius) ||
		CollisionPointCercle(B, C, radius))
		return true;

	return false;
}

