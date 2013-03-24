exports.onload = function(data) {
	log([this, data]);
	return true;
};

exports.onsave = function() {
	return "ObjectData";
};

exports.oncollision = function(otherObject) {
	log('oncollision: '+otherObject);
};