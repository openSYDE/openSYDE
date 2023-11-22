//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog shows settings for creating a Service Project
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPCREATESERVICEPROJDIALOGWIDGET_HPP
#define C_POPCREATESERVICEPROJDIALOGWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"

#include "C_PopServiceProjSettingsModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_PopCreateServiceProjDialogWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopCreateServiceProjDialogWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_PopCreateServiceProjDialogWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_PopCreateServiceProjDialogWidget(void) override;

   void InitStaticNames(void) const;
   void PrepareDialog(const std::vector<uint32_t> & orc_ViewIndices);
   void GetCheckedItems(std::vector<uint32_t> & orc_ViewIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerView) const;
   QString GetPassword(void) const;
   QString GetSpPath(void) const;
   void SetSpPath(const QString & orc_SpPath) const;
   int32_t SaveCurrentProjectForServiceMode(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_PopCreateServiceProjDialogWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   int32_t m_CheckPath(void);
   void m_OkClicked(void);
   void m_OnCancel(void);
   void m_SpPathClicked(void);
   void m_InitPwdLineEdit(void);
   void m_ShowPassword(void);
   void m_HidePassword(void);
   void m_UpdateSelection(const int32_t os32_SelectionCount) const;
   std::vector<std::array<bool, 3 > > m_CollectViewConfig(void);
   void m_SavePermissionsToUserSettings(std::vector<std::array<bool, 3> > & orc_ViewConfigs);
   //   int32_t m_FileExists(const QString & orc_FilePath);

   stw::opensyde_gui_logic::C_PopServiceProjSettingsModel mc_Model;

   //Avoid call
   C_PopCreateServiceProjDialogWidget(const C_PopCreateServiceProjDialogWidget &);
   C_PopCreateServiceProjDialogWidget & operator =(const C_PopCreateServiceProjDialogWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
