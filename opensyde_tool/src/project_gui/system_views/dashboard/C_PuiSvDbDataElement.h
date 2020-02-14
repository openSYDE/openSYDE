//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for graphics items with system view dashboard data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENT_H
#define C_PUISVDBDATAELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiBsDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElement :
   public C_PuiBsDataElement
{
public:
   enum E_Type ///< Type of dashboard data element
   {
      eCHART,        ///< Data element for chart
      eLABEL,        ///< Data element for label
      ePIE_CHART,    ///< Data element for pie chart
      eSPIN_BOX,     ///< Data element for spin box
      eSLIDER,       ///< Data element for slider
      eTABLE,        ///< Data element for table
      eTOGGLE,       ///< Data element for toggle
      ePROGRESS_BAR, ///< Data element for progress bar
      eLINE_ARROW,   ///< Data element for line arrow
      eBOUNDARY,     ///< Data element for boundary
      eTEXT_ELEMENT, ///< Data element for text element
      eIMAGE,        ///< Data element for image element
      ePARAM,        ///< Data element for parametrization element
      eUNKNOWN       ///< Data element unknown
   };

   C_PuiSvDbDataElement(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                        const stw_types::sint32 & ors32_DataIndex, const E_Type & ore_Type);
   virtual ~C_PuiSvDbDataElement(void);

   virtual void SyncIndex(const E_Type & ore_Type, const stw_types::sint32 & ors32_Index, const E_Action & ore_Action);

   virtual void SetDashboardIndex(const stw_types::uint32 ou32_Value);
   E_Type GetWidgetType(void) const;

protected:
   const stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_DashboardIndex;
   const E_Type me_Type; ///< Type to know on which event to react

private:
   C_PuiSvDbDataElement(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
