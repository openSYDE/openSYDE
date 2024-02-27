//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for advanced properties in SYDEflash
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLASETADVANCEDPROPERTIESWIDGET_H
#define C_FLASETADVANCEDPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaSetAdvancedPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaSetAdvancedPropertiesWidget :
   public opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_FlaSetAdvancedPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaSetAdvancedPropertiesWidget(void) override;

   void EnableSettings(const bool oq_Enabled);

   void SaveUserSettings(void) const;
   void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;

   int32_t GetFlashloaderResetWaitTime(void) const;
   int32_t GetRequestDownloadTimeout(void) const;
   int32_t GetTransferDataTimeout(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHide(void);

private:
   Ui::C_FlaSetAdvancedPropertiesWidget * mpc_Ui;

   //Avoid call
   C_FlaSetAdvancedPropertiesWidget(const C_FlaSetAdvancedPropertiesWidget &);
   C_FlaSetAdvancedPropertiesWidget & operator =(const C_FlaSetAdvancedPropertiesWidget &) &;

   void m_OnExpand(const bool oq_Expand) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
