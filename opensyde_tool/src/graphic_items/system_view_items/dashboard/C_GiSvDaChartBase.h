//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard chart item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDACHARTBASE_H
#define C_GISVDACHARTBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include <QAction>

#include "C_GiSvDaRectBaseGroup.h"
#include "C_SyvDaItChartWidget.h"
#include "C_PuiSvDbWidgetBase.h"

#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaChartBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaChartBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                     const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                     QGraphicsItem * const opc_Parent = NULL);

   virtual ~C_GiSvDaChartBase(void);

   virtual stw_types::sintn type(void) const override;

   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;

   virtual void UpdateShowValue(void) override;
   virtual void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex,
                                   const stw_types::sintn osn_Value) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual bool CallProperties(void) override;
   virtual void SetDrawingActive(const bool oq_Active) override;

   virtual void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                     const bool oq_Active) override;

protected:
   virtual void m_UpdateErrorIcon(void) override;
   virtual bool m_AllowWarningIcon(void) const override;
   virtual QString m_GetCommonToolTipContent(void) const override;

private:
   C_SyvDaItChartWidget * mpc_ChartWidget;

   static const stw_types::uint32 mhu32_MaximumDataElements;

   QAction * mpc_AddDataElement;
   QAction * mpc_AddSeperator;
   QAction * mpc_ConfigDataElement;
   QAction * mpc_RemoveDataElement;
   QAction * mpc_MiscSeperator;

   void m_AddNewDataElement(void);
   void m_RemoveDataElement(void);
   void m_RegisterDataElementRail(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   stw_types::sint32 m_SetChangedChartData(void);

   //Avoid call
   C_GiSvDaChartBase(const C_GiSvDaChartBase &);
   C_GiSvDaChartBase & operator =(const C_GiSvDaChartBase &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
