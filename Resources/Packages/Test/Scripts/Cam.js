exports.onload = function(data) {
	this.setMainCam();
	exports.camObject = this;
	//this.rotation = this.transformation().getEuler();
	//this.rotation[2] = 0.0;
};

/*
exports.onsave = function() {
	return "";
};*/