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
#include "C_SyvDaItPaWidgetNew.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"

#include "C_SyvComDriverDiag.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaParam :
   public C_GiSvDaRectBaseGroup
{
   Q_OBJECT

public:
   C_GiSvDaParam(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                 const int32_t & ors32_DataIndex, const uint64_t & oru64_Id, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaParam(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void UpdateShowValue(void) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   void EditModeActiveChanged(const bool oq_Active) override;
   bool EnableEditContent(void) override;
   void DisableEditContent(void) override;
   void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager, const bool oq_Active) override;

   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) override;

   void SetSyvComDriver(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver);

   int32_t GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                        stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);

   uint32_t GetViewIndex(void) const;
   const stw::opensyde_gui_logic::C_PuiSvDbParam * GetParamItem(void) const;
   int32_t SetParamItem(const stw::opensyde_gui_logic::C_PuiSvDbParam & orc_Content) const;
   int32_t AddParamItemDataElement(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                   const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Content);
   int32_t DeleteLists(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);
   QWidget * GetPopUpParent(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);

protected:
   bool m_AllowWarningIcon(void) const override;
   bool m_HasEditContentMode(void) const override;
   bool m_AllowRefreshButton(void) const override;
   QString m_GetCommonToolTipContent(void) const override;

private:
   bool mq_Connected;
   C_SyvDaItPaWidgetNew * mpc_ParamWidget;

   QAction * mpc_AddDataElement;

   int32_t m_MapDataElementIndexToInternalElementIndex(const uint32_t ou32_DataElementIndex,
                                                       uint32_t & oru32_InternalElementIndex) const;

   //Avoid call
   C_GiSvDaParam(const C_GiSvDaParam &);
   C_GiSvDaParam & operator =(const C_GiSvDaParam &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
