exports.onload = function(data) {
	log(data);
};

/*
exports.onsave = function() {
	return "ObjectData";
};
*/
exports.oncollision = function(otherObject) {
	var result = Intersection.sphereIntersection(this.transformation.w, 15.0, 0xFFFF);
	for(var i = 0; i < result.length; i ++) {
		if(result[i].mass)
			result[i].applyLinearImpulse(result[i].transformation.w.getDiff(this.transformation.w).mult(2.0));
	}
};