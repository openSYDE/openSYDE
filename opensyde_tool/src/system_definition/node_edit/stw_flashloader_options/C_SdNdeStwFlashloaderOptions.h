//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for STW flashloader options (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDESTWFLASHLOADEROPTIONS_H
#define C_SDNDESTWFLASHLOADEROPTIONS_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeStwFlashloaderOptions;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeStwFlashloaderOptions :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeStwFlashloaderOptions(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const stw_types::uint32 ou32_Index);
   ~C_SdNdeStwFlashloaderOptions(void);

   void InitStaticNames(void) const;

private:
   Ui::C_SdNdeStwFlashloaderOptions * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_Index;

   void m_LoadData(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_OnUseResetMessageChanged(void) const;

   //Avoid call
   C_SdNdeStwFlashloaderOptions(const C_SdNdeStwFlashloaderOptions &);
   C_SdNdeStwFlashloaderOptions & operator =(const C_SdNdeStwFlashloaderOptions &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
