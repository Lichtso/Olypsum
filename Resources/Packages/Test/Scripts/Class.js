exports.onload = function(data) {
	log(data);
};

/*
exports.onsave = function() {
	return "ObjectData";
};
*/
exports.oncollision = function(otherObject) {
	var now = new Date();
	if(!this.spawnTimer || now.getTime() - this.spawnTimer.getTime() > 2000.0) {
		var transform = this.transformation();
		transform.w(transform.w().add(new Vector3(0.0, -2.0, 0.0)));
		loadContainer(transform, "woodenBox");
		this.spawnTimer = now;
		log('Spawning: '+now);
	}
};