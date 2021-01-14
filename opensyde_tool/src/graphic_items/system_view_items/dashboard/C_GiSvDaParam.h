//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard parameterization table item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAPARAM_H
#define C_GISVDAPARAM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaItPaWidgetNew.h"
#include "C_GiSvDaRectBaseGroup.h"

#include "C_SyvComDriverDiag.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaParam :
   public C_GiSvDaRectBaseGroup
{
   Q_OBJECT

public:
   C_GiSvDaParam(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                 const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                 QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaParam(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
   virtual bool CallProperties(void) override;

   virtual void UpdateShowValue(void) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual void EditModeActiveChanged(const bool oq_Active) override;
   virtual void HandleManualOperationFinished(const stw_types::sint32 os32_Result,
                                              const stw_types::uint8 ou8_NRC) override;
   virtual void SetErrorForFailedCyclicElementIdRegistrations(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
      const std::vector<QString> & orc_FailedIdErrorDetails) override;

   void SetSyvComDriver(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver);

   stw_types::sint32 GetLastValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                  stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value);

   stw_types::uint32 GetViewIndex(void) const;
   const stw_opensyde_gui_logic::C_PuiSvDbParam * GetParamItem(void) const;
   stw_types::sint32 SetParamItem(const stw_opensyde_gui_logic::C_PuiSvDbParam & orc_Content) const;
   stw_types::sint32 AddParamItemDataElement(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                             const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Content);
   stw_types::sint32 DeleteLists(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);
   QWidget * GetPopUpParent(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNvmReadList(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id);

protected:
   virtual bool m_AllowWarningIcon(void) const override;
   virtual bool m_AllowRefreshButton(void) const override;
   virtual QString m_GetCommonToolTipContent(void) const override;

private:
   bool mq_Connected;
   bool mq_EditActive;
   C_SyvDaItPaWidgetNew * mpc_ParamWidget;

   stw_types::sint32 m_MapDataElementIndexToInternalElementIndex(const stw_types::uint32 ou32_DataElementIndex,
                                                                 stw_types::uint32 & oru32_InternalElementIndex) const;

   //Avoid call
   C_GiSvDaParam(const C_GiSvDaParam &);
   C_GiSvDaParam & operator =(const C_GiSvDaParam &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
