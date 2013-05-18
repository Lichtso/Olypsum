/**
 3D vector
 @class
 @constructor
 @param {Array|Object|Number[]} coords x, y, z
 @property {Number} 0|x X-Coord
 @property {Number} 1|y Y-Coord
 @property {Number} 2|z Z-Coord
*/
function Vector3() {
	/**
	 Calculates the angle between this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @return {Number} radians
	*/
	this.getAngle = function(vec) {
		//[native code] does something like:
		return Math.acos(this.getDot(vec));
	};
	/**
	 Calculates the sum of this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.getSum = function(vec) {
		//[native code] does something like:
		return new Vector3(this.x+vec.x, this.y+vec.y, this.z+vec.z);
	};
	/**
	 Calculates the difference between this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.getDiff = function(vec) {
		//[native code] does something like:
		return new Vector3(this.x-vec.x, this.y-vec.y, this.z-vec.z);
	};
	/**
	 Calculates the product of this and another Vector3 or a Number.
	 This method does not modify the vectors.
	 @param {Vector3|Number} vec
	 @return {Vector3}
	*/
	this.getProduct = function(vec) {
		//[native code]
	};
	/**
	 Calculates the quotient of this and another Vector3 or a Number.
	 This method does not modify the vectors.
	 @param {Vector3|Number} vec
	 @return {Vector3}
	*/
	this.getQuotient = function(vec) {
		//[native code]
	};
	/**
	 Calculates the dot product of this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @return {Number}
	*/
	this.getDot = function(vec) {
		//[native code] does something like:
		return this.x*vec.x + this.y*vec.y + this.z*vec.z;
	};
	/**
	 Calculates the cross product of this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.getCross = function(vec) {
		//[native code]
	};
	/**
	 Calculates the length of this Vector3.
	 This method does not modify the vector.
	 @return {Number}
	*/
	this.getLength = function() {
		//[native code] does something like:
		return Math.sqrt(this.getDot(this));
	};
	/**
	 Calculates the direction of this Vector3.
	 This method does not modify the vector.
	 @return {Vector3}
	*/
	this.getNormalized = function() {
		//[native code]
	};
	/**
	 Calculates the linear interpolation between this and another Vector3.
	 This method does not modify the vectors.
	 @param {Vector3} vec
	 @param {Number} t
	 @return {Vector3}
	*/
	this.getInterpolation = function(vec, t) {
		//[native code]
	};
	/**
	 Adds another to this Vector3 and stores the result in this Vector3.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.add = function(vec) {
		//[native code]
	};
	/**
	 Subtracts another from this Vector3 and stores the result in this Vector3.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.sub = function(vec) {
		//[native code]
	};
	/**
	 Multiplies this Vector3 with another or a Number and stores the result in this Vector3.
	 @param {Vector3|Number} vec
	 @return {Vector3}
	*/
	this.mult = function(vec) {
		//[native code]
	};
	/**
	 Divides this Vector3 by another or a Number and stores the result in this Vector3.
	 @param {Vector3|Number} vec
	 @return {Vector3}
	*/
	this.divide = function(vec) {
		//[native code]
	};
	/**
	 Calculates the cross product of this and another Vector3 and stores the result in this Vector3.
	 @param {Vector3} vec
	 @return {Vector3}
	*/
	this.cross = function(vec) {
		//[native code]
	};
	/**
	 Normalizes this Vector3.
	 @return {Vector3}
	*/
	this.normalize = function() {
		//[native code]
	};
	/**
	 Convertes Vector3 to a Array
	 @return {Array} [x, y, z]
	*/
	this.toJSON = function() {
		//[native code] does something like:
		return [this.x, this.y, this.z];
	};
	/**
	 Convertes Vector3 to a String
	 @return {String} 'x y z'
	*/
	this.toString = function() {
		//[native code] does something like:
		return this.x+' '+this.y+' '+this.z;
	};
}