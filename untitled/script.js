function fetchTemperatureHumidity() {
    fetch('http://localhost:8080')
        .then(response => response.json())
        .then(data => {
            document.getElementById('temperature').textContent = data.temperature.toFixed(2);
            document.getElementById('humidity').textContent = data.humidity.toFixed(2);
        })
        .catch(error => console.error('Error:', error));
}

fetchTemperatureHumidity(); // 初始加载
setInterval(fetchTemperatureHumidity, 10000); // 每10秒刷新
