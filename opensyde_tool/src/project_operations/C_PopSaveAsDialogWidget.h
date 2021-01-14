//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for save-as dialog (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPSAVEASDIALOGWIDGET_H
#define C_POPSAVEASDIALOGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_UsHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_PopSaveAsDialogWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopSaveAsDialogWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_PopSaveAsDialogWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_PopSaveAsDialogWidget(void);

   void InitStaticNames(void) const;
   void SaveUserSettings(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_PopSaveAsDialogWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const stw_types::sintn mhsn_VERSION_INDEX_V2;
   static const stw_types::sintn mhsn_VERSION_INDEX_V3;

   void m_InitDefaultProjectName(void) const;
   stw_types::sint32 m_SaveToFile(const QString & orc_File, const bool oq_UseDeprecatedFileFormatV2) const;
   QString m_GetValidPath(const QString & orc_Path) const;
   void m_OnBrowse(void);
   void m_OnSave(void);
   void m_OnCancel();

   //Avoid call
   C_PopSaveAsDialogWidget(const C_PopSaveAsDialogWidget &);
   C_PopSaveAsDialogWidget & operator =(const C_PopSaveAsDialogWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
