//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2019-2022  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include <circle/util.h>
#include "font.h"

LOGMODULE ("kernel");

CKernel *CKernel::s_pThis = nullptr;

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_USBHCI (&m_Interrupt, &m_Timer, TRUE),
	m_pKeyboard (0),
	m_SerialCDC (&m_Interrupt),
	m_GUI (&m_Screen, &m_Interrupt),
	m_ShutdownMode (ShutdownNone)
{
	s_pThis = this;
	// m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
	s_pThis = nullptr;
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_USBHCI.Initialize ();
	}

	if (bOK)
	{
		bOK = m_SerialCDC.Initialize ();
	}

	if (bOK)	
	{
		m_RPiTouchScreen.Initialize ();

		bOK = m_GUI.Initialize ();
	}

	// if (bOK)
	// {
	// 	bOK = m_Dash.Initialize ();
	// }

	return bOK;
}

static lv_obj_t * textarea;

TShutdownMode CKernel::Run (void)
{
	if (m_SerialCDC.UpdatePlugAndPlay ())
	{
		m_Logger.SetNewTarget (&m_SerialCDC);
	}
	LOGNOTE ("Compile time: " __DATE__ " " __TIME__);

	m_SerialCDC.RegisterReceiveHandler (usbCDCReceiveHandler);

	textarea = lv_textarea_create(lv_scr_act());
	// lv_obj_set_style_pad_all(lv_scr_act(), 10, 0);
	lv_obj_set_size(textarea, 800, 600);
    lv_obj_set_style_radius(textarea, 0, 0);
	lv_obj_align(textarea, LV_ALIGN_TOP_MID, 0, 0);
	// lv_textarea_set_text_selection(textarea, true);
    lv_obj_set_scrollbar_mode(textarea, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(textarea, LV_DIR_VER);

	FONT(ATKINSON_HYPERLEGIBLE_REGULAR_102, 16);
    ATKINSON_HYPERLEGIBLE_REGULAR_102_16->line_height = 32;
	lv_obj_set_style_text_font(textarea, ATKINSON_HYPERLEGIBLE_REGULAR_102_16, 0);

    lv_textarea_set_text(textarea, R"(It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of Light, it was the season of Darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to Heaven, we were all going direct the other way—in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only.

There were a king with a large jaw and a queen with a plain face, on the throne of England; there were a king with a large jaw and a queen with a fair face, on the throne of France. In both countries it was clearer than crystal to the lords of the State preserves of loaves and fishes, that things in general were settled for ever.

It was the year of Our Lord one thousand seven hundred and seventy-five. Spiritual revelations were conceded to England at that favoured period, as at this. Mrs. Southcott had recently attained her five-and-twentieth blessed birthday, of whom a prophetic private in the Life Guards had heralded the sublime appearance by announcing that arrangements were made for the swallowing up of London and Westminster. Even the Cock-lane ghost had been laid only a round dozen of years, after rapping out its messages, as the spirits of this very year last past (supernaturally deficient in originality) rapped out theirs. Mere messages in the earthly order of events had lately come to the English Crown and People, from a congress of British subjects in America: which, strange to relate, have proved more important to the human race than any communications yet received through any of the chickens of the Cock-lane brood.

France, less favoured on the whole as to matters spiritual than her sister of the shield and trident, rolled with exceeding smoothness down hill, making paper money and spending it. Under the guidance of her Christian pastors, she entertained herself, besides, with such humane achievements as sentencing a youth to have his hands cut off, his tongue torn out with pincers, and his body burned alive, because he had not kneeled down in the rain to do honour to a dirty procession of monks which passed within his view, at a distance of some fifty or sixty yards. It is likely enough that, rooted in the woods of France and Norway, there were growing trees, when that sufferer was put to death, already marked by the Woodman, Fate, to come down and be sawn into boards, to make a certain movable framework with a sack and a knife in it, terrible in history. It is likely enough that in the rough outhouses of some tillers of the heavy lands adjacent to Paris, there were sheltered from the weather that very day, rude carts, bespattered with rustic mire, snuffed about by pigs, and roosted in by poultry, which the Farmer, Death, had already set apart to be his tumbrils of the Revolution. But that Woodman and that Farmer, though they work unceasingly, work silently, and no one heard them as they went about with muffled tread: the rather, forasmuch as to entertain any suspicion that they were awake, was to be atheistical and traitorous.
    
In England, there was scarcely an amount of order and protection to justify much national boasting. Daring burglaries by armed men, and highway robberies, took place in the capital itself every night; families were publicly cautioned not to go out of town without removing their furniture to upholsterers’ warehouses for security; the highwayman in the dark was a City tradesman in the light, and, being recognised and challenged by his fellow-tradesman whom he stopped in his character of “the Captain,” gallantly shot him through the head and rode away; the mail was waylaid by seven robbers, and the guard shot three dead, and then got shot dead himself by the other four, “in consequence of the failure of his ammunition:” after which the mail was robbed in peace; that magnificent potentate, the Lord Mayor of London, was made to stand and deliver on Turnham Green, by one highwayman, who despoiled the illustrious creature in sight of all his retinue; prisoners in London gaols fought battles with their turnkeys, and the majesty of the law fired blunderbusses in among them, loaded with rounds of shot and ball; thieves snipped off diamond crosses from the necks of noble lords at Court drawing-rooms; musketeers went into St. Giles’s, to search for contraband goods, and the mob fired on the musketeers, and the musketeers fired on the mob, and nobody thought any of these occurrences much out of the common way. In the midst of them, the hangman, ever busy and ever worse than useless, was in constant requisition; now, stringing up long rows of miscellaneous criminals; now, hanging a housebreaker on Saturday who had been taken on Tuesday; now, burning people in the hand at Newgate by the dozen, and now burning pamphlets at the door of Westminster Hall; to-day, taking the life of an atrocious murderer, and to-morrow of a wretched pilferer who had robbed a farmer’s boy of sixpence.

All these things, and a thousand like them, came to pass in and close upon the dear old year one thousand seven hundred and seventy-five. Environed by them, while the Woodman and the Farmer worked unheeded, those two of the large jaws, and those other two of the plain and the fair faces, trod with stir enough, and carried their divine rights with a high hand. Thus did the year one thousand seven hundred and seventy-five conduct their Greatnesses, and myriads of small creatures—the creatures of this chronicle among the rest—along the roads that lay before them.
)");

	// lv_demo_widgets ();
	// dash_create(NULL);

	// unsigned nLastTime = 0;


	// for (unsigned nCount = 0; m_ShutdownMode == ShutdownNone; nCount++)
	// {
	while (true) {
		boolean bUpdated = m_USBHCI.UpdatePlugAndPlay ();
		if (bUpdated && m_pKeyboard == 0) {
			m_pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);
			if (m_pKeyboard != 0) {
				m_pKeyboard->RegisterRemovedHandler (KeyboardRemovedHandler);

#if 1	// set to 0 to test raw mode
				m_pKeyboard->RegisterShutdownHandler (ShutdownHandler);
				m_pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
#else
				m_pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);
#endif

				LOGNOTE("Just type something!");
			}
		}

		if (m_pKeyboard != 0) {
			// CUSBKeyboardDevice::UpdateLEDs() must not be called in interrupt context,
			// that's why this must be done here. This does nothing in raw mode.
			m_pKeyboard->UpdateLEDs ();
		}
		// dash_loop();
		m_GUI.Update (bUpdated);

		if (m_SerialCDC.UpdatePlugAndPlay ()) {
			m_Logger.SetNewTarget (&m_SerialCDC);
		}

	}

		// unsigned nNow = m_Timer.GetTime ();
		// if (nLastTime != nNow)
		// {
		// 	LOGNOTE ("Time is %u", nNow);

		// 	nLastTime = nNow;
		// }

	return ShutdownHalt;
}

void CKernel::usbCDCReceiveHandler(void *pBuffer, unsigned nLength)
{
	// assert (s_pThis);
	// s_pThis->m_Screen.Write (pBuffer, nLength);
	// s_pThis->m_Serial.Write (pBuffer, nLength);
}


void CKernel::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
#ifdef EXPAND_CHARACTERS
	while (*pString)
          {
	  CString s;
	  s.Format ("'%c' %d %02X\n", *pString, *pString, *pString);
          pString++;
	  s_pThis->m_Screen.Write (s, strlen (s));
          }
#else
	// s_pThis->m_Screen.Write (pString, strlen (pString));
	if (*pString == 0x7F) {
		lv_textarea_del_char(textarea);
	} else {
		if (pString[0] == 0x1B) {
			if (pString[2] == 'A') {
				lv_textarea_cursor_up(textarea);
			} else if (pString[2] == 'B') {
				lv_textarea_cursor_down(textarea);
			} else if (pString[2] == 'C') {
				lv_textarea_cursor_right(textarea);
			} else if (pString[2] == 'D') {
				lv_textarea_cursor_left(textarea);
			} else if (pString[2] == '3' && pString[3] == '~') {
				lv_textarea_del_char_forward(textarea);
			}
		} else {
			lv_textarea_add_text(textarea, pString);
		}
	}
#endif
}

void CKernel::ShutdownHandler (void)
{
	assert (s_pThis != 0);
	s_pThis->m_ShutdownMode = ShutdownReboot;
}

void CKernel::KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	assert (s_pThis != 0);

	CString Message;
	Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);

	for (unsigned i = 0; i < 6; i++)
	{
		if (RawKeys[i] != 0)
		{
			CString KeyCode;
			KeyCode.Format (" %02X", (unsigned) RawKeys[i]);

			Message.Append (KeyCode);
		}
	}

	LOGNOTE(Message);
}

void CKernel::KeyboardRemovedHandler (CDevice *pDevice, void *pContext)
{
	assert (s_pThis != 0);

	LOGNOTE("Keyboard removed");

	s_pThis->m_pKeyboard = 0;
}