exports.onload = function(data) {
	exports.object = this;
	exports.steering = 0.0;
};

exports.init = function() {
	exports.wheels = {};
	exports.object.iterateLinks(function(link) {
		if(!(link instanceof Dof6PhysicLink)) return;
		var pos = link.frameA().w(), name = (pos.x > 0.0) ? 'L' : 'R';
		name += (pos.z > 0.0) ? 'F' : 'B';
		exports.wheels[name] = link;
	});
};

exports.targetSteering = function(target) {
	exports.steering += (target-exports.steering)*Animation.factor;
	var link = exports.wheels['LF'];
	link.angularLimitMin(new Vector3(1.0, exports.steering, 0.0));
	link.angularLimitMax(new Vector3(-1.0, exports.steering, 0.0));
	link = exports.wheels['RF'];
	link.angularLimitMin(new Vector3(1.0, exports.steering, 0.0));
	link.angularLimitMax(new Vector3(-1.0, exports.steering, 0.0));
};

exports.motor = function(speed) {
	for(var i in exports.wheels)
		exports.wheels[i].motorVelocity(3, speed);
};