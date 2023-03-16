//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific parameter set image file as part of a node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMPARAMWIDGET_H
#define C_SYVUPPACLISTNODEITEMPARAMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacListNodeItemWidget.hpp"

#include "C_OscViewNodeUpdateParamInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemParamSetWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemParamSetWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                        const QString & orc_DeviceName, const bool oq_FileBased,
                                        const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   void SetParamInfo(const opensyde_core::C_OscViewNodeUpdateParamInfo & orc_ParamInfo);

   stw::opensyde_core::C_OscViewNodeUpdateParamInfo GetParamInfo(void) const;

   uint32_t GetType(void) const override;
   void ViewFileInfo(void) override;
   bool IsViewFileInfoPossible(void) const override;

protected:
   void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove) override;
   QString m_CreateToolTipTitle(void) const override;

private:
   stw::opensyde_core::C_OscViewNodeUpdateParamInfo mc_ParamsetInfo;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
