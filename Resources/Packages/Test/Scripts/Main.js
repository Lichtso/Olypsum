exports.explosion = function(transform) {
	var radius = 5, result = Intersection.sphereIntersection(transform.w(), radius, 0xFFFF);
	for(var i = 0; i < result.length; i ++) {
		if(result[i].mass && result[i] != exports.grabbedObject) { //Push other objects away
			var vec = result[i].transformation().w().sub(transform.w());
			vec.mult(120.0/vec.getLength());
			result[i].applyLinearImpulse(vec);
		}

		if(result[i].className == "TerrainObject") { //Hole in ground
			var mat = result[i].transformation().getInverse().mult(transform),
				size = result[i].collisionShapeInfo().size,
				pos = mat.w().divide(size).mult(0.5).add(new Vector3(0.5, 0.5, 0.5)),
				centerX = Math.round(pos.x*result[i].width), centerY = Math.round(pos.z*result[i].length);
			radius = radius*0.25*result[i].width/size.x;
			for(var y = Math.max(centerY-radius, 0); y < Math.min(centerY+radius+1, result[i].length); y ++)
				for(var x = Math.max(centerX-radius, 0); x < Math.min(centerX+radius+1, result[i].width); x ++) {
					var diffX = x-centerX, diffY = y-centerY,
						dist = Math.sqrt(diffX*diffX+diffY*diffY)/radius;
					if(dist < 1.0)
						result[i][y*result[i].width+x] = Math.min(result[i][y*result[i].width+x], Math.max(0.0, pos.y-Math.cos(Math.asin(dist))/size.y));
				}
			result[i].updateModel();
		}
	}

	//Spawn effects
	var mat = new Matrix4();
	mat.w(transform.w());
	var imported = loadContainer(mat, "explosion");
	Animation.addFrames(imported[1], "color", false, [1.0, 1.0], [0.0, 1.5], [null, new Vector3(0.0, 0.0, 0.0)]);
}

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
		var rotation = transform.rotation(), up = new Vector3(0.0, 1.0, 0.0);
		transform.rotation(rotation.getProduct(new Quaternion(Mouse.x()*0.01, Mouse.y()*0.01, 0.0)));
		transform.y(up);
		transform.x(transform.y().cross(transform.z()).normalize());
		transform.y(transform.z().cross(transform.x()).normalize());
		if(transform.y().getDot(up) < 0.12)
			transform.rotation(rotation.mult(new Quaternion(Mouse.x()*0.01, 0.0, 0.0)));
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
			exports.explosion(exports.grabbedObject.transformation());
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