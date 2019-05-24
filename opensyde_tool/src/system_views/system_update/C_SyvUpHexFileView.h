//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Data Block file details (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPHEXFILEVIEW_H
#define C_SYVUPHEXFILEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OsyHexFile.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpHexFileView;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpHexFileView :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpHexFileView(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_File);
   ~C_SyvUpHexFileView(void);

   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvUpHexFileView * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const QString mc_File;
   static const QString mhc_StartTD;
   static const QString mhc_ContinueTD;

   void m_LoadInfo(void) const;
   void m_OkClicked(void);
   static void mh_AddFileSection(const QString & orc_Path, QString & orc_Content);
   static QString mh_GetMD5Hex(const QString & orc_Path);
   static void mh_AddDataInformation(stw_opensyde_core::C_OsyHexFile & orc_HexFile, QString & orc_Content);
   static void mh_AddApplicationInformation(stw_opensyde_core::C_OsyHexFile & orc_HexFile, QString & orc_Content);

   //Avoid call
   C_SyvUpHexFileView(const C_SyvUpHexFileView &);
   C_SyvUpHexFileView & operator =(const C_SyvUpHexFileView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
