//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog shows settings for creating a Service Project
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPCREATESERVICEPROJDIALOGWIDGET_H
#define C_POPCREATESERVICEPROJDIALOGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"

#include "C_PopServiceProjSettingsModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_PopCreateServiceProjDialogWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopCreateServiceProjDialogWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_PopCreateServiceProjDialogWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_PopCreateServiceProjDialogWidget(void);

   void InitStaticNames(void) const;
   void PrepareDialog(const std::vector<stw_types::uint32> & orc_ViewIndices);
   void GetCheckedItems(std::vector<stw_types::uint32> & orc_ViewIndices,
                        std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerView) const;
   QString GetPassword(void) const;
   QString GetSpPath(void) const;
   void SetSpPath(const QString & orc_SpPath) const;
   stw_types::sint32 SaveCurrentProjectForServiceMode(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_PopCreateServiceProjDialogWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   stw_types::sint32 m_CheckPath(void);
   void m_OkClicked(void);
   void m_OnCancel(void);
   void m_SpPathClicked(void);
   void m_InitPwdLineEdit(void);
   void m_ShowPassword(void);
   void m_HidePassword(void);
   void m_UpdateSelection(const stw_types::sintn osn_SelectionCount) const;
   std::vector<std::array<bool, 3 > > m_CollectViewConfig(void);
   void m_SavePermissionsToUserSettings(std::vector<std::array<bool, 3> > & orc_ViewConfigs);
   //   stw_types::sint32 m_FileExists(const QString & orc_FilePath);

   stw_opensyde_gui_logic::C_PopServiceProjSettingsModel mc_Model;

   //Avoid call
   C_PopCreateServiceProjDialogWidget(const C_PopCreateServiceProjDialogWidget &);
   C_PopCreateServiceProjDialogWidget & operator =(const C_PopCreateServiceProjDialogWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
