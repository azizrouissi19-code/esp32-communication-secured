# esp32-communication-secured

An ESP32 web-server demo showing **secured** HTTP command handling: API-key / Bearer-token
authorization and a strict command whitelist, contrasted with cleartext exposure.

## Security model
- Every control request must carry `Authorization: Bearer <API_KEY>` (or `?api_key=`).
- Only whitelisted commands are accepted: `forward, backward, left, right, stop, led_on, led_off`.

## Setup
Credentials are **not** committed. Copy the template and fill in your own:
```
cp secrets.h.example secrets.h
```
Then edit `secrets.h` with your Wi-Fi SSID/password, device token, and API key.
`secrets.h` is gitignored — never commit it.

## Build
Arduino IDE / PlatformIO with the ESP32 board package.
