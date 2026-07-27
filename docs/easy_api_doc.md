# LEOP Server API (historical overview)

> **Noncanonical historical draft:** This Swedish overview is retained for
> reference but may contain stale behavior or encoding. Use the current
> [ESP interface contract](interface-contract.md) and Glennergy
> [server API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md)
> for implementation work.

ESP32-terminalen hämtar data från LEOP-servern via tre HTTP GET-endpoints.

| Endpoint | Syfte | Används av |
| --- | --- | --- |
| `GET /id=2?recommendation` | Hämtar rekommendationer för elförbrukning | Electricity view |
| `GET /id=2?weather` | Hämtar väderprognos | Weather dashboard |
| `GET /id=2?price` | Hämtar elpriser | Price view |

Alla endpoints returnerar JSON-arrayer med tidsstämplade värden. ESP32-klienten parsar svaret, sparar giltig data i SPIFFS-cache och skickar datan vidare till UI:t via FreeRTOS-köer.

### Response fields

| Endpoint | Viktiga fält |
| --- | --- |
| `recommendation` | `id`, `type`, `timestamp` |
| `weather` | `temp`, `uv_index`, `weather_code`, `timestamp` |
| `price` | `price SEK`, `timestamp` |

### Error handling

- Om WiFi är anslutet hämtar ESP32 live-data från servern.
- Giltig serverdata sparas lokalt som JSON-cache.
- Om WiFi saknas används senaste cachade JSON-data från SPIFFS.
- Om JSON är tom eller ogiltig avvisas den av parsern och fetch-status markeras som misslyckad.

Den historiska fullversionen finns i [`API_ENDPOINTS.md`](API_ENDPOINTS.md).
Använd de kanoniska dokumenten ovan för aktuellt beteende.
