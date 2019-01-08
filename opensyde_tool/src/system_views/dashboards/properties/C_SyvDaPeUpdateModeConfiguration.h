//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODECONFIGURATION_H
#define C_SYVDAPEUPDATEMODECONFIGURATION_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SyvDaPeUpdateModeConfiguration;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SyvDaPeUpdateModeConfiguration :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeUpdateModeConfiguration(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const stw_types::uint32 ou32_ViewIndex);
   ~C_SyvDaPeUpdateModeConfiguration(void);

   void InitStaticNames(void) const;
   void Select(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaPeUpdateModeConfiguration(const C_SyvDaPeUpdateModeConfiguration &);
   C_SyvDaPeUpdateModeConfiguration & operator =(const C_SyvDaPeUpdateModeConfiguration &);

   Ui::C_SyvDaPeUpdateModeConfiguration * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ConfigureClicked(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
