//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard table item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATABLEBASE_H
#define C_GISVDATABLEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaItTaView.h"
#include "C_GiSvDaRectBaseGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaTableBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaTableBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                     const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                     QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaTableBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;

   virtual void UpdateShowValue(void);
   virtual void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value);
   virtual void ConnectionActiveChanged(const bool oq_Active);
   virtual void EditModeActiveChanged(const bool oq_Active) override;
   virtual bool CallProperties(void) override;

   virtual void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                     const bool oq_Active) override;

   stw_types::sint32 GetLastValueUnscaled(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                          std::vector<stw_types::float64> & orc_Values);

   const stw_opensyde_gui_logic::C_PuiSvDbTable * GetTableItem(void) const;
   stw_types::sint32 SetTableItem(const stw_opensyde_gui_logic::C_PuiSvDbTable & orc_Content) const;
   bool GetViewActive(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   stw_types::uint32 GetViewIndex(void) const;
   bool CheckItemError(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                       QString & orc_Content, bool & orq_IsTransmissionError) const;

protected:
   virtual void m_UpdateErrorIcon(void) override;
   virtual bool m_AllowWarningIcon(void) const override;
   virtual QString m_GetCommonToolTipContent(void) const override;

private:
   stw_opensyde_gui::C_SyvDaItTaView * mpc_TableWidget;

   QAction * mpc_AddDataElement;
   QAction * mpc_AddSeperator;
   QAction * mpc_ConfigDataElement;
   QAction * mpc_RemoveDataElement;
   QAction * mpc_MiscSeperator;

   void m_AddNewDataElement(void);
   void m_RemoveDataElement(void);
   void m_RegisterDataElementRail(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   stw_types::sint32 m_MapDataElementIndexToInternalElementIndex(const stw_types::uint32 ou32_DataElementIndex,
                                                                 stw_types::uint32 & oru32_InternalElementIndex) const;

   //Avoid call
   C_GiSvDaTableBase(const C_GiSvDaTableBase &);
   C_GiSvDaTableBase & operator =(const C_GiSvDaTableBase &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
