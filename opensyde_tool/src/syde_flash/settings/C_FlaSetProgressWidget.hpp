//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Settings progress widget
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLASETPROGRESSWIDGET_HPP
#define C_FLASETPROGRESSWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaSetProgressWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaSetProgressWidget :
   public opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_FlaSetProgressWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaSetProgressWidget(void) override;

   void ClearProgress(void);
   void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;
   void ShowProgress(const QString & orc_Text);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHide(void);

private:
   Ui::C_FlaSetProgressWidget * mpc_Ui;

   //Avoid call
   C_FlaSetProgressWidget(const C_FlaSetProgressWidget &);
   C_FlaSetProgressWidget & operator =(const C_FlaSetProgressWidget &) &;

   void m_OnExpand(const bool oq_Expand) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
