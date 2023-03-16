//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI text element data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSTEXTELEMENT_H
#define C_PUIBSTEXTELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFont>
#include <QColor>

#include "stwtypes.hpp"

#include "C_PuiBsBox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsTextElement :
   public C_PuiBsBox
{
public:
   C_PuiBsTextElement(void);
   ~C_PuiBsTextElement(void) override;

   void CalcHash(uint32_t & oru32_HashValue) const override;

   QFont c_UiFontStyle;        ///< font style of text element
   QColor c_UiFontColorBright; ///< font color of text element in bright mode
   QColor c_UiFontColorDark;   ///< font color of text element in dark mode
   QString c_UiText;           ///< shown text of text element
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
