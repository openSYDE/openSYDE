//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for "About openSYDE" popup dialog. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGABOUTDIALOG_H
#define C_NAGABOUTDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagAboutDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagAboutDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagAboutDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString oc_ProductName,
                             const QString oc_LogoUrl, const uint32_t ou32_Margin,
                             const QString oc_OptionalComponents = "");
   ~C_NagAboutDialog(void) override;

   void InitStaticNames(void) const;
   void InitDynamicNames() const;
   void InitLogo() const;
   void InitLayout() const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_NagAboutDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   //Avoid call
   C_NagAboutDialog(const C_NagAboutDialog &);
   C_NagAboutDialog & operator =(const C_NagAboutDialog &) &;

   QString mc_ProductName;
   QString mc_LogoUrl;
   uint32_t mu32_Margin;
   QString mc_OptionalComponents;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
