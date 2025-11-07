# Mullvad VPN Manager (MVM)

> A terminal UI for quickly switching and managing Mullvad WireGuard profiles on systemd systems.

This is a terminal based program that eases the process of managing a Mullvad VPN configuration on a system using [WireGuard](https://www.wireguard.com/) and the system manager [systemd](https://github.com/systemd/systemd/).

It does not aim to be complete in that it can setup and configure Mullvad in all possible system configurations, but in easing the process of modifying an already existing Mullvad configuration on the user's system (given they use, as mentioned above, systemd and WireGuard).

It currently uses [ncurses](https://invisible-island.net/ncurses/ncurses.html) for the user interface and [sd-bus](https://github.com/systemd/systemd/) to interact with systemd.


## Prerequisites

- [systemd](https://github.com/systemd/systemd/)
- [WireGuard](https://www.wireguard.com/)


## Features

- Manage WireGuard profiles created from Mullvad’s configuration ZIPs
- Quickly start/stop connections via systemd services
- View active connection status in a TUI
- Optional basic firewall integration (iptables)
  - Profile and firewall switching planned


## Planned Features

- NetworkManager backend support
- OpenVPN profile compatibility
- Custom firewall templates and editor integration

>> **Disclaimer:** This is an independent hobby project and not affiliated with Mullvad VPN.
