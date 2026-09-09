/*
 * main.c - Fingerprint Security System (simple version)
 * =========================================================================
 * Three features only, built directly on the HAL/MCAL drivers below:
 *   1. Recognition: match -> "Welcome, <name>" on the LCD
 *   2. 3 consecutive failed matches -> buzzer alarm
 *   3. Menu (hold BACK from idle): add / delete / modify fingerprints
 *
 * Built on top of the provided LCD / TWI+EEPROM / UART / DIO drivers.
 * See README.md for the pin map and wiring notes.
 * =========================================================================
 */
#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>

#include "../Include/MCAL/DIO/DIO_Interface.h"
#include "../Include/HAL/EEPROM/EEPROM_Interface.h"
#include "../Include/HAL/LCD/LCD_Interface.h"
#include "../Include/HAL/FINGERPRINT/FINGERPRINT_Interface.h"
#include "../Include/HAL/USERS/USERS_Interface.h"
#include "../Include/HAL/USERS/USERS_Cfg.h"
#include "../Include/HAL/BUTTONS/BUTTONS_Interface.h"
#include "../Include/HAL/BUZZER/BUZZER_Interface.h"
#include "../Include/HAL/CTRL/CTRL_Interface.h"
#include "../Include/APP/APP_Cfg.h"

/* ---------------------------------------------------------------------
 * Init
 * --------------------------------------------------------------------- */

static void APP_voidSystemInit(void)
{
    DIO_voidInit();
    LCD_voidInit();
    EEPROM_Init();   /* initializes I2C/TWI internally */
    FP_voidInit();

    LCD_voidSendString((u8 *)"FP Security Sys");
    LCD_u8GoToXY(LCD_u8_LINE2, 0);
    LCD_voidSendString((u8 *)"Initializing...");

    if (FP_u8VerifyPassword() != FP_OK) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Sensor error!");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString((u8 *)"Check wiring");
        while (1) { BUZZ_voidBeep(200); _delay_ms(500); }
    }
    _delay_ms(1000);
}

/* ---------------------------------------------------------------------
 * Name entry: UP/DOWN scroll the current letter, SELECT accepts it and
 * moves on, BACK ends the name early.
 * --------------------------------------------------------------------- */

static const char CHARSET[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#define CHARSET_LEN (sizeof(CHARSET) - 1)

static void APP_voidEnterName(u8 *A_pu8Out, u8 A_u8MaxLen)
{
    u8 Local_u8Pos = 0;
    u8 Local_u8Idx = 1; /* start on 'A' */
    memset(A_pu8Out, 0, A_u8MaxLen + 1);

    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)"Enter name:");

    while (Local_u8Pos < A_u8MaxLen) {
        A_pu8Out[Local_u8Pos] = CHARSET[Local_u8Idx];

        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString((u8 *)"                ");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString(A_pu8Out);

        u8 Local_u8Btn = BTN_u8WaitForPress();
        if (Local_u8Btn == BTN_UP) {
            Local_u8Idx = (Local_u8Idx + 1) % CHARSET_LEN;
        } else if (Local_u8Btn == BTN_DOWN) {
            Local_u8Idx = (Local_u8Idx == 0) ? (CHARSET_LEN - 1) : (Local_u8Idx - 1);
        } else if (Local_u8Btn == BTN_SELECT) {
            Local_u8Pos++;
            Local_u8Idx = 1;
        } else if (Local_u8Btn == BTN_BACK) {
            break;
        }
    }
    A_pu8Out[Local_u8Pos] = '\0';
}

/* ---------------------------------------------------------------------
 * Admin menu actions
 * --------------------------------------------------------------------- */

static u16 APP_u16FindFreeSlot(void)
{
    u16 Local_u16Id;
    for (Local_u16Id = 1; Local_u16Id <= USERS_MAX_COUNT; Local_u16Id++)
        if (!USERS_u8Exists(Local_u16Id))
            return Local_u16Id;
    return 0; /* database full */
}

static void APP_voidAdminAddUser(void)
{
    u16 Local_u16Slot = APP_u16FindFreeSlot();
    if (Local_u16Slot == 0) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"DB full!");
        _delay_ms(1500);
        return;
    }

    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)"Place finger");
    LCD_u8GoToXY(LCD_u8_LINE2, 0);
    LCD_voidSendString((u8 *)"(scan 1 of 2)");

    u8 Local_u8Code = FP_u8Enroll(Local_u16Slot);
    if (Local_u8Code != FP_OK) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Enroll failed");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString((u8 *)"code:");
        LCD_voidDisplayNumber(Local_u8Code);
        BUZZ_voidBeep(300);
        _delay_ms(1500);
        return;
    }

    u8 Local_au8Name[USERS_NAME_LEN + 1];
    APP_voidEnterName(Local_au8Name, USERS_NAME_LEN);
    USERS_voidSave(Local_u16Slot, Local_au8Name);

    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)"Saved as:");
    LCD_u8GoToXY(LCD_u8_LINE2, 0);
    LCD_voidSendString(Local_au8Name);
    _delay_ms(1500);
}

static void APP_voidAdminDeleteUser(void)
{
    u16 Local_u16Id = 1;
    u8 Local_au8Name[USERS_NAME_LEN + 1];

    while (Local_u16Id <= USERS_MAX_COUNT && !USERS_u8Exists(Local_u16Id)) Local_u16Id++;
    if (Local_u16Id > USERS_MAX_COUNT) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"No users saved");
        _delay_ms(1500);
        return;
    }

    while (1) {
        USERS_voidGetName(Local_u16Id, Local_au8Name);
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Delete user?");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidDisplayNumber(Local_u16Id);
        LCD_voidSendString((u8 *)":");
        LCD_voidSendString(Local_au8Name);

        u8 Local_u8Btn = BTN_u8WaitForPress();
        if (Local_u8Btn == BTN_UP || Local_u8Btn == BTN_DOWN) {
            s16 Local_s16Step = (Local_u8Btn == BTN_UP) ? 1 : -1;
            u16 Local_u16Next = Local_u16Id;
            do {
                Local_u16Next = (u16)(Local_u16Next + Local_s16Step);
                if (Local_u16Next < 1) Local_u16Next = USERS_MAX_COUNT;
                if (Local_u16Next > USERS_MAX_COUNT) Local_u16Next = 1;
            } while (!USERS_u8Exists(Local_u16Next) && Local_u16Next != Local_u16Id);
            Local_u16Id = Local_u16Next;
        } else if (Local_u8Btn == BTN_SELECT) {
            FP_u8Delete(Local_u16Id);
            USERS_voidDelete(Local_u16Id);
            LCD_voidClearDisplay();
            LCD_voidSendString((u8 *)"Deleted.");
            _delay_ms(1200);
            return;
        } else if (Local_u8Btn == BTN_BACK) {
            return;
        }
    }
}

static void APP_voidAdminModifyUser(void)
{
    u16 Local_u16Id = 1;
    u8 Local_au8Name[USERS_NAME_LEN + 1];

    while (Local_u16Id <= USERS_MAX_COUNT && !USERS_u8Exists(Local_u16Id)) Local_u16Id++;
    if (Local_u16Id > USERS_MAX_COUNT) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"No users saved");
        _delay_ms(1500);
        return;
    }

    while (1) {
        USERS_voidGetName(Local_u16Id, Local_au8Name);
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Rename user?");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidDisplayNumber(Local_u16Id);
        LCD_voidSendString((u8 *)":");
        LCD_voidSendString(Local_au8Name);

        u8 Local_u8Btn = BTN_u8WaitForPress();
        if (Local_u8Btn == BTN_UP || Local_u8Btn == BTN_DOWN) {
            s16 Local_s16Step = (Local_u8Btn == BTN_UP) ? 1 : -1;
            u16 Local_u16Next = Local_u16Id;
            do {
                Local_u16Next = (u16)(Local_u16Next + Local_s16Step);
                if (Local_u16Next < 1) Local_u16Next = USERS_MAX_COUNT;
                if (Local_u16Next > USERS_MAX_COUNT) Local_u16Next = 1;
            } while (!USERS_u8Exists(Local_u16Next) && Local_u16Next != Local_u16Id);
            Local_u16Id = Local_u16Next;
        } else if (Local_u8Btn == BTN_SELECT) {
            u8 Local_au8NewName[USERS_NAME_LEN + 1];
            APP_voidEnterName(Local_au8NewName, USERS_NAME_LEN);
            USERS_voidSave(Local_u16Id, Local_au8NewName);
            LCD_voidClearDisplay();
            LCD_voidSendString((u8 *)"Updated.");
            _delay_ms(1200);
            return;
        } else if (Local_u8Btn == BTN_BACK) {
            return;
        }
    }
}

static void APP_voidAdminMenu(void)
{
    const char *Local_apcItems[] = { "Add user", "Delete user", "Modify user", "Exit" };
    u8 Local_u8Idx = 0;

    while (1) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Menu:");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString((u8 *)Local_apcItems[Local_u8Idx]);

        u8 Local_u8Btn = BTN_u8WaitForPress();
        if (Local_u8Btn == BTN_UP)   Local_u8Idx = (Local_u8Idx == 0) ? 3 : Local_u8Idx - 1;
        else if (Local_u8Btn == BTN_DOWN) Local_u8Idx = (Local_u8Idx + 1) % 4;
        else if (Local_u8Btn == BTN_BACK) return;
        else if (Local_u8Btn == BTN_SELECT) {
            switch (Local_u8Idx) {
                case 0: APP_voidAdminAddUser(); break;
                case 1: APP_voidAdminDeleteUser(); break;
                case 2: APP_voidAdminModifyUser(); break;
                case 3: return;
            }
        }
    }
}

/* ---------------------------------------------------------------------
 * Control panel (shown after a successful fingerprint match)
 * --------------------------------------------------------------------- */

static void APP_voidShowDeviceStatus(u8 A_u8Sel)
{
    static const char *Local_apcNames[] = { "LED 1", "LED 2", "LED 3"};
    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)Local_apcNames[A_u8Sel]);
    LCD_u8GoToXY(LCD_u8_LINE2, 0);
    LCD_voidSendString(CTRL_u8IsOn(A_u8Sel) ? (u8 *)"State: ON " : (u8 *)"State: OFF");
}

static void APP_voidControlPanel(void)
{
    u8 Local_u8Sel = 0;
    APP_voidShowDeviceStatus(Local_u8Sel);

    while (1) {
        u8 Local_u8Btn = BTN_u8WaitForPress();
        if (Local_u8Btn == BTN_BACK) return; /* re-lock the system */

        if (Local_u8Btn == BTN_UP)   Local_u8Sel = (Local_u8Sel == 0) ? 3 : Local_u8Sel - 1;
        else if (Local_u8Btn == BTN_DOWN) Local_u8Sel = (Local_u8Sel + 1) % 4;
        else if (Local_u8Btn == BTN_SELECT) CTRL_voidToggle(Local_u8Sel);

        APP_voidShowDeviceStatus(Local_u8Sel);
    }
}

/* ---------------------------------------------------------------------
 * Alarm / lockout
 * --------------------------------------------------------------------- */

static void APP_voidTriggerAlarmLockout(void)
{
    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)"!! ALARM !!");
    LCD_u8GoToXY(LCD_u8_LINE2, 0);
    LCD_voidSendString((u8 *)"Access denied");

    BUZZ_voidAlarm(APP_LOCKOUT_SECONDS); /* returns early if BTN_BACK is pressed */

    LCD_voidClearDisplay();
    LCD_voidSendString((u8 *)"System reset.");
    _delay_ms(1000);
}

/* ---------------------------------------------------------------------
 * Main
 * --------------------------------------------------------------------- */

int main(void)
{
    u8 Local_u8FailCount = 0;

    APP_voidSystemInit();

    while (1) {
        LCD_voidClearDisplay();
        LCD_voidSendString((u8 *)"Place finger,");
        LCD_u8GoToXY(LCD_u8_LINE2, 0);
        LCD_voidSendString((u8 *)"hold BACK=Menu");

        /* deliberate-hold window to reach the admin menu without scanning */
        if (BTN_u8GetPressed() == BTN_BACK) {
            _delay_ms(600);
            if (BTN_u8GetPressed() == BTN_BACK) {
                while (BTN_u8GetPressed() != BTN_NONE) _delay_ms(10);
                APP_voidAdminMenu();
                continue;
            }
        }

        u16 Local_u16MatchedId = 0, Local_u16Score = 0;
        u8 Local_u8Code = FP_u8Identify(&Local_u16MatchedId, &Local_u16Score);

        if (Local_u8Code == FP_OK) {
            u8 Local_au8Name[USERS_NAME_LEN + 1];
            USERS_voidGetName(Local_u16MatchedId, Local_au8Name);

            LCD_voidClearDisplay();
            LCD_voidSendString((u8 *)"Welcome,");
            LCD_u8GoToXY(LCD_u8_LINE2, 0);
            LCD_voidSendString(Local_au8Name);
            BUZZ_voidBeep(80);
            _delay_ms(1500); /* hold the greeting on screen, then re-arm scanning */

            Local_u8FailCount = 0;
            APP_voidControlPanel();
        }
        else if (Local_u8Code == FP_ERR_NO_FINGER) {
            _delay_ms(200); /* nothing on the sensor yet - keep looping */
        }
        else {
            /* a finger WAS scanned, but not recognized */
            Local_u8FailCount++;
            LCD_voidClearDisplay();
            LCD_voidSendString((u8 *)"Not recognized");
            LCD_u8GoToXY(LCD_u8_LINE2, 0);
            LCD_voidSendString((u8 *)"Attempt ");
            LCD_voidDisplayNumber(Local_u8FailCount);
            LCD_voidSendString((u8 *)"/3");
            BUZZ_voidBeep(150);
            _delay_ms(1000);

            if (Local_u8FailCount >= APP_MAX_FAILED_ATTEMPTS) {
                APP_voidTriggerAlarmLockout();
                Local_u8FailCount = 0;
            }
        }
    }

    return 0;
}


/*
 * button_test.c - TEMPORARY diagnostic, not part of the real project.
 * -----------------------------------------------------------------------
 * Swap this in as Src/main.c for a minute (rename your real main.c to
 * main.c.bak first so the linker doesn't see two main()s), flash it, and
 * press each button one at a time. The buzzer should beep for as long as
 * ANY of the 4 button pins reads LOW - that tells us which pins actually
 * react, with zero LCD/sensor code in the way to confuse things.
 *
 * Put your real main.c back afterwards - this file is only for testing.
 * -----------------------------------------------------------------------
 */
//#include "../Include/Lib/STD_Types.h"
//#include "../Include/Lib/BIT_Math.h"
//#include <avr/io.h>
//#include "../Include/MCAL/DIO/DIO_Interface.h"
//
//int main(void)
//{
////	MCUCSR |= (1 << JTD);
////	MCUCSR |= (1 << JTD);
//
//    DIO_voidInit();
//
////    DIO_u8SetPinDirection(DIO_u8_PORTC, DIO_u8_PIN5, DIO_u8_INPUT);
////        DIO_u8SetPinValue(DIO_u8_PORTC, DIO_u8_PIN5, DIO_u8_HIGH); //
//
//    while (1) {
//        u8 Local_u8Val;
//        u8 Local_u8AnyPressed = 0;
//
//        /* PC3=UP, PC4=DOWN, PC5=SELECT, PC6=BACK - matches your current
//         * BUTTONS_Cfg.h. Change these 4 pin numbers here if you want to
//         * probe different physical pins while debugging. */
//
//          DIO_u8GetPinValue(DIO_u8_PORTD, DIO_u8_PIN3, &Local_u8Val);
//          if (Local_u8Val == DIO_u8_LOW) Local_u8AnyPressed = 1;
//
//        DIO_u8GetPinValue(DIO_u8_PORTD, DIO_u8_PIN4, &Local_u8Val);
//        if (Local_u8Val == DIO_u8_LOW) Local_u8AnyPressed = 1;
//
//        DIO_u8GetPinValue(DIO_u8_PORTD, DIO_u8_PIN5, &Local_u8Val);
//        if (Local_u8Val == DIO_u8_LOW) Local_u8AnyPressed = 1;
//
//        DIO_u8GetPinValue(DIO_u8_PORTD, DIO_u8_PIN6, &Local_u8Val);
//        if (Local_u8Val == DIO_u8_LOW) Local_u8AnyPressed = 1;
//
//        /* buzzer pin per your current BUZZER_Cfg.h: PORTD, PIN7 */
//        DIO_u8SetPinValue(DIO_u8_PORTD, DIO_u8_PIN7,
//                           Local_u8AnyPressed ? DIO_u8_LOW : DIO_u8_HIGH);
//    }
//
//    return 0;
//}


/*
 * reset_all.c - TEMPORARY utility, not part of the real project.
 * -----------------------------------------------------------------------
 * Wipes EVERY fingerprint template stored on the AS608 sensor itself,
 * AND clears the matching name table in the external EEPROM (so you
 * don't end up with orphaned names pointing at empty slots).
 *
 * HOW TO USE:
 *   1. Rename your real Src/main.c -> main.c.bak (only one main() allowed)
 *   2. Drop this file into Src/ as main.c (or just build it alongside
 *      after removing/renaming the other main.c)
 *   3. Build, flash, power on. Watch the LCD for the result.
 *   4. Put your real main.c back afterwards.
 *
 * This is DESTRUCTIVE and cannot be undone - it deletes every enrolled
 * fingerprint and every stored name.
 * -----------------------------------------------------------------------
 */
//#include "../Include/Lib/STD_Types.h"
//#include "../Include/Lib/BIT_Math.h"
//
//#define F_CPU 8000000UL
//#include <util/delay.h>
//
//#include "../Include/MCAL/DIO/DIO_Interface.h"
//#include "../Include/HAL/EEPROM/EEPROM_Interface.h"
//#include "../Include/HAL/LCD/LCD_Interface.h"
//#include "../Include/HAL/FINGERPRINT/FINGERPRINT_Interface.h"
//#include "../Include/HAL/USERS/USERS_Interface.h"
//#include "../Include/HAL/USERS/USERS_Cfg.h"
//#include "../Include/HAL/BUZZER/BUZZER_Interface.h"
//
//int main(void)
//{
//    DIO_voidInit();
//    LCD_voidInit();
//    EEPROM_Init();
//    FP_voidInit();
//
//    LCD_voidSendString((u8 *)"Factory reset..");
//
//    if (FP_u8VerifyPassword() != FP_OK) {
//        LCD_voidClearDisplay();
//        LCD_voidSendString((u8 *)"Sensor error!");
//        LCD_u8GoToXY(LCD_u8_LINE2, 0);
//        LCD_voidSendString((u8 *)"Check wiring");
//        while (1) { BUZZ_voidBeep(200); _delay_ms(500); }
//    }
//
//    /* 1) wipe every fingerprint template stored on the AS608 itself */
//    u8 Local_u8Code = FP_u8EmptyDatabase();
//
//    /* 2) clear the matching name table in the external EEPROM,
//     *    regardless of whether step 1 fully succeeded, so the two
//     *    stores can't end up disagreeing with each other */
//    u16 Local_u16Id;
//    for (Local_u16Id = 1; Local_u16Id <= USERS_MAX_COUNT; Local_u16Id++)
//        USERS_voidDelete(Local_u16Id);
//
//    LCD_voidClearDisplay();
//    if (Local_u8Code == FP_OK) {
//        LCD_voidSendString((u8 *)"All cleared!");
//        LCD_u8GoToXY(LCD_u8_LINE2, 0);
//        LCD_voidSendString((u8 *)"Memory is empty");
//        BUZZ_voidBeep(100);
//    } else {
//        LCD_voidSendString((u8 *)"Sensor wipe");
//        LCD_u8GoToXY(LCD_u8_LINE2, 0);
//        LCD_voidSendString((u8 *)"failed, code:");
//        LCD_voidDisplayNumber(Local_u8Code);
//        BUZZ_voidBeep(300);
//    }
//
//    while (1) { /* halt here - flash your real project next */ }
//    return 0;
//}
