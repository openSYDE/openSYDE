//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for graphics items with system view dashboard data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENT_HPP
#define C_PUISVDBDATAELEMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiBsDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElement :
   public C_PuiBsDataElement
{
public:
   enum E_Type ///< Type of dashboard data element
   {
      eCHART,        ///< Data element for chart (deprecated but kept for compatibility reasons)
      eTAB_CHART,    ///< Data element for chart tab
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

   C_PuiSvDbDataElement(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                        const int32_t & ors32_DataIndex, const E_Type & ore_Type);
   ~C_PuiSvDbDataElement(void) override;

   virtual void SyncIndex(const E_Type & ore_Type, const int32_t & ors32_Index, const E_Action & ore_Action);

   virtual void SetDashboardIndex(const uint32_t ou32_Value);
   E_Type GetWidgetType(void) const;

protected:
   const uint32_t mu32_ViewIndex;
   uint32_t mu32_DashboardIndex;
   const E_Type me_Type; ///< Type to know on which event to react

private:
   C_PuiSvDbDataElement(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
