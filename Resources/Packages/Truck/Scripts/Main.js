var Car = Engine.getScript('Car');

exports.onload = function() {
	Car.init();
};

exports.ongametick = function() {
	if(Engine.gamePaused) return;

	if(Keyboard.isKeyPressed(4)) //E
		Car.targetSteering(-0.7);
	else if(Keyboard.isKeyPressed(7)) //Q
		Car.targetSteering(0.7);

	if(Keyboard.isKeyPressed(26)) //W
		Car.motor(-40.0);
	else if(Keyboard.isKeyPressed(22)) //S
		Car.motor(40.0);
	else
		Car.motor(0.0);
};

exports.explosion = function() {
	
};