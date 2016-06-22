var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
var address = '';

function sendDict() {
      console.log(address);
         console.log("sending addr");
	address=address+'\n';
         dictionary = {
          "0" : address
          };
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
                            );
}

function addrSuccess(pos) {
  // Construct URL
  var url = "http://54.149.47.51:8000/test?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude;

  console.log("addr Success");
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      address = responseText;
      console.log(address);
      sendDict();
    }      
  );
}

function addrError(err) {
  console.log("Error requesting address!");
}

function getAddr() {
  navigator.geolocation.getCurrentPosition(
    addrSuccess,
    addrError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    getAddr();
  }                     
);

