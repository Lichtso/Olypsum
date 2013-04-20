exports.onload = function(data) {
	this.setMainCam();
	exports.camObject = this;
	this.rotation = this.transformation().getEuler();
	this.rotation[2] = 0.0;
	//log(this.rotation);
};

/*
exports.onsave = function() {
	return "";
};*/