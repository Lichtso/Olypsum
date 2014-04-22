var Main = Engine.getScript('Main');

/*exports.onload = function(data) {

};

exports.onsave = function() {
	return "ObjectData";
};
*/
exports.oncollision = function(otherObject, posA, posB, dists, impulses) {
	var impulse = impulses[0];
	for(var i = 1; i < impulses.length; impulses ++)
		if(impulses[i] > impulse)
			impulse = impulses[i];
	if(impulse < 20.0 || this.integrity <= 0.0 || !Main.explosion) return;
	Main.explosion(this);
    
	/*var now = new Date();
	if(!this.spawnTimer || now.getTime() - this.spawnTimer.getTime() > 2000.0) {
		var transform = this.transformation();
		transform.translate(new Vector3(0.0, -2.0, 0.0));
		var box = loadContainer(transform, "woodenBox")[0];
		transform.setIdentity();
		transform.rotate(new Vector3(0.0, 1.0, 0.0), Math.PI/2);
		transform.rotate(new Vector3(1.0, 0.0, 0.0), Math.PI);
		transform.translate(new Vector3(5.0, 2.0, 0.0));
		this.spawnTimer = now;
	}*/
};