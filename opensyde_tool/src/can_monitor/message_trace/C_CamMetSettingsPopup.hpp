//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for trace settings popup dialog
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETSETTINGSPOPUP_H
#define C_CAMMETSETTINGSPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMetSettingsPopup;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetSettingsPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMetSettingsPopup(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_CamMetSettingsPopup(void) override;

   // Set for initialization
   void SetValues(const bool oq_DisplayTimestampAbsoluteTimeOfDay, const uint32_t ou32_TraceBufferSize) const;

   // Get
   bool GetDisplayTimestampAbsoluteTimeOfDay(void) const;
   uint32_t GetTraceBufferSize(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CamMetSettingsPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   //Avoid call
   C_CamMetSettingsPopup(const C_CamMetSettingsPopup &);
   C_CamMetSettingsPopup & operator =(const C_CamMetSettingsPopup &) &;

   void m_InitStaticNames(void) const;
   void m_InitStaticGuiElements(void) const;
   void m_OnCancel(void) const;
   void m_OnOk(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
