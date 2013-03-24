exports.onload = function(localData, globalData) {
	log([levelID, localData, globalData]);
};

exports.onleave = function() {
	saveLevel("LocalData", null);
};