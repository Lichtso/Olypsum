exports.onload = function(data) {
	log([this, data]);
	return true;
};

exports.onsave = function() {
	return "ObjectData";
};

exports.oncollision = function(otherObject) {
	//var result = Intersection.sphereIntersection(this.transformation.w, 15.0, 0xFFFF);
	//log(result);
	//otherObject.applyLinearImpulse(otherObject.transformation.w.getDiff(this.transformation.w).mult(10.0));
};