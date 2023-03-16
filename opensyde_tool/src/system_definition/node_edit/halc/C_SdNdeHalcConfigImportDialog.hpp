//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for import of a HALC configuration file
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGIMPORTDIALOG_H
#define C_SDNDEHALCCONFIGIMPORTDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OgePopUpDialog.hpp"

#include "C_OscHalcConfigStandalone.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcConfigImportDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcConfigImportDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const uint32_t ou32_NodeIndex, const QString & orc_ImportFileName);
   ~C_SdNdeHalcConfigImportDialog(void) noexcept override;

   void InitStaticNames(void) const;

   int32_t PrepareDialog(QString & orc_ErrorDetails);
   int32_t GetResult(QString & orc_ErrorDetails) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeHalcConfigImportDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const uint32_t mu32_NodeIndex;
   const QString mc_ImportFileName;
   int32_t ms32_Result;

   stw::opensyde_core::C_OscHalcConfigStandalone mc_ImportConfig;

   void m_OkClicked(void);
   void m_OnCancel(void);

   static bool mh_CheckConsistency(const stw::opensyde_core::C_OscHalcConfig * const opc_Config,
                                   const stw::opensyde_core::C_OscHalcConfigStandalone & orc_ConfigStandalone,
                                   QString & orc_ErrorDetails);
   static bool mh_CheckConsistencyEl(const stw::opensyde_core::C_OscHalcConfigParameter & orc_Ref,
                                     const stw::opensyde_core::C_OscHalcConfigParameter & orc_New,
                                     QString & orc_ErrorDetails, const QString & orc_Domain,
                                     const QString & orc_Channel, const QString & orc_CheckAddendum);

   //Avoid call
   C_SdNdeHalcConfigImportDialog(const C_SdNdeHalcConfigImportDialog &);
   C_SdNdeHalcConfigImportDialog & operator =(const C_SdNdeHalcConfigImportDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace
#endif
