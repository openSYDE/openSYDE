//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element in the system view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBNODEDATAPOOLLISTELEMENTID_H
#define C_PUISVDBNODEDATAPOOLLISTELEMENTID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbNodeDataPoolListElementId :
   public stw_opensyde_core::C_OSCNodeDataPoolListElementId
{
public:
   enum E_Type
   {
      eDATAPOOL_ELEMENT,
      eBUS_SIGNAL
   };

   C_PuiSvDbNodeDataPoolListElementId(void);
   C_PuiSvDbNodeDataPoolListElementId(const C_OSCNodeDataPoolListElementId & orc_Base, const E_Type oe_Type,
                                      const bool oq_UseArrayElementIndex,
                                      const stw_types::uint32 ou32_ArrayElementIndex, const bool oq_IsValid = true, const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder =
                                         stw_opensyde_core::C_OSCNodeDataPool::eDIAG,
                                      const QString & orc_InvalidNamePlaceholder = "");
   C_PuiSvDbNodeDataPoolListElementId(const stw_types::uint32 ou32_NodeIndex,
                                      const stw_types::uint32 ou32_DataPoolIndex,
                                      const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_ElementIndex,
                                      const E_Type oe_Type, const bool oq_UseArrayElementIndex,
                                      const stw_types::uint32 ou32_ArrayElementIndex, const bool oq_IsValid = true, const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder =
                                         stw_opensyde_core::C_OSCNodeDataPool::eDIAG,
                                      const QString & orc_InvalidNamePlaceholder = "");

   virtual bool operator < (const C_OSCNodeDataPoolId & orc_Cmp) const;  //lint !e1511  //Hiding of original operators
                                                                         // is intended
   virtual bool operator == (const C_OSCNodeDataPoolId & orc_Cmp) const; //lint !e1511  //Hiding of original operators
                                                                         // is intended

   bool CheckSameDataElement(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_OtherId) const;
   bool CheckSameDataElement(const C_PuiSvDbNodeDataPoolListElementId & orc_OtherId) const;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   void MarkInvalid(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                    const QString & orc_InvalidName);

   bool GetIsValid(void) const;
   QString GetInvalidNamePlaceholder(void) const;
   QString GetInvalidItemPlaceholderTooltip(void) const;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type GetInvalidTypePlaceholder(void) const;

   //Simple getter & setter
   E_Type GetType(void) const;
   void SetType(const E_Type oe_Type);
   QString GetHalChannelName(void) const;
   void SetHalChannelName(const QString & orc_Value);

   stw_types::uint32 GetArrayElementIndex(void) const;
   stw_types::uint32 GetArrayElementIndexOrZero(void) const;

   bool GetUseArrayElementIndex(void) const;

private:
   E_Type me_Type;
   bool mq_UseArrayElementIndex;
   stw_types::uint32 mu32_ArrayElementIndex;
   bool mq_IsValid; ///< Invalid flag,
   ///< should only be set by system definition
   ///< to system view synchronisation engine
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_InvalidTypePlaceholder; ///< Type used in case of invalid
   QString mc_InvalidNamePlaceholder;                                      ///< Name used in case of invalid
   QString mc_HalChannelName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
