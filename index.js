$(document).ready(function() {
  $( "#upload" ).click(function() {
    var reader = new FileReader();
    var list = document.getElementById("fileName");
    reader.onloadend = function () {
    
      post('./index.php', {data: reader.result});
    };
    reader.readAsText(list.files[0]);
    
    
    
  });
  
  $( "#run" ).click(function() {
    post('./index.php', {run:"1"});
  });
});

// function to submit post request
function post(path, params, method) {
    method = method || "post"; // Set method to post by default if not specified.

    // The rest of this code assumes you are not using a library.
    // It can be made less wordy if you use one.
    var form = document.createElement("form");
    form.setAttribute("method", method);
    form.setAttribute("action", path);

    for(var key in params) {
        if(params.hasOwnProperty(key)) {
            var hiddenField = document.createElement("input");
            hiddenField.setAttribute("type", "hidden");
            hiddenField.setAttribute("name", key);
            hiddenField.setAttribute("value", params[key]);

            form.appendChild(hiddenField);
         }
    }

    document.body.appendChild(form);
    form.submit();
}