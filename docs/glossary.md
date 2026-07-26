# Glennergy glossary

| Metadata | Value |
| --- | --- |
| Status | Current, with unresolved terms marked explicitly |
| Canonical owner | Glennergy-ESP |
| Audience | Readers and contributors across both repositories |
| Last verified | Glennergy-ESP `b5a502a`; Glennergy `42798be` |

This glossary defines the preferred English terms used by Glennergy and
Glennergy-ESP documentation. Exact source-code identifiers retain their original
spelling even when the prose term differs.

| Preferred term | Meaning | Exact identifiers or legacy terms | Status |
| --- | --- | --- | --- |
| Glennergy | The Linux server stack that gathers weather and electricity-price data, performs calculations, and exposes LEOP data to the ESP firmware | Repository `glennergy` | Implemented |
| Glennergy-ESP | Firmware for the ESP32-S3 display/controller that reads data from Glennergy and presents it locally | Repository `Glennergy-ESP` | Implemented |
| LEOP server | The Glennergy service as seen by Glennergy-ESP | `LEOP`, `LEOPFetcher_*` | Implemented project term; the expansion of “LEOP” remains unresolved |
| LEOP client | The firmware modules that fetch, parse, cache and publish Glennergy data | `main/LEOP/LEOP_Fetcher.*` and category modules | Implemented |
| Property | One configured installation/building for which Glennergy obtains inputs and calculates results | `Homesystem_t`; Swedish `fastighet`; configuration filenames containing `Fastigheter` | Preferred English prose term |
| Property ID | The integer currently used in HTTP paths to select a property result | `id`; current ESP value `2` | Temporary, unauthenticated implementation |
| Device identity | A planned identifier unique to an individual ESP32-S3 unit | UUID-like direction; no final field or protocol | Planned and not designed |
| InputCache | The Glennergy process that owns current weather and spot-price input data and serves local IPC requests | `Glennergy-InputCache`, `inputcache_*` | Implemented |
| Meteo | The Glennergy weather-data producer | `Glennergy-Meteo`; production code under `API/Meteocpp` | Implemented; preserve executable/source names |
| Spotpris | The Glennergy electricity spot-price producer | `Glennergy-Spotpris`; Swedish for spot price | Implemented; preserve executable/source names |
| Algorithm | The Glennergy calculation process | `Glennergy-Algoritm`, `AlgoritmShared`; Swedish code spelling `Algoritm` | Use “Algorithm” in prose and exact spelling for identifiers |
| Recommendation dataset | The normal 96-object API response for one matching positive property ID requested with the current `recommendation` command | JSON fields `id`, `type`, `timestamp`, `temp` | Implemented; intended meaning of `type` remains unresolved; ID `0` is a known unsafe exception |
| Recommendation type | The `type` number in each recommendation object | `recommendation[].type` | Unknown intent; current algorithm discards a categorical result and publishes `average_WindowLow_percent` instead |
| Weather dataset | The 96-object API response containing forecast temperature, weather code and UV index | JSON fields `timestamp`, `temp`, `weather_code`, `uv_index` | Implemented |
| Price dataset | The 96-object API response containing electricity prices | JSON fields `timestamp`, `price SEK` | Implemented; the space in `price SEK` is part of the current contract |
| Electricity area | One of Sweden’s electricity price zones | `SE1`, `SE2`, `SE3`, `SE4`; property field `electricity_area` | Implemented |
| Quarter-hour sample | A value representing one 15-minute interval | Current API result arrays contain 96 entries | Implemented |
| Live data | A response obtained from Glennergy during current connectivity | Category GET responses | Implemented |
| Cached data | The latest successfully stored JSON category response used by the ESP while offline | SPIFFS files `Recommendations.json`, `Weather.json`, `Price.json` | Implemented |
| Latest-value queue | A depth-one FreeRTOS queue whose previous value is overwritten by a newer snapshot | `xQueueOverwrite` flows | Implemented; not a durable event history |
| Connected | All three LEOP category fetches succeeded, or the current health probe succeeded | `LEOP_CONNECTION_CONNECTED` | Implemented status |
| Degraded | Only some LEOP category fetches succeeded | `LEOP_CONNECTION_DEGRADED` | Implemented status |
| Unavailable | Repeated server health/fetch failures reached the current threshold | `LEOP_CONNECTION_UNAVAILABLE` | Implemented status |
| Stable production | Behavior represented by the stable remote `main` branch | `origin/main` snapshots in the campaign manifest | Owner-defined lifecycle term |
| Authoritative implementation | Current development behavior represented by `dev` | `dev` snapshots in the campaign manifest | Owner-defined lifecycle term |

## Units and representations

| Value | Current representation | Notes |
| --- | --- | --- |
| Temperature | Degrees Celsius | Used in weather/sensor contexts; recommendation semantics remain separate |
| Pressure | Hectopascals in the ESP UI/data path | Verify at each public field because lower-level sensor APIs may use different units |
| Humidity | Percent relative humidity | ESP sensor data |
| Price | SEK/kWh in algorithm/project intent | Current JSON key is exactly `price SEK`; consumers must preserve it |
| UV index | Glennergy serializes a JSON real after upstream integer conversion; ESP reads with `json_integer_value` | The current ESP consumer can store zero for the real-valued wire member |
| Timestamp | Server timestamp string can include an offset; ESP retains at most 19 characters | Longer values lose the offset portion |

## Update triggers

Update this glossary when a public schema, unit, route term, process/module name,
property identity model or preferred English term changes. New documentation
must link to this glossary instead of redefining cross-project terms differently.
