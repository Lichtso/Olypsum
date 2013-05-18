/**
 Seconds elapsed since the last game tick
 @type Number
*/
var animationFactor = 0.0;

/**
 Path to the current loaded container (level).
 Can also be written to load another one
 @type String
*/
var levelID = '';

/**
 Loads a module if necessary
 
 @param {Matrix4} transformation Where the container is inserted
 @param {String} path 'Container.xml'
 @return {Array} List of the loaded objects or null on failture
*/
function loadContainer(transformation, path) {
	//[native code]
}

/**
 Prints a message on the console and std::out
 
 @param {Object} data to be printed
*/
function log(data) {
	//[native code]
}

/**
 Loads a module if necessary
 
 @param {String} path '../Package/File.js' or 'File.js'
 @return {Object} The module
*/
function require(path) {
	//[native code]
}

/**
 Saves the current loaded container (level)
 
 @param {String} localData
 @param {String} globalData
 @return {Boolean} Success
*/
function saveLevel(localData, globalData) {
	//[native code]
}