# Bug Reproduction: Media and Other Keys Autoscroll the Terminal

## Steps to reproduce

1. Open Windows Terminal with any shell (Ubuntu, PowerShell, CMD).
2. Run a command that produces several screens of output (e.g. run `ls` five times in a directory with many files).
3. Scroll the viewport upward so that the active prompt/input area is off-screen.
4. Press a non-character key that is not a modifier — for example: Volume Up, Volume Down, Mute, Play/Pause, Next Track, or the Calculator key.

## Observed

The terminal viewport immediately snaps back to the bottom (where text input is awaited), even though the key pressed does not produce any characters and is not intended as terminal input.

Root cause: `Terminal::IsInputKey()` (in `src/cascadia/TerminalCore/Terminal.hpp`) classifies media, browser, and launcher virtual keys (VK_BROWSER_BACK 0xA6 through VK_LAUNCH_APP2 0xB7) as "input keys". This causes `Terminal::TrySnapOnInput()` to scroll the viewport to the bottom whenever one of those keys is pressed, regardless of whether the key generates any terminal output.

## Expected

Pressing media keys (volume control, play/pause, next/previous track, calculator, browser back/forward, etc.) should have no effect on the terminal viewport scroll position. The screen should stay scrolled to wherever the user left it, because those keys do not send characters to the terminal process.
