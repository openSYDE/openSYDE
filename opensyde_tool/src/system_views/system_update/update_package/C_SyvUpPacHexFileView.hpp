//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Data Block file details (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACHEXFILEVIEW_H
#define C_SYVUPPACHEXFILEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OsyHexFile.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacHexFileView;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacHexFileView :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacHexFileView(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_File);
   ~C_SyvUpPacHexFileView(void) override;

   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacHexFileView * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const QString mc_AbsoluteFilePath;
   static const QString mhc_START_TD;
   static const QString mhc_CONTINUE_TD;

   void m_LoadInfo(void) const;
   void m_OkClicked(void);
   static void mh_AddFileSection(const QString & orc_Path, QString & orc_Content);
   static QString mh_GetMD5Hex(const QString & orc_Path);
   static void mh_AddDataInformation(stw::opensyde_core::C_OsyHexFile & orc_HexFile, QString & orc_Content);
   static void mh_AddApplicationInformation(stw::opensyde_core::C_OsyHexFile & orc_HexFile, QString & orc_Content);

   //Avoid call
   C_SyvUpPacHexFileView(const C_SyvUpPacHexFileView &);
   C_SyvUpPacHexFileView & operator =(const C_SyvUpPacHexFileView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
