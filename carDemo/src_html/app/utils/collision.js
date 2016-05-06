

define([],function()
{
	var self = {};

	//

	self.CollisionDroiteSeg_BBox = function(A, B, O, P)
	{
		var minAB = {x:Math.min(A.x, B.x), y:Math.min(A.y, B.y)};
		var minOP = {x:Math.min(O.x, P.x), y:Math.min(O.y, P.y)};
		var maxAB = {x:Math.max(A.x, B.x), y:Math.max(A.y, B.y)};
		var maxOP = {x:Math.max(O.x, P.x), y:Math.max(O.y, P.y)};

		return !(maxAB.x < minOP.x ||
				 minAB.x > maxOP.x ||
				 maxAB.y < minOP.y ||
				 minAB.y > maxOP.y);
	}

	self.CollisionDroiteSeg = function(A, B, O, P)
	{
		var AO = {},
			AP = {},
			AB = {};

		AB.x = B.x - A.x;
		AB.y = B.y - A.y;
		AP.x = P.x - A.x;
		AP.y = P.y - A.y;
		AO.x = O.x - A.x;
		AO.y = O.y - A.y;

		return ((AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0);
	}

	self.CollisionSegSeg_partial = function(A, B, O, P)
	{
		// THIS IS SLOWER
		// if (!CollisionDroiteSeg_BBox(A,B,O,P))
		// 	return 1.0;
		// /THIS IS SLOWER

		if (!self.CollisionDroiteSeg(A,B,O,P))
			return 1.0;

		var AB = {},
			OP = {};

		AB.x = B.x - A.x;
		AB.y = B.y - A.y;
		OP.x = P.x - O.x;
		OP.y = P.y - O.y;

		return -(A.x * OP.y - O.x * OP.y - OP.x * A.y + OP.x * O.y) / (AB.x * OP.y - AB.y * OP.x);
	}

	self.CollisionSegSeg = function(A, B, O, P)
	{
		var k = self.CollisionSegSeg_partial(A, B, O, P);

		return (!(k < 0.0 || k > 1.0));
	}

	/////////////////////////////////////////////
	/////////////////////////////////////////////
	/////////////////////////////////////////////

	self.CollisionPointCercle = function(P, C, radius)
	{
		var d2 = (P.x - C.x) * (P.x - C.x) + (P.y - C.y) * (P.y - C.y);

		return (d2 <= radius * radius);
	}

	self.CollisionDroiteCercle_BBox = function(A,  B, C, radius)
	{
		var minAB = {x:Math.min(A.x, B.x), y:Math.min(A.y, B.y)};
		var maxAB = {x:Math.max(A.x, B.x), y:Math.max(A.y, B.y)};
		var minC = {x:C.x - radius, y:C.y - radius};
		var maxC = {x:C.x + radius, y:C.y + radius};

		return !(maxAB.x < minC.x ||
				 minAB.x > maxC.x ||
				 maxAB.y < minC.y ||
				 minAB.y > maxC.y);
	}

	self.CollisionDroiteCercle = function(A,  B, C, radius)
	{
		if (!self.CollisionDroiteCercle_BBox(A,B,C,radius))
			return false;

		var	u = {x:B.x - A.x, y:B.y - A.y};
		var	AC = {x:C.x - A.x, y:C.y - A.y};

		var numerateur = u.x * AC.y - u.y * AC.x;   // norme du vecteur v

		if (numerateur < 0)
			numerateur = -numerateur ;   // valeur absolue ; si c'est négatif, on prend l'opposé.

		var denominateur = Math.sqrt(u.x * u.x + u.y * u.y);  // norme de u
		var CI = numerateur / denominateur;

		return (CI < radius);
	}

	self.CollisionSegmentCercle = function(A,  B, C, radius)
	{
		if (self.CollisionDroiteCercle(A, B, C, radius) == false)
			return false;  // si on ne touche pas la droite, on ne touchera jamais le segment

		var AB = {},
			AC = {},
			BC = {};

		AB.x = B.x - A.x;
		AB.y = B.y - A.y;
		AC.x = C.x - A.x;
		AC.y = C.y - A.y;
		BC.x = C.x - B.x;
		BC.y = C.y - B.y;

		var pscal1 = AB.x * AC.x + AB.y * AC.y;  // produit scalaire
		var pscal2 = (-AB.x) * BC.x + (-AB.y) * BC.y;  // produit scalaire

		if (pscal1 >= 0 && pscal2 >= 0)
			return true;   // I entre A et B, ok.

		// dernière possibilité, A ou B dans le cercle
		if (self.CollisionPointCercle(A, C, radius) ||
			self.CollisionPointCercle(B, C, radius))
			return true;

		return false;
	}

	self.rotateVec2 = function(point, center, angle)
	{
		var	newPos = {};

		var cos_a = Math.cos(angle);
		var sin_a = Math.sin(angle);

		newPos.x = center.x + (point.x-center.x) * cos_a + (point.y-center.y)*sin_a;
		newPos.y = center.y + (point.x-center.x) * sin_a - (point.y-center.y)*cos_a;

		return newPos;
	}

	self.lerp = function(p1, p2, ratio)
	{
		var ret_val = {};
		ret_val.x = p1.x + (p2.x - p1.x) * ratio;
		ret_val.y = p1.y + (p2.y - p1.y) * ratio;
		return ret_val;
	}

	//

	return self;
})









//
//
//
// UTILS



// UTILS
//
//
//



