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
#include "C_SyvUpPacListNodeItemWidget.h"

#include "C_PuiSvNodeUpdateParamInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemParamSetWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemParamSetWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex,
                                        const QString & orc_DeviceName, const bool oq_FileBased,
                                        const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   void SetParamInfo(const stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & orc_ParamInfo);

   stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo GetParamInfo(void) const;

   virtual stw_types::uint32 GetType(void) const override;
   virtual void ViewFileInfo(void) override;
   virtual bool IsViewFileInfoPossible(void) const override;

protected:
   virtual void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                      bool & orq_TriggerRemove) override;
   virtual QString m_CreateToolTipTitle(void) const override;

private:
   stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo mc_ParamsetInfo;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
