//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget for filter ID selection from database (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASESELECTIONPOPUP_H
#define C_CAMMOSDATABASESELECTIONPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <array>
#include <QWidget>
#include "stwtypes.h"
#include "C_OgeContextMenu.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDatabaseSelectionPopup;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseSelectionPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMosDatabaseSelectionPopup(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const bool oq_AllowMultiSelect = false);
   ~C_CamMosDatabaseSelectionPopup(void);

   void InitStaticNames(void) const;
   void SaveUserSettings(void) const;
   std::vector<std::array<QString, 2> > GetSelectedDataElements(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_CamMosDatabaseSelectionPopup * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_HandleHiding(void) const;
   void m_CancelClicked(void);
   void m_OnSearch(const QString & orc_Text) const;
   void m_UpdateSelection(const stw_types::sintn osn_SelectionCount) const;
   void m_SetupContextMenu(const bool & orq_MultiSelect);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   //Avoid call
   C_CamMosDatabaseSelectionPopup(const C_CamMosDatabaseSelectionPopup &);
   C_CamMosDatabaseSelectionPopup & operator =(const C_CamMosDatabaseSelectionPopup &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
