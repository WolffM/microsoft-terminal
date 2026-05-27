## Steps to reproduce
1. On Windows Terminal 1.17.11461.0, open a normal Terminal window.
2. Keep at least one additional app window open so Alt+Tab has multiple candidates.
3. Press `Alt+Tab` once to switch from Terminal to another app.
4. Immediately press `Alt+Tab` again expecting to return to Terminal.
5. For code-level tracing, inspect the show/hide path in `AppHost::_ShowWindowChanged` and `IslandWindow::ShowWindowChanged` while stepping through foreground/minimize transitions.

## Observed
Instead of returning directly to Terminal on the second Alt+Tab, Windows selects a different window and Terminal is effectively pushed back in the switching order. The traced path shows window show/hide requests are funneled through a trailing-only 200ms throttler, so restore requests can be delayed enough to miss the immediate Alt+Tab transition timing. This reproduces a user-visible ordering glitch where Terminal appears later than expected in the Alt+Tab stack.

## Expected
After switching away from Terminal with Alt+Tab, the next Alt+Tab should bring focus back to Terminal as the most recent prior app window. Restore/show behavior should be applied promptly enough to preserve normal Alt+Tab MRU expectations, while still preventing rapid PTY/host oscillation loops. In short: keep anti-loop throttling, but do not defer the first restore decision in a way that reorders interactive task switching.
