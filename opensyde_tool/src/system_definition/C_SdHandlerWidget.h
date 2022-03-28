//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition user

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDHANDLERWIDGET_H
#define C_SDHANDLERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_NagUseCaseWidget.h"

#include "C_SdTopologyWidget.h"
#include "C_SdNdeNodeEditWidget.h"
#include "C_SdBueBusEditWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdHandlerWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdHandlerWidget :
   public C_NagUseCaseWidget
{
   Q_OBJECT

public:
   explicit C_SdHandlerWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdHandlerWidget();

   virtual void SetParentHook(QWidget * const opc_Parent) override;
   virtual void UserInputFunc(const stw_types::uint32 ou32_FuncNumber) override;
   virtual void Save(void) override;
   virtual void OpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                           const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                           const stw_types::sint32 os32_Flag) override;
   virtual bool PrepareToClose(void) override;

   virtual void SetSubMode(const stw_types::sint32 os32_SubMode, const stw_types::uint32 ou32_Index,
                           const stw_types::uint32 ou32_Flag) override;
   virtual bool GlobalUserKeyPress(QKeyEvent * const opc_Event) override;
   virtual void CallHelp(void) override;

private:
   //Avoid call
   C_SdHandlerWidget(const C_SdHandlerWidget &);
   C_SdHandlerWidget & operator =(const C_SdHandlerWidget &); //lint !e1511 //we want to hide the base

   void m_DataChanged(void);
   void m_NodeChanged(const stw_types::uint32 ou32_Index);
   void m_BusChanged(const stw_types::uint32 ou32_Index);

   void m_SwitchToBus(const stw_types::uint32 ou32_Index, const QString & orc_BusName);
   void m_SwitchToBusProperties(const stw_types::uint32 ou32_Index, const QString & orc_BusName);
   void m_SetFlag(const stw_types::uint32 ou32_Flag) const;
   void m_ErrorChange(void);
   void m_GenerateCode(void) const;
   void m_Export(void);
   void m_Import(void) const;
   void m_RtfExport(void);
   void m_TspImport(void);
   void m_GenerateHalcDatapools(void) const;

   Ui::C_SdHandlerWidget * mpc_Ui;
   C_SdTopologyWidget * mpc_Topology;

   C_SdNdeNodeEditWidget * mpc_ActNodeEdit;
   C_SdBueBusEditWidget * mpc_ActBusEdit;

   stw_types::sintn msn_NodeEditTabIndex;
   stw_types::sintn msn_BusEditTabIndex;
   stw_types::sint32 ms32_SubMode;
   stw_types::uint32 mu32_Index;
   bool mq_DataChanged;

   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_APPLY;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_GENERATE_CODE;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_IMPORT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_EXPORT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_RTF_EXPORT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_TSP_IMPORT;

   const QString mc_TooltipGenerateCodeHeading;
   const QString mc_TooltipGenerateCodeContentSysdef;
   const QString mc_TooltipGenerateCodeContentNode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
