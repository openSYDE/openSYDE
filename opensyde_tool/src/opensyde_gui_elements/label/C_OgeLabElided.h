//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label that automatically elides text on resize event
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABELIDED_H
#define C_OGELABELIDED_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabGenericNoPaddingNoMargins.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabElided :
   public C_OgeLabGenericNoPaddingNoMargins
{
   Q_OBJECT

public:
   explicit C_OgeLabElided(QWidget * const opc_Parent = NULL);
   void SetCompleteText(const QString & orc_Text);
   QString GetCompleteText(void) const;
   void SetFontForEliding(const QFont & orc_Font);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   QString mc_CompleteText;
   QFont mc_Font;
   void m_UpdateElidedText(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
