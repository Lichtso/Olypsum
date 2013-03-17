exports.onload = function() {
	log('Hello, World!');
	log(this);
	return true;
};

exports.oncollision = function(b) {
	log('oncollision: '+b);
};