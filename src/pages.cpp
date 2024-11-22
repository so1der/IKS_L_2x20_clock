#include <Arduino.h>

const char* main_page = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAB2lBMVEVHcEwrKyszMzM0NDQuLi42NjYxMTEmJiY8PDwnJyctLS08PDw5OTk5OTk1NTUlJSUvLy83NzcmJiY8PDwmJiYwMDA8PDw8PDw4ODgqKioyMjIpKSkpKSksLCw7Ozs5OTkoKCgaGhozMzM6OjopKSkqKio1NTUgICAnJychISEoKCgXFxcmJiYYGBgZGRknJyc6Ojr5+fn6+vooKCgpKSk7Ozs8PDzp6enq6urt7e0qKio1NTUyMjLr6+vu7u43NzcvLy8sLCzn5+fs7OzY2Njv7+/FxcXm5uY5OTk2NjY0NDQxMTEwMDA4ODgrKyszMzMuLi4tLS3w8PDR0dH4+Pjz8/NERETX19fW1tby8vL29vba2trMzMzx8fHNzc1ZWVlubm6VlZWoqKienp7Q0NCMjIy+vr6GhobHx8e5ubmdnZ1dXV3g4ODLy8v39/eOjo7BwcHIyMjo6Oi8vLxUVFTT09OQkJB3d3fU1NTKysp6enpycnKPj4+goKCZmZmzs7OLi4vl5eWlpaVlZWVTU1Pb29tNTU3j4+O1tbXV1dV7e3tHR0erq6tmZmbd3d2vr6/GxsZGRkbf399paWn09PTe3t6AgID19fWioqKJiYlcXFzS0tLJycmBgYHL+VRNAAAAL3RSTlMA39/f39/f39/V39Xdvt9s39/dbL7f3b7f39/dvt/VbAgIH2wICB8fbB9sCAgICCCzWX0AAAHZSURBVDjLfdNldxoBEIXhCwFa4u5J3ZWtBEJCWwqhkNRYIEAgxF0br7u763/t3IVCTkv3+TDD2XeWwxcAbK+qq92aR21d1R6InUWn/6toL3C49ZSO1t3YdlzXLpToH5Sg+AJFhtVJDw3e1NakOhzRQjHqL4pIn6KsPVXE9QHOiVchpS/CUo+KM2JKHrocSla4TcYUSwXMXKqSh8pihtkvVDufzCU3d2+CxYxSp7izLD9C2fgQynX7w/sspSjg8szZ+PDz103fEPewFMAYEL02jWs5asvpZTHCeFlkDmwbqfCmAxYjGjtE9sXVX57sQZSlERauGwOh+MdwLBb70R3IXCSn77JYYOkW4+PJoOZnT2Al9iUkB+Hnt1ksqOkRI96MtdS6zGA4LjdRlhqUnxX9rrSljj+fXEFvP0s5TFwJR9r3b46cBIsJpitidubaefqUXtT5epbFhMKr4s27t51/WX9xi6UQLefE4nv7P550sbTAwLXQlscCiwGGS2Le7XY/eyTDvaJyzozImGcxoKFd+Bd9vumXPvEgxbn0+J5vws/SgCaudufo2FAXDa5qa2hs1KmFJjSf1NWM/Sd0HcQR6zEd1h3APp0L6wH+vQ+VVVduyaOyuuwo8BsKeOtaDbX/XwAAAABJRU5ErkJggg==">
    <title>Налаштування Годиннику</title>
    <style>
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
        }

      
        body {
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            background-color: #212539; 
        }

        .container {
            width: 90%;
            max-width: 400px;
            padding: 20px;
            background-color: #2f3148; 
            border-radius: 10px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
            color: #FFF;
        }

        .container h1 {
            font-size: 1.5em;
            text-align: center;
            margin-bottom: 20px;
        }

        .button-sync {
            width: 100%;
            padding: 12px;
            margin-bottom: 20px;
            font-size: 1em;
            background-color: #6a6d8a;
            color: #FFF;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .button-sync:hover {
            background-color: #575a7a;
        }

        .section-title {
            text-align: center;
            margin-bottom: 10px;
            font-weight: bold;
        }

        .form-group {
            display: flex;
            flex-direction: column;
            margin-bottom: 15px;
        }

        .form-group label {
            margin-bottom: 5px;
            font-size: 0.9em;
            color: #aaa;
        }

        .form-group input,
        .form-group select {
            padding: 10px;
            font-size: 1em;
            background-color: #3b3e57;
            color: #FFF;
            border: 1px solid #555;
            border-radius: 5px;
            outline: none;
        }

        .form-group input::placeholder {
            color: #888;
        }

        .form-group select {
            appearance: none;
            cursor: pointer;
            background-size: 16px;
        }

        .form-group select option {
            color: #000; 
        }

        .form-divider {
            margin-bottom: 20px; 
        }

        .divider {
            width: 100%;
            height: 1px;
            background-color: #444;
            margin: 20px 0;
        }

        @media (min-width: 768px) {
            .container {
                max-width: 400px;
            }
        }
    </style>
</head>

<body onload = "getData()">

    <div class="container">
        <h1>Налаштування годиннику</h1>

        <button class="button-sync", onclick="syncTime()">Синхронізувати з браузеру

        </button>

        <div class="form-group">
            <label for="temperature">Калібрування температури:</label>
            <input type="number" value=0 id="temperature" placeholder="Введіть значення">
        </div>
        <button class="button-sync" onclick="temperatureOffset()" >Калібрувати </button>

<div class="divider"></div>

        <div class="section-title">Налаштування WiFi</div>
        <div class="form-group">
            <label for="ssid">SSID:</label>
            <input type="text" id="ssid" placeholder="Введіть SSID">
        </div>
        <div class="form-group">
            <label for="password">Пароль:</label>
            <input type="password" id="password" placeholder="Введіть пароль">
        </div>
        <button class="button-sync" onclick="sendWiFi()" >Підключитись </button>
<div class="divider"></div>
        <div class="form-divider"></div>
        <div class="form-group">
            <label for="ntp-server">NTP сервер:</label>
            <input type="text" id="ntp-server" maxlength="255" placeholder="Введіть адресу NTP сервера">
        </div>
        <button class="button-sync" onclick="ntpServer()" >Зберегти NTP сервер </button>
<div class="divider"></div>
        <div class="form-group">
            <label for="timezone">Часовий Пояс:</label>
            <select id="timezone">
                <option value="7200">UTC+2</option>
                <option value="10800">UTC+3</option>
            </select>
        </div>
        <button class="button-sync", onclick="setOffset()" >Зберегти пояс </button>
    </div>

<script>
function syncTime() {
        const now = new Date(); 
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();
        const year = now.getFullYear();
        const month = now.getMonth() + 1;
        const day = now.getDate();
        const url = `/sync_time?&hours=${hours}&minutes=${minutes}&seconds=${seconds}&year=${year}&month=${month}&day=${day}`;
        fetch(url);
        alert("Налаштування відправлені");

      }

function setOffset() {
    const timezone = document.getElementById("timezone");
    const value = timezone.value;
    const text = timezone.options[e.selectedIndex].text;
    const url = `/set_timezone?&offset=${value}&name=${text}`;
    fetch(url);
    alert("Налаштування відправлені");

}

function temperatureOffset() {
	const offset = document.getElementById("temperature");
	const offset_value = offset.value;
	const url = `/temperature_offset?&offset=${offset_value}`;
	fetch(url);
    alert("Налаштування відправлені");
}

function ntpServer() {
	const e = document.getElementById("ntp-server");
	const ntp_url = e.value;
	const url = `/ntp_server?&ntp_url=${ntp_url}&len=${ntp_url.length}`;
	fetch(url);
    alert("Налаштування відправлені");
}

function sendWiFi() {
    const e = document.getElementById("ssid");
    const e2 = document.getElementById("password");
    const ssid = e.value;
    const password = e2.value;
    const url = `/set_wifi?&ssid=${ssid}&password=${password}`;
    fetch(url);
    alert("Налаштування відправлені");
}
function getData() {
    fetch('/get_data')
        .then(response => response.json())
        .then(data => {
            document.getElementById('ntp-server').value = data.NTP;
            document.getElementById('timezone').value = data.seconds_offset;
            document.getElementById('temperature').value = data.temp_offset;
            document.getElementById('ssid').value = data.ssid;
        })
        .catch(error => console.error('Error:', error));
}
</script>

</body>
</html>)rawliteral";
