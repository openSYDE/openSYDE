//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for user key selection (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENKEYSELECT_HPP
#define C_CAMGENKEYSELECT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamGenKeySelect;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenKeySelect :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamGenKeySelect(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                              const uint32_t ou32_MessageIndex);
   ~C_CamGenKeySelect(void) override;

   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CamGenKeySelect * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_MessageIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_LoadData(void) const;

   //Avoid call
   C_CamGenKeySelect(const C_CamGenKeySelect &);
   C_CamGenKeySelect & operator =(const C_CamGenKeySelect &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
