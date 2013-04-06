exports.onload = function(localData, globalData) {
	log(levelID);
};

/*exports.onleave = function() {
	saveLevel(null, null);
};*/

exports.ongametick = function() {
	var Cam = require('Cam');
	Cam.ongametick.call(Cam.camObject);

	if(exports.grabbedObject != null) {
		var camTransform = require('Cam').camObject.transformation;
		var objTransform = exports.grabbedObject.transformation;
		var velocity = camTransform.getTransformed(exports.grabbedVector).sub(objTransform.w);
        var speed = velocity.getLength();
        velocity.mult(1.0/speed);
        speed = Math.min(speed*5.0, 10.0);
        
        if(isKeyPressed(308))
            exports.grabbedObject.setAngularVelocity(camTransform.getRotated(new Vector3(mouseMotionY, mouseMotionX, 0.0)).mult(-0.1));
        else
            exports.grabbedObject.setAngularVelocity(new Vector3(0.0, 0.0, 0.0));
        exports.grabbedObject.setLinearVelocity(velocity.mult(speed));
	}
};

exports.onmousewheel = function(delta) {
	if(exports.grabbedObject != null) {
		var length = exports.grabbedVector.getLength();
		exports.grabbedVector.mult(1.0/length);
		length = Math.max(1.0, length-delta*0.5);
		exports.grabbedVector.mult(length);
	}
};

exports.onmousedown = function() {
	var transform = require('Cam').camObject.transformation;
	var hits = Intersection.rayCast(transform.w, transform.z.mult(-100.0), 0xFFFF, true);
	if(hits.objects.length == 0 || !hits.objects[0].mass)
		exports.grabbedObject = null;
	else{
		exports.grabbedObject = hits.objects[0];
		exports.grabbedVector = new Vector3(0, 0, -exports.grabbedObject.transformation.w.sub(transform.w).getLength());
	}
};

exports.onmouseup = function() {
	exports.grabbedObject = null;
};