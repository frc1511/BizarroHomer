function updateValue() {
  var randomNumber = Math.random();
  document.getElementById('data').textContent = randomNumber;
}

// Update every 500ms (0.5s).
setInterval(updateValue, 500);
