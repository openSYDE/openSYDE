//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data block properties (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDBADD_H
#define C_SDNDEDBADD_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCNodeApplication.h"

namespace Ui
{
class C_SdNdeDbAdd;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDbAdd :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbAdd(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                         const stw_types::uint32 ou32_NodeIndex);
   ~C_SdNdeDbAdd(void);

   void InitStaticNames(void) const;
   bool GetFromTSP(void) const;
   stw_opensyde_core::C_OSCNodeApplication::E_Type GetApplicationType(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDbAdd * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw_types::uint32 mu32_NodeIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   //Avoid call
   C_SdNdeDbAdd(const C_SdNdeDbAdd &);
   C_SdNdeDbAdd & operator =(const C_SdNdeDbAdd &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
