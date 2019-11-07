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

#include "stwtypes.h"

#include "C_PuiSvPc.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvSeDllConfigurationDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeDllConfigurationDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvSeDllConfigurationDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SyvSeDllConfigurationDialog();

   void InitText() const;
   void SetDllType(const stw_opensyde_gui_logic::C_PuiSvPc::E_CANDllType oe_Type) const;
   void SetCustomDllPath(const QString & orc_Path) const;
   void SetBitrate(const stw_types::uint64 ou64_Bitrate);
   stw_opensyde_gui_logic::C_PuiSvPc::E_CANDllType GetDllType(void) const;
   QString GetCustomDllPath(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_SyvSeDllConfigurationDialog(const C_SyvSeDllConfigurationDialog &);
   C_SyvSeDllConfigurationDialog & operator =(const C_SyvSeDllConfigurationDialog &);

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;
   void m_ConfigureDllClicked(void) const;
   void m_TestConnectionClicked(void) const;
   void m_ConcretDllClicked(void) const;
   void m_OtherDllClicked(void) const;
   void m_OnBrowse(void) const;

   void m_ShowCustomDllPath(const bool oq_Active) const;
   bool m_CheckCustomDllPath(void) const;
   QString m_GetAbsoluteDllPath(void) const;

   Ui::C_SyvSeDllConfigurationDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   stw_types::uint64 mu64_Bitrate;
};
}

#endif
