//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for choosing and configuring the CAN communication DLL

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSEDLLCONFIGURATIONDIALOG_H
#define C_SYVSEDLLCONFIGURATIONDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_PuiSvPc.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvSeDllConfigurationDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeDllConfigurationDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvSeDllConfigurationDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SyvSeDllConfigurationDialog() override;

   void InitText() const;
   void SetDllType(const stw::opensyde_gui_logic::C_PuiSvPc::E_CanDllType oe_Type) const;
   void SetCustomDllPath(const QString & orc_Path) const;
   void SetBitrate(const uint64_t ou64_Bitrate);
   stw::opensyde_gui_logic::C_PuiSvPc::E_CanDllType GetDllType(void) const;
   QString GetCustomDllPath(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvSeDllConfigurationDialog(const C_SyvSeDllConfigurationDialog &);
   C_SyvSeDllConfigurationDialog & operator =(const C_SyvSeDllConfigurationDialog &) &;

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;
   void m_ConfigureDllClicked(void) const;
   void m_TestConnectionClicked(void) const;
   void m_ConcretDllClicked(void) const;
   void m_OtherDllClicked(void) const;
   void m_OnBrowse(void) const;
   void m_OnDroppedDllPath(void);
   void m_SetCustomDllPath(const QString & orc_New) const;

   void m_ShowCustomDllPath(const bool oq_Active) const;
   bool m_CheckCustomDllPath(void) const;
   QString m_GetAbsoluteDllPath(void) const;

   Ui::C_SyvSeDllConfigurationDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   uint64_t mu64_Bitrate;
};
}
}

#endif
