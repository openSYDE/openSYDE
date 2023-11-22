//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard table item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATABLEBASE_HPP
#define C_GISVDATABLEBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaItTaView.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaTableBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaTableBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                     const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                     QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaTableBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;

   void UpdateShowValue(void) override;
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   void EditElementProperties(void);
   void EditModeActiveChanged(const bool oq_Active) override;
   bool EnableEditContent(void) override;
   void DisableEditContent(void) override;

   void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager, const bool oq_Active) override;

   int32_t GetLastValueUnscaled(const uint32_t ou32_WidgetDataPoolElementIndex, std::vector<float64_t> & orc_Values,
                                std::vector<QString> & orc_DisplayValues);

   const stw::opensyde_gui_logic::C_PuiSvDbTable * GetTableItem(void) const;
   int32_t SetTableItem(const stw::opensyde_gui_logic::C_PuiSvDbTable & orc_Content) const;
   bool GetViewActive(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   bool GetViewDashboardRouteValid(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   uint32_t GetViewIndex(void) const;
   bool CheckItemError(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                       QString & orc_Content, bool & orq_IsTransmissionError) const;

protected:
   void m_UpdateErrorIcon(void) override;
   bool m_AllowWarningIcon(void) const override;
   bool m_HasEditContentMode(void) const override;
   QString m_GetCommonToolTipContent(void) const override;

private:
   stw::opensyde_gui::C_SyvDaItTaView * mpc_TableWidget;

   QAction * mpc_AddDataElement;
   QAction * mpc_AddSeperator;
   QAction * mpc_ConfigDataElement;
   QAction * mpc_ConfigSeperator;
   QAction * mpc_RemoveDataElement;
   QAction * mpc_MoveUpDataElement;
   QAction * mpc_MoveDownDataElement;
   QAction * mpc_MiscSeperator;

   void m_AddNewDataElement(void);
   void m_MoveDataElementUp(void);
   void m_MoveDataElementDown(void);
   void m_RemoveDataElement(void);
   void m_RegisterDataElementRail(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   int32_t m_MapDataElementIndexToInternalElementIndex(const uint32_t ou32_DataElementIndex,
                                                       uint32_t & oru32_InternalElementIndex) const;

   //Avoid call
   C_GiSvDaTableBase(const C_GiSvDaTableBase &);
   C_GiSvDaTableBase & operator =(const C_GiSvDaTableBase &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
