# Presentation asset status

These SVG and PNG files are historical presentation snapshots. They are not
canonical architecture or hardware references and should not be reused as
current diagrams without revalidation against `dev`.

| Asset | Current disposition |
| --- | --- |
| `block_diagram.*` | Historical high-level overview; includes the former 7-inch display assumption |
| `pin_configuration.*` | Historical 7B/7-inch board diagram; does not match the repository-declared 4.3-inch target |
| `leop_fetch_sequence.*` and `leop_fetch_sequence_v2.svg` | Predates parts of the current health-probe, reconnect-deadline, cache-validation, and publication behavior |
| `leop_state_diagram.*` | Predates the canonical connection-state and health-retry model |
| `queue_interfaces.*` | Partial queue snapshot; omits newer state/event details and direct shared-state access |
| `runtime_signal_flow.*` | Historical overview that simplifies queue ownership and synchronization boundaries |

Use the maintained Mermaid diagrams in these documents instead:

- [Firmware architecture](../architecture.md)
- [Connectivity and data flow](../connectivity.md)
- [ESP interface contract](../interface-contract.md)
- [UI guide](../ui-guide.md)
- [Hardware guide](../hardware.md)

The five photographs under `docs/images/user-guide/` are separate observed UI
snapshots and remain the images used by the visual user guide.
