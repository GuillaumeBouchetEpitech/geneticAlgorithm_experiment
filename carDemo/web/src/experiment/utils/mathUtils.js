
class MathUtils {

	static collisionDroiteSeg(A, B, O, P) {

		const	AO = {},
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

	static collisionSegSeg_partial(A, B, O, P) {

		// THIS IS SLOWER
		// if (!collisionDroiteSeg_BBox(A,B,O,P))
		// 	return 1.0;
		// /THIS IS SLOWER

		if (!MathUtils.collisionDroiteSeg(A,B,O,P))
			return 1.0;

		const	AB = {},
				OP = {};

		AB.x = B.x - A.x;
		AB.y = B.y - A.y;
		OP.x = P.x - O.x;
		OP.y = P.y - O.y;

		return -(A.x * OP.y - O.x * OP.y - OP.x * A.y + OP.x * O.y) / (AB.x * OP.y - AB.y * OP.x);
	}

	static collisionSegSeg(A, B, O, P) {

		const k = MathUtils.collisionSegSeg_partial(A, B, O, P);

		return (!(k < 0.0 || k > 1.0));
	}

	/////////////////////////////////////////////
	/////////////////////////////////////////////
	/////////////////////////////////////////////

	static collisionPointCercle(P, C, radius) {

		const d2 = (P.x - C.x) * (P.x - C.x) + (P.y - C.y) * (P.y - C.y);

		return (d2 <= radius * radius);
	}

	static collisionDroiteCercle_BBox(A,  B, C, radius) {

		const minAB = {x:Math.min(A.x, B.x), y:Math.min(A.y, B.y)};
		const maxAB = {x:Math.max(A.x, B.x), y:Math.max(A.y, B.y)};
		const minC = {x:C.x - radius, y:C.y - radius};
		const maxC = {x:C.x + radius, y:C.y + radius};

		return !(maxAB.x < minC.x ||
				 minAB.x > maxC.x ||
				 maxAB.y < minC.y ||
				 minAB.y > maxC.y);
	}

	static collisionDroiteCercle(A,  B, C, radius) {

		if (!MathUtils.collisionDroiteCercle_BBox(A,B,C,radius))
			return false;

		const	u = {x:B.x - A.x, y:B.y - A.y};
		const	AC = {x:C.x - A.x, y:C.y - A.y};

		let numerateur = u.x * AC.y - u.y * AC.x;   // norme du vecteur v

		if (numerateur < 0)
			numerateur = -numerateur ;   // valeur absolue ; si c'est négatif, on prend l'opposé.

		const denominateur = Math.sqrt(u.x * u.x + u.y * u.y);  // norme de u
		const CI = numerateur / denominateur;

		return (CI < radius);
	}

	static collisionSegmentCercle(A,  B, C, radius) {

		if (MathUtils.collisionDroiteCercle(A, B, C, radius) == false)
			return false;  // si on ne touche pas la droite, on ne touchera jamais le segment

		const AB = {},
			AC = {},
			BC = {};

		AB.x = B.x - A.x;
		AB.y = B.y - A.y;
		AC.x = C.x - A.x;
		AC.y = C.y - A.y;
		BC.x = C.x - B.x;
		BC.y = C.y - B.y;

		const pscal1 = AB.x * AC.x + AB.y * AC.y;  // produit scalaire
		const pscal2 = (-AB.x) * BC.x + (-AB.y) * BC.y;  // produit scalaire

		if (pscal1 >= 0 && pscal2 >= 0)
			return true;   // I entre A et B, ok.

		// dernière possibilité, A ou B dans le cercle
		if (MathUtils.collisionPointCercle(A, C, radius) ||
			MathUtils.collisionPointCercle(B, C, radius))
			return true;

		return false;
	}

	static rotateVec2(point, center, angle) {

		const	newPos = {};

		const cos_a = Math.cos(angle);
		const sin_a = Math.sin(angle);

		newPos.x = center.x + (point.x-center.x) * cos_a + (point.y - center.y) * sin_a;
		newPos.y = center.y + (point.x-center.x) * sin_a - (point.y - center.y) * cos_a;

		return newPos;
	}

	static lerp(p1, p2, ratio) {

		const ret_val = {};
		ret_val.x = p1.x + (p2.x - p1.x) * ratio;
		ret_val.y = p1.y + (p2.y - p1.y) * ratio;
		return ret_val;
	}
}

//

export default MathUtils;
