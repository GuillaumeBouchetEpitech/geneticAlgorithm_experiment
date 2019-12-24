
class MathUtils {

	static collisionDroiteSeg(A, B, O, P) {

		const	AB = { x: B.x - A.x, y: B.y - A.y };
		const	AP = { x: P.x - A.x, y: P.y - A.y };
		const	AO = { x: O.x - A.x, y: O.y - A.y };

		return ((AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0);
	}

	static collisionSegSeg_partial(A, B, O, P) {

		if (!MathUtils.collisionDroiteSeg(A,B,O,P))
			return 1.0;

		const AB = { x: B.x - A.x, y: B.y - A.y };
		const OP = { x: P.x - O.x, y: P.y - O.y };

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

	static collisionDroiteCercle_BBox(A, B, C, radius) {

		const minAB = { x: Math.min(A.x, B.x), y: Math.min(A.y, B.y) };
		const maxAB = { x: Math.max(A.x, B.x), y: Math.max(A.y, B.y) };
		const minC = { x: C.x - radius, y: C.y - radius };
		const maxC = { x: C.x + radius, y: C.y + radius };

		return !(
			maxAB.x < minC.x ||
			minAB.x > maxC.x ||
			maxAB.y < minC.y ||
			minAB.y > maxC.y
		);
	}

	static collisionDroiteCercle(A, B, C, radius) {

		if (!MathUtils.collisionDroiteCercle_BBox(A, B, C, radius))
			return false;

		const U = { x: B.x - A.x, y: B.y - A.y };
		const AC = { x: C.x - A.x, y: C.y - A.y };

		const vNorm = Math.abs(U.x * AC.y - U.y * AC.x);
		const uNorm = Math.sqrt(U.x * U.x + U.y * U.y);
		const CI = vNorm / uNorm;

		return (CI < radius);
	}

	static collisionSegmentCercle(A, B, C, radius) {

		if (!MathUtils.collisionDroiteCercle(A, B, C, radius))
			return false;

		const AB = { x: B.x - A.x, y: B.y - A.y };
		const AC = { x: C.x - A.x, y: C.y - A.y };
		const BC = { x: C.x - B.x, y: C.y - B.y };

		const scalProd1 = AB.x * AC.x + AB.y * AC.y; // scalar product
		const scalProd2 = (-AB.x) * BC.x + (-AB.y) * BC.y; // scalar product

		if (scalProd1 >= 0 && scalProd2 >= 0)
			return true; // <= I is between A and B

		// last possibility, A ou B inside the circle C
		return (
			MathUtils.collisionPointCercle(A, C, radius) ||
			MathUtils.collisionPointCercle(B, C, radius)
		);
	}

	static rotateVec2(point, center, angle) {

		const cos_a = Math.cos(angle);
		const sin_a = Math.sin(angle);

		return {
			x: center.x + (point.x - center.x) * cos_a + (point.y - center.y) * sin_a,
			y: center.y + (point.x - center.x) * sin_a - (point.y - center.y) * cos_a
		};
	}

	static lerp(p1, p2, ratio) {
		return {
			x: p1.x + (p2.x - p1.x) * ratio,
			y: p1.y + (p2.y - p1.y) * ratio
		};
	}

	static clamp(value, min, max) {
		return Math.max(min, Math.min(max, value));
	}
}

//

module.exports = MathUtils;
