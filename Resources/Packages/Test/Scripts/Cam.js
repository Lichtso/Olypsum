exports.onload = function(data) {
	this.setMainCam();
	exports.camObject = this;
	this.rotation = this.transformation.getEuler();
	this.rotation[2] = 0.0;
	log(this.rotation);
};

exports.ongametick = function() {
	var transform = this.transformation,
		speed = animationFactor*5.0;

	if(isKeyPressed(97)) //A
		transform.setW(transform.w.add(transform.x.mult(-speed)));
	else if(isKeyPressed(100)) //D
		transform.setW(transform.w.add(transform.x.mult(speed)));

	if(isKeyPressed(101)) //E
		transform.setW(transform.w.add(transform.y.mult(-speed)));
	else if(isKeyPressed(113)) //Q
		transform.setW(transform.w.add(transform.y.mult(speed)));

	if(isKeyPressed(119)) //W
		transform.setW(transform.w.add(transform.z.mult(-speed)));
	else if(isKeyPressed(115)) //S
		transform.setW(transform.w.add(transform.z.mult(speed)));

	if(!isKeyPressed(308)) {
		this.rotation[0] += mouseMotionX*0.01;
		this.rotation[1] = Math.min(Math.max(this.rotation[1]+mouseMotionY*0.01, -Math.PI/2), Math.PI/2);
		transform.setRotation(new Quaternion(this.rotation));
	}

	this.setTransformation(transform);
};

/*
exports.onsave = function() {
	return "";
};*/