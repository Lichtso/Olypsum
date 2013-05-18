/*exports.onload = function(data) {
	log([typeof this, this instanceof Object]);
};

exports.onsave = function() {
	return "ObjectData";
};
*/
exports.oncollision = function(otherObject) {
	var now = new Date();
	if(!this.spawnTimer || now.getTime() - this.spawnTimer.getTime() > 2000.0) {
		var transform = this.transformation();
		transform.translate(new Vector3(0.0, -2.0, 0.0));
		var box = loadContainer(transform, "woodenBox")[0];
		transform.setIdentity();
		transform.rotate(new Vector3(0.0, 1.0, 0.0), Math.PI/2);
		transform.rotate(new Vector3(1.0, 0.0, 0.0), Math.PI);
		transform.translate(new Vector3(5.0, 2.0, 0.0));

		Animation.addFrames(box, "transformation", true, [0.0, 0.0], [2.0, 2.0], [null, transform]);
		this.spawnTimer = now;
		//log('Spawning: '+now);
	}
};