function parse_csv(csv_data) {
  var rows = csv_data.split('\n');

  var parsed_data = [];
  for (var i = 0; i < rows.length; i++) {
    if (rows[i] == '') {
      continue;
    }
    var values = rows[i].split(',');

    var key = values[0].trim();
    var value = values[1].trim();

    parsed_data.push([key, value]);
  }

  return parsed_data;
}

// Function to update the value
function update_val() {
  // Create a new XMLHttpRequest object
  var xhr = new XMLHttpRequest();

  // Configure the request
  xhr.open('GET', '/get/values', true);

  // Define the callback function
  xhr.onreadystatechange = function() {
    if (xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
      var csvData = xhr.responseText;
      var parsedData = parse_csv(csvData);

      var tableHTML = '<table>';
      tableHTML += '<tr>';
      tableHTML += '<th>Key</th>'; // Key
      tableHTML += '<th>Value</th>'; // Value
      tableHTML += '</tr>';
      for (var i = 0; i < parsedData.length; i++) {
        tableHTML += '<tr>';
        tableHTML += '<td>' + parsedData[i][0] + '</td>'; // Key
        tableHTML += '<td>' + parsedData[i][1] + '</td>'; // Value
        tableHTML += '</tr>';
      }
      tableHTML += '</table>';

      document.getElementById('data').innerHTML = tableHTML;
    }
  };

  // Send the request
  xhr.send();
}

// 500ms
setInterval(update_val, 500);
