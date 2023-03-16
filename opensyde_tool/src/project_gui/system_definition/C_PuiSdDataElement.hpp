//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for graphics items with system definition data elements (header)

   Base class for graphics items with system definition data elements (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDDATAELEMENT_H
#define C_PUISDDATAELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_PuiBsDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdDataElement :
   public C_PuiBsDataElement
{
public:
   enum E_Type ///< Type of data element
   {
      eNODE,             ///< Data element for node
      eBUS,              ///< Data element for bus
      eLINE_ARROW,       ///< Data element for line arrow
      eBOUNDARY,         ///< Data element for boundary
      eTEXT_ELEMENT,     ///< Data element for text element
      eTEXT_ELEMENT_BUS, ///< Data element for bus text element
      eIMAGE,            ///< Data element for image element
      eBUS_CONNECTOR,    ///< Data element for bus connector element
      eUNKNOWN           ///< Data element unknown
   };

   C_PuiSdDataElement(const int32_t & ors32_Index, const E_Type & ore_Type);
   ~C_PuiSdDataElement() override;

   virtual void SyncIndex(const E_Type & ore_Type, const int32_t & ors32_Index, const E_Action & ore_Action);

protected:
   const E_Type me_Type; ///< Type to know on which event to react

private:
   C_PuiSdDataElement(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
