#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <rfb/rfb.h>
#include <rfb/keysym.h>
#include "logging.h"
#include "usbhid_scancodes.h"
#include "usbhid_gadget.h"

int g_keyboard_gadget_fd = -1;

int usbhid_gadget_keyboard_init(void)
{
    LOGI("Trying to open USB HID Gadget Keyboard...");
    g_keyboard_gadget_fd = open("/dev/hidg0", O_RDWR);
    if (g_keyboard_gadget_fd < 0)
    {
        LOGE("Failed to open USB HID Gadget Keyboard");
        return -1;
    }
    LOGI("USB HID Gadget Keyboard opened");
    return 0;
}

void usbhid_gadget_keyboard_close(void)
{
    if (g_keyboard_gadget_fd >= 0)
    {
        LOGI("Closing USB HID Gadget Keyboard...");
        close(g_keyboard_gadget_fd);
        g_keyboard_gadget_fd = -1;
        LOGI("USB HID Gadget Keyboard closed");
    }
}

int usbhid_gadget_write_keyboard(const char *report)
{
    unsigned int retry_count = HID_REPORT_RETRY_MAX;
    ssize_t ret;

    while(retry_count > 0)
    {
        ret = write(g_keyboard_gadget_fd, report, KEY_REPORT_LENGTH);
        if (ret == KEY_REPORT_LENGTH)
        {
            LOGV("USB HID Gadget Keyboard write success");
            return 0;
        }

        if(errno == EAGAIN || errno == ESHUTDOWN)
        {
            retry_count--;
            LOGV("USB HID Gadget Keyboard write needs retry, left retry count: %d", retry_count);
            continue;
        }

        // other errors, just break and no need for retry
        break;
    }
    
    LOGE("USB HID Gadget Keyboard write failed, errno: %d, %s", errno, strerror(errno));
    return -EIO;
}

unsigned char rfb_key_to_gadget_scancode(rfbKeySym key)
{
    unsigned char scancode = 0;

    if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))
    {
        scancode = USBHID_KEY_A + ((key & 0x5F) - 'A');
    }
    else if (key >= '1' && key <= '9')
    {
        scancode = USBHID_KEY_1 + (key - '1');
    }
    else if (key >= XK_F1 && key <= XK_F12)
    {
        scancode = USBHID_KEY_F1 + (key - XK_F1);
    }
    else if (key >= XK_KP_F1 && key <= XK_KP_F4)
    {
        scancode = USBHID_KEY_F1 + (key - XK_KP_F1);
    }
    else if (key >= XK_KP_1 && key <= XK_KP_9)
    {
        scancode = USBHID_KEY_KP_1 + (key - XK_KP_1);
    }
    else
    {
        switch (key)
        {
            case XK_exclam:
                scancode = USBHID_KEY_1;
                break;
            case XK_at:
                scancode = USBHID_KEY_2;
                break;
            case XK_numbersign:
                scancode = USBHID_KEY_3;
                break;
            case XK_dollar:
                scancode = USBHID_KEY_4;
                break;
            case XK_percent:
                scancode = USBHID_KEY_5;
                break;
            case XK_asciicircum:
                scancode = USBHID_KEY_6;
                break;
            case XK_ampersand:
                scancode = USBHID_KEY_7;
                break;
            case XK_asterisk:
                scancode = USBHID_KEY_8;
                break;
            case XK_parenleft:
                scancode = USBHID_KEY_9;
                break;
            case XK_0:
            case XK_parenright:
                scancode = USBHID_KEY_0;
                break;
            case XK_Return:
                scancode = USBHID_KEY_RETURN;
                break;
            case XK_Escape:
                scancode = USBHID_KEY_ESC;
                break;
            case XK_BackSpace:
                scancode = USBHID_KEY_BACKSPACE;
                break;
            case XK_Tab:
            case XK_KP_Tab:
                scancode = USBHID_KEY_TAB;
                break;
            case XK_space:
            case XK_KP_Space:
                scancode = USBHID_KEY_SPACE;
                break;
            case XK_minus:
            case XK_underscore:
                scancode = USBHID_KEY_MINUS;
                break;
            case XK_plus:
            case XK_equal:
                scancode = USBHID_KEY_EQUAL;
                break;
            case XK_bracketleft:
            case XK_braceleft:
                scancode = USBHID_KEY_LEFTBRACE;
                break;
            case XK_bracketright:
            case XK_braceright:
                scancode = USBHID_KEY_RIGHTBRACE;
                break;
            case XK_backslash:
            case XK_bar:
                scancode = USBHID_KEY_BACKSLASH;
                break;
            case XK_colon:
            case XK_semicolon:
                scancode = USBHID_KEY_SEMICOLON;
                break;
            case XK_quotedbl:
            case XK_apostrophe:
                scancode = USBHID_KEY_APOSTROPHE;
                break;
            case XK_grave:
            case XK_asciitilde:
                scancode = USBHID_KEY_GRAVE;
                break;
            case XK_comma:
            case XK_less:
                scancode = USBHID_KEY_COMMA;
                break;
            case XK_period:
            case XK_greater:
                scancode = USBHID_KEY_DOT;
                break;
            case XK_slash:
            case XK_question:
                scancode = USBHID_KEY_SLASH;
                break;
            case XK_Caps_Lock:
                scancode = USBHID_KEY_CAPSLOCK;
                break;
            case XK_Print:
                scancode = USBHID_KEY_PRINT;
                break;
            case XK_Scroll_Lock:
                scancode = USBHID_KEY_SCROLLLOCK;
                break;
            case XK_Pause:
                scancode = USBHID_KEY_PAUSE;
                break;
            case XK_Insert:
            case XK_KP_Insert:
                scancode = USBHID_KEY_INSERT;
                break;
            case XK_Home:
            case XK_KP_Home:
                scancode = USBHID_KEY_HOME;
                break;
            case XK_Page_Up:
            case XK_KP_Page_Up:
                scancode = USBHID_KEY_PAGEUP;
                break;
            case XK_Delete:
            case XK_KP_Delete:
                scancode = USBHID_KEY_DELETE;
                break;
            case XK_End:
            case XK_KP_End:
                scancode = USBHID_KEY_END;
                break;
            case XK_Page_Down:
            case XK_KP_Page_Down:
                scancode = USBHID_KEY_PAGEDOWN;
                break;
            case XK_Right:
            case XK_KP_Right:
                scancode = USBHID_KEY_RIGHT;
                break;
            case XK_Left:
            case XK_KP_Left:
                scancode = USBHID_KEY_LEFT;
                break;
            case XK_Down:
            case XK_KP_Down:
                scancode = USBHID_KEY_DOWN;
                break;
            case XK_Up:
            case XK_KP_Up:
                scancode = USBHID_KEY_UP;
                break;
            case XK_Num_Lock:
                scancode = USBHID_KEY_NUMLOCK;
                break;
            case XK_KP_Enter:
                scancode = USBHID_KEY_KP_ENTER;
                break;
            case XK_KP_Equal:
                scancode = USBHID_KEY_KP_EQUAL;
                break;
            case XK_KP_Multiply:
                scancode = USBHID_KEY_KP_MULTIPLY;
                break;
            case XK_KP_Add:
                scancode = USBHID_KEY_KP_ADD;
                break;
            case XK_KP_Subtract:
                scancode = USBHID_KEY_KP_SUBTRACT;
                break;
            case XK_KP_Decimal:
                scancode = USBHID_KEY_KP_DECIMAL;
                break;
            case XK_KP_Divide:
                scancode = USBHID_KEY_KP_DIVIDE;
                break;
            case XK_KP_0:
                scancode = USBHID_KEY_KP_0;
                break;
        }
    }

    return scancode;
}

/*
void rfb_key_event_handler(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{

}
*/
