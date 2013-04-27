exports.onload = function(localData, globalData) {
	
};

/*exports.onleave = function() {
	saveLevel(null, null);
};

exports.onpause = function(paused) {
	if(!paused) {
		var view = new GUIFramedView(GUIScreenView());
		view.width = 200;
		var input = new GUITextField(view);
		input.text = "lol";
		view.updateContent();
	}
};*/

exports.ongametick = function() {
	var Cam = require('Cam');

	var transform = Cam.camObject.transformation(),
		speed = animationFactor*5.0;

	if(Keyboard.isKeyPressed(97)) //A
		transform.w(transform.w().add(transform.x().mult(-speed)));
	else if(Keyboard.isKeyPressed(100)) //D
		transform.w(transform.w().add(transform.x().mult(speed)));

	if(Keyboard.isKeyPressed(101)) //E
		transform.w(transform.w().add(transform.y().mult(-speed)));
	else if(Keyboard.isKeyPressed(113)) //Q
		transform.w(transform.w().add(transform.y().mult(speed)));

	if(Keyboard.isKeyPressed(119)) //W
		transform.w(transform.w().add(transform.z().mult(-speed)));
	else if(Keyboard.isKeyPressed(115)) //S
		transform.w(transform.w().add(transform.z().mult(speed)));

	if(!Keyboard.isKeyPressed(308)) { //Alt left
		Cam.camObject.rotation[0] += Mouse.x()*0.01;
		Cam.camObject.rotation[1] = Math.min(Math.max(Cam.camObject.rotation[1]+Mouse.y()*0.01, -Math.PI/2), Math.PI/2);
		transform.setRotation(new Quaternion(Cam.camObject.rotation));
	}

	Cam.camObject.transformation(transform);

	if(exports.grabbedObject != null) {
		var camTransform = require('Cam').camObject.transformation(),
			objTransform = exports.grabbedObject.transformation();
		var velocity = camTransform.getTransformed(exports.grabbedVector).sub(objTransform.w()),
			speed = velocity.getLength();
        velocity.mult(1.0/speed);
        speed = Math.min(speed*5.0, 10.0);
        
        if(Keyboard.isKeyPressed(308)) //Alt left
            exports.grabbedObject.angularVelocity(camTransform.getRotated(new Vector3(Mouse.y(), Mouse.x(), 0.0)).mult(-0.1));
        else
            exports.grabbedObject.angularVelocity(new Vector3(0.0, 0.0, 0.0));
        exports.grabbedObject.linearVelocity(velocity.mult(speed));

        if(Keyboard.isKeyPressed(304)) { //Shift left: Explode
		var result = Intersection.sphereIntersection(exports.grabbedObject.transformation().w(), 10.0, 0xFFFF);
			for(var i = 0; i < result.length; i ++)
				if(result[i].mass && result[i] != exports.grabbedObject) {
					var vec = result[i].transformation().w().sub(exports.grabbedObject.transformation().w());
					vec.mult(120.0/vec.getLength());
					result[i].applyLinearImpulse(vec);
				}
			exports.grabbedObject.integrity = 0.0;
			exports.grabbedObject = null;
		}
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
	var transform = require('Cam').camObject.transformation();
	var hits = Intersection.rayCast(transform.w(), transform.z().mult(-100.0), 0xFFFF, true);
	if(hits.objects.length == 0 || !hits.objects[0].mass)
		exports.grabbedObject = null;
	else{
		exports.grabbedObject = hits.objects[0];
		exports.grabbedVector = new Vector3(0, 0, -exports.grabbedObject.transformation().w().sub(transform.w()).getLength());
	}
};

exports.onmouseup = function() {
	exports.grabbedObject = null;
};