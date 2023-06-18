function get_log(name, element) {
  var xhr = new XMLHttpRequest();
  
  xhr.open('GET', name, true);
  
  xhr.onreadystatechange = function() {
    if (xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
      var inner_html = '<h3>' + name + '</h3>\n';
      inner_html += '<pre class=output_box>' + xhr.responseText + '</pre>';
      document.getElementById(element).innerHTML = inner_html;
    }
  };
  
  xhr.send();
}

function update_outputs() {
  get_log('/' + which_program + '_output.log', 'stdout');
  get_log('/' + which_program + '_error.log', 'stderr');
}

// 500ms
setInterval(update_outputs, 500);
