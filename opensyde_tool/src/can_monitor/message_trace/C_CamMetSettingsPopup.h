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

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMetSettingsPopup;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetSettingsPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMetSettingsPopup(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_CamMetSettingsPopup(void);

   // Set for initialization
   void SetValues(const bool oq_DisplayTimestampAbsoluteTimeOfDay, const stw_types::uint32 ou32_TraceBufferSize) const;

   // Get
   bool GetDisplayTimestampAbsoluteTimeOfDay(void) const;
   stw_types::uint32 GetTraceBufferSize(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CamMetSettingsPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   //Avoid call
   C_CamMetSettingsPopup(const C_CamMetSettingsPopup &);
   C_CamMetSettingsPopup & operator =(const C_CamMetSettingsPopup &);

   void m_InitStaticNames(void) const;
   void m_InitStaticGUIElements(void) const;
   void m_OnCancel(void) const;
   void m_OnOk(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
