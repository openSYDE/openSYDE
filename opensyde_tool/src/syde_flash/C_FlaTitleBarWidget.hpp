//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Title bar widget for SYDEflash
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLATITLEBARWIDGET_HPP
#define C_FLATITLEBARWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaTitleBarWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaTitleBarWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_FlaTitleBarWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaTitleBarWidget(void) override;

   void InitStaticNames(void) const;
   void EnableActions(const bool oq_Enabled);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateNode(void);
   void SigSearchNode(void);
   void SigConfigureNode(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_FlaTitleBarWidget * mpc_Ui;

   //Avoid call
   C_FlaTitleBarWidget(const C_FlaTitleBarWidget &);
   C_FlaTitleBarWidget & operator =(const C_FlaTitleBarWidget &) &;

   void m_ShowAbout(void);
   void m_TriggerHelp(void);
   void m_SetButtonsText(const bool oq_IconOnly) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
