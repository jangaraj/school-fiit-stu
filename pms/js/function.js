function waitingBox() {
	var elem = document.getElementById('waitbox');
	
		clickfirst = false;
		document.body.style.cursor = 'wait';
		var ie6 = (document.all && !window.opera && !window.XMLHttpRequest) ? true : false;
		elem.style.zIndex='999';
		var height = 0;
		var width = 0;
		if (ie6) {
			var elements = document.body.getElementsByTagName('select');
			for (var i = 0; i < elements.length; i++) {
				elements[i].disabled='disabled';
			}
		} 
		if (!window.opera) {
			height = document.body.clientHeight;
		} else {
			height = document.documentElement.clientHeight;
		}
		elem.style.margin='0';
		elem.style.height=height+'px';
		elem.style.visibility='visible';
	
} 
					
function waitingBoxDown() {
  var elem = document.getElementById('waitbox');  
  document.body.style.cursor = 'default';
  elem.style.zIndex='-999';
  elem.style.visibility='hidden';  
}					