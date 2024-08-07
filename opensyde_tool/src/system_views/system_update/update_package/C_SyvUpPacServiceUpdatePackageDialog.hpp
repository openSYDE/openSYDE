//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for creating service update package
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSERVICEUPDATEPACKAGEDIALOG_H
#define C_SYVUPPACSERVICEUPDATEPACKAGEDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacServiceUpdatePackageDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacServiceUpdatePackageDialog :
   public QWidget
{
   Q_OBJECT

public:
   enum E_PackageType
   {
      eARCHIVE = 0,
      eDIRECTORY = 1,
      eSECURE_ARCHIVE = 2
   };

   explicit C_SyvUpPacServiceUpdatePackageDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SyvUpPacServiceUpdatePackageDialog(void) override;

   void InitStaticNames(void) const;

   E_PackageType GetSelectedOption() const;

private:
   //Avoid call
   C_SyvUpPacServiceUpdatePackageDialog(const C_SyvUpPacServiceUpdatePackageDialog &);

   C_SyvUpPacServiceUpdatePackageDialog & operator =(const C_SyvUpPacServiceUpdatePackageDialog &) &;

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;

   Ui::C_SyvUpPacServiceUpdatePackageDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
