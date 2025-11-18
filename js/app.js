// --- DOM Elemente ---
const connectButton = document.getElementById('connectButton');
const btStatus = document.getElementById('btStatus');
const outputText = document.getElementById('outputText');
const visualIndicator = document.getElementById('visualIndicator');

// --- Web Bluetooth Code ---
let arduinoDevice;
let arduinoServer;
let arduinoUartService;
let arduinoUartCharacteristic;

const ARDUINO_UART_SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const ARDUINO_UART_WRITE_CHARACTERISTIC_CANDIDATES = [
    '6e400002-b5a3-f393-e0a9-e50e24dcca9e', // Standard-Nordic RX (Central → Peripheral)
    '6e400003-b5a3-f393-e0a9-e50e24dcca9e'  // Fallback für Boards, die auf TX schreiben lassen
];
const ARDUINO_NAME_PREFIXES = ['Arduino UNO R4', 'UNO R4', 'Arduino UNO'];

function buildArduinoFilters() {
    const filters = ARDUINO_NAME_PREFIXES.map((prefix) => ({
        namePrefix: prefix,
        services: [ARDUINO_UART_SERVICE_UUID]
    }));
    filters.push({ services: [ARDUINO_UART_SERVICE_UUID] });
    return filters;
}

async function connectArduino() {
    try {
        arduinoDevice = await navigator.bluetooth.requestDevice({
            filters: buildArduinoFilters(),
            optionalServices: [ARDUINO_UART_SERVICE_UUID]
        });

        arduinoServer = await arduinoDevice.gatt.connect();
        arduinoUartService = await arduinoServer.getPrimaryService(ARDUINO_UART_SERVICE_UUID);
        arduinoUartCharacteristic = await findArduinoCharacteristic(arduinoUartService);

        console.log('✅ Arduino UNO R4 verbunden');
        alert('Arduino UNO R4 erfolgreich verbunden!');
        setBluetoothConnected(true);

        arduinoDevice.addEventListener('gattserverdisconnected', handleArduinoDisconnect);
    } catch (error) {
        console.error('❌ Fehler beim Verbinden mit dem Arduino UNO R4:', error);
        alert('Arduino UNO R4 konnte nicht verbunden werden. Bitte prüfe den Bluetooth-Status und das laufende Sketch.');
        cleanupArduinoState();
        setBluetoothConnected(false);
    }
}

function handleArduinoDisconnect() {
    console.warn('⚠️ Arduino UNO R4 Verbindung getrennt.');
    alert('Arduino UNO R4 Verbindung getrennt.');
    cleanupArduinoState();
    setBluetoothConnected(false);
}

function cleanupArduinoState() {
    arduinoUartCharacteristic = null;
    arduinoUartService = null;
    if (arduinoServer && arduinoServer.connected) {
        arduinoServer.disconnect();
    }
    arduinoServer = null;
    arduinoDevice = null;
}

async function findArduinoCharacteristic(service) {
    for (const uuid of ARDUINO_UART_WRITE_CHARACTERISTIC_CANDIDATES) {
        try {
            return await service.getCharacteristic(uuid);
        } catch (_) {
            // continue searching
        }
    }
    throw new Error('Kein kompatibles UART-Charakteristikum im UNO R4 Service gefunden.');
}

async function sendToArduino(text) {
    if (!arduinoUartCharacteristic) {
        console.warn('Arduino UNO R4 ist nicht verbunden – Nachricht wurde nicht gesendet.');
        return;
    }

    try {
        const data = new TextEncoder().encode(String(text) + '\n');
        await arduinoUartCharacteristic.writeValueWithoutResponse(data);
        console.log(`Gesendet: ${text}`);
    } catch (error) {
        console.error('Senden zum Arduino UNO R4 fehlgeschlagen:', error);
    }
}

function setBluetoothConnected(isConnected) {
    if (isConnected) {
        btStatus.textContent = 'Status: Verbunden';
        btStatus.style.color = '#28a745';
        connectButton.textContent = 'Trennen';
        connectButton.onclick = handleArduinoDisconnect;
    } else {
        btStatus.textContent = 'Status: Nicht verbunden';
        btStatus.style.color = '#dc3545';
        connectButton.textContent = 'Mit Arduino verbinden';
        connectButton.onclick = connectArduino;
    }
}

// Initialer Setup für den Bluetooth-Button
if ('bluetooth' in navigator) {
    connectButton.onclick = connectArduino;
} else {
    connectButton.disabled = true;
    btStatus.textContent = 'Web Bluetooth wird von diesem Browser nicht unterstützt.';
    alert('Web Bluetooth wird von diesem Browser nicht unterstützt. Bitte nutze Chrome auf Desktop oder Android.');
}

// --- Web Speech API Code ---
const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
const validNumbers = {
    'null': '#6c757d', 'eins': '#dc3545', 'zwei': '#fd7e14', 'drei': '#ffc107',
    'vier': '#28a745', 'fünf': '#20c997', 'sechs': '#17a2b8', 'sieben': '#007bff',
    'acht': '#6610f2', 'neun': '#e83e8c'
};
// Map für die Umwandlung von Wort zu Ziffer
const numberMap = {
    'null': '0', 'eins': '1', 'zwei': '2', 'drei': '3', 'vier': '4',
    'fünf': '5', 'sechs': '6', 'sieben': '7', 'acht': '8', 'neun': '9'
};

if (SpeechRecognition) {
    const recognition = new SpeechRecognition();
    recognition.lang = 'de-DE';
    recognition.continuous = true;
    recognition.interimResults = false;

    recognition.onresult = (event) => {
        for (let i = event.resultIndex; i < event.results.length; ++i) {
            if (event.results[i].isFinal) {
                const speechResult = event.results[i][0].transcript.toLowerCase().trim().replace('.', '');
                console.log('Vollständiges Ergebnis:', speechResult);

                // Teile das Ergebnis in einzelne Wörter auf und suche nach einem gültigen Zahlwort
                const words = speechResult.split(' ');
                const foundNumberWord = words.find(word => validNumbers.hasOwnProperty(word));

                if (foundNumberWord) {
                    console.log('Gültiges Zahlwort gefunden:', foundNumberWord);
                    outputText.textContent = foundNumberWord;
                    visualIndicator.style.backgroundColor = validNumbers[foundNumberWord];
                    visualIndicator.style.transform = 'scale(1.2)';

                    // Wandle das gefundene Wort in die entsprechende Ziffer um
                    const numberToSend = numberMap[foundNumberWord];

                    // Sende die Ziffer an den Arduino
                    sendToArduino(numberToSend);
                }
            }
        }
    };

    recognition.onerror = (event) => {
        console.error('Fehler bei der Spracherkennung:', event.error);
    };
    
    recognition.onend = () => {
        console.log('Spracherkennung beendet, starte neu.');
        recognition.start();
    };
    
    recognition.start();

} else {
    outputText.textContent = 'Browser nicht unterstützt';
    alert('Dein Browser unterstützt die Web Speech API nicht.');
}
