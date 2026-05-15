// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include <WexTestClass.h>

#include "../cascadia/TerminalCore/Terminal.hpp"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

using namespace Microsoft::Terminal::Core;

constexpr Microsoft::Console::VirtualTerminal::TerminalInput::OutputType unhandled()
{
    return {};
}

constexpr Microsoft::Console::VirtualTerminal::TerminalInput::OutputType escChar(const wchar_t wch)
{
    const wchar_t buffer[2]{ L'\x1b', wch };
    return { { &buffer[0], 2 } };
}

namespace TerminalCoreUnitTests
{
    class InputTest
    {
        TEST_CLASS(InputTest);

        TEST_METHOD(AltShiftKey);
        TEST_METHOD(InvalidKeyEvent);
        TEST_METHOD(MediaKeysAreNotInputKeys);

        Terminal term{ Terminal::TestDummyMarker{} };
    };

    void InputTest::AltShiftKey()
    {
        // Tests GH:637

        // Verify that Alt+a generates a lowercase 'a' on the input
        VERIFY_ARE_EQUAL(escChar(L'a'), term.SendCharEvent(L'a', 0, ControlKeyStates::LeftAltPressed));

        // Verify that Alt+shift+a generates an uppercase 'a' on the input
        VERIFY_ARE_EQUAL(escChar(L'A'), term.SendCharEvent(L'A', 0, ControlKeyStates::LeftAltPressed | ControlKeyStates::ShiftPressed));
    }

    void InputTest::InvalidKeyEvent()
    {
        // Certain applications like AutoHotKey and its keyboard remapping feature,
        // send us key events using SendInput() whose values are outside of the valid range.
        VERIFY_ARE_EQUAL(unhandled(), term.SendKeyEvent(0, 123, {}, true));
        VERIFY_ARE_EQUAL(unhandled(), term.SendKeyEvent(255, 123, {}, true));
    }

    void InputTest::MediaKeysAreNotInputKeys()
    {
        // Media, browser, and launcher keys (VK_BROWSER_BACK through
        // VK_LAUNCH_APP2) must NOT be treated as input keys. If they were, pressing
        // them would snap the terminal viewport to the bottom even though they don't
        // produce any terminal output.
        //
        // Regular keys (e.g. VK_RETURN) must still be considered input keys.
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_BACK));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_FORWARD));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_REFRESH));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_STOP));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_SEARCH));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_FAVORITES));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_BROWSER_HOME));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_VOLUME_MUTE));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_VOLUME_DOWN));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_VOLUME_UP));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_MEDIA_NEXT_TRACK));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_MEDIA_PREV_TRACK));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_MEDIA_STOP));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_MEDIA_PLAY_PAUSE));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_LAUNCH_MAIL));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_LAUNCH_MEDIA_SELECT));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_LAUNCH_APP1));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_LAUNCH_APP2));

        // Modifier keys are also not input keys (existing behavior).
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_CONTROL));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_SHIFT));
        VERIFY_IS_FALSE(Terminal::IsInputKey(VK_MENU));

        // Regular keys are input keys.
        VERIFY_IS_TRUE(Terminal::IsInputKey(VK_RETURN));
        VERIFY_IS_TRUE(Terminal::IsInputKey(VK_BACK));
        VERIFY_IS_TRUE(Terminal::IsInputKey(VK_SPACE));
        VERIFY_IS_TRUE(Terminal::IsInputKey('A'));
    }
}
