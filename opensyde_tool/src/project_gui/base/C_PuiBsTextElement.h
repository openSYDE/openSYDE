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

#include "stwtypes.h"

#include "C_PuiBsBox.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsTextElement :
   public C_PuiBsBox
{
public:
   C_PuiBsTextElement(void);
   virtual ~C_PuiBsTextElement(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QFont c_UIFontStyle;        ///< font style of text element
   QColor c_UIFontColorBright; ///< font color of text element in bright mode
   QColor c_UIFontColorDark;   ///< font color of text element in dark mode
   QString c_UIText;           ///< shown text of text element
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
