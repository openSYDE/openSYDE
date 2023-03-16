//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for STW flashloader options (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDESTWFLASHLOADEROPTIONS_H
#define C_SDNDESTWFLASHLOADEROPTIONS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeStwFlashloaderOptions;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeStwFlashloaderOptions :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeStwFlashloaderOptions(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const uint32_t ou32_Index);
   ~C_SdNdeStwFlashloaderOptions(void) override;

   void InitStaticNames(void) const;

private:
   Ui::C_SdNdeStwFlashloaderOptions * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_Index;

   void m_LoadData(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_OnUseResetMessageChanged(void) const;

   //Avoid call
   C_SdNdeStwFlashloaderOptions(const C_SdNdeStwFlashloaderOptions &);
   C_SdNdeStwFlashloaderOptions & operator =(const C_SdNdeStwFlashloaderOptions &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
