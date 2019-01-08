//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI text element data: stores UI information (implementation)

   UI text element data: stores UI information

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "constants.h"
#include "C_PuiBsTextElement.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     27.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiBsTextElement::C_PuiBsTextElement(void) :
   C_PuiBsBox(),
   c_UIFontStyle(QFont("Segoe UI", 18)),
   c_UIFontColorBright(mc_STYLE_GUIDE_COLOR_34),
   c_UIFontColorDark(mc_STYLE_GUIDE_COLOR_0),
   c_UIText("Text Element")
{
   c_UIFontStyle.setPixelSize(18);
   c_UIFontStyle.setItalic(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     15.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiBsTextElement::~C_PuiBsTextElement(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiBsTextElement::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;
   const QString c_Font = this->c_UIFontStyle.toString();

   stw_scl::C_SCLChecksums::CalcCRC32(c_Font.toStdString().c_str(), c_Font.length(), oru32_HashValue);

   sn_Value = this->c_UIFontColorBright.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorBright.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorBright.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorBright.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   sn_Value = this->c_UIFontColorDark.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorDark.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorDark.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIFontColorDark.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(this->c_UIText.toStdString().c_str(), this->c_UIText.length(), oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}
