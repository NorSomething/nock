# nock

A minimal X11 screen locker built with XCB and PAM.

---





---

## Features

- Fullscreen lockscreen window covering 1920x1080
- PAM authentication against the current user account
- Keyboard grab to prevent input routing to other windows
- Toggle password visibility on/off with a mouse click
- Supports letters, numbers, symbols, spacebar, and backspace
- Escape key exits (useful during development/testing)

---

## Installation

**Dependencies**

- `libxcb`
- `libxcb-keysyms`
- `libxcb-util`
- `libpam`
- `libpam-misc`

On Arch-based systems:

```
sudo pacman -S libxcb xcb-util-keysyms pam
```

On Debian/Ubuntu-based systems:

```
sudo apt install libxcb1-dev libxcb-keysyms1-dev libxcb-util-dev libpam0g-dev
```

**PAM config**

Run the install script once as root to set up the PAM service file:

```
sudo ./install.sh
```

This creates `/etc/pam.d/nock` which delegates auth to `system-local-login`.

**Build**

```
make
```

---

## Usage

Run the binary directly:

```
./nock
```

nock will grab the keyboard and display a fullscreen white window. Type your password and press Enter. Click anywhere to toggle whether the password is shown or hidden.

To unlock: type your user password and press Enter.  
To exit during development: press Escape.

---

## Todo

- [ ] Proper fullscreen support
- [ ] Error handling for font/graphics context setup
- [ ] Error handling for keyboard grab
- [ ] Configurable resolution (currently hardcoded to 1920x1080)
- [ ] PAM error message display in the window
- [ ] Support for more special characters and key combinations
- [ ] Mouse pointer grab alongside keyboard grab
- [ ] Proper session unlock integration (e.g. hook into a display manager or systemd-logind)

