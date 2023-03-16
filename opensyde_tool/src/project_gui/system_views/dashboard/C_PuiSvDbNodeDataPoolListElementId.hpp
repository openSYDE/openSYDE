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
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbNodeDataPoolListElementId :
   public stw::opensyde_core::C_OscNodeDataPoolListElementId
{
public:
   enum E_Type
   {
      eDATAPOOL_ELEMENT,
      eBUS_SIGNAL
   };

   C_PuiSvDbNodeDataPoolListElementId(void);
   C_PuiSvDbNodeDataPoolListElementId(const C_OscNodeDataPoolListElementId & orc_Base, const E_Type oe_Type,
                                      const bool oq_UseArrayElementIndex, const uint32_t ou32_ArrayElementIndex,
                                      const bool oq_IsValid = true, const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder =
                                         stw::opensyde_core::C_OscNodeDataPool::eDIAG,
                                      const QString & orc_InvalidNamePlaceholder = "");
   C_PuiSvDbNodeDataPoolListElementId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                      const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                      const E_Type oe_Type, const bool oq_UseArrayElementIndex,
                                      const uint32_t ou32_ArrayElementIndex, const bool oq_IsValid = true, const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder =
                                         stw::opensyde_core::C_OscNodeDataPool::eDIAG,
                                      const QString & orc_InvalidNamePlaceholder = "");

   bool operator < (const C_OscNodeDataPoolId & orc_Cmp) const override;
   bool operator == (const C_OscNodeDataPoolId & orc_Cmp) const override;

   bool CheckSameDataElement(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_OtherId) const;
   bool CheckSameDataElement(const C_PuiSvDbNodeDataPoolListElementId & orc_OtherId) const;

   void CalcHash(uint32_t & oru32_HashValue) const override;
   void MarkInvalid(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                    const QString & orc_InvalidName);

   bool GetIsValid(void) const;
   QString GetInvalidNamePlaceholder(void) const;
   QString GetInvalidItemPlaceholderTooltip(void) const;
   stw::opensyde_core::C_OscNodeDataPool::E_Type GetInvalidTypePlaceholder(void) const;

   //Simple getter & setter
   E_Type GetType(void) const;
   void SetType(const E_Type oe_Type);
   QString GetHalChannelName(void) const;
   void SetHalChannelName(const QString & orc_Value);

   uint32_t GetArrayElementIndex(void) const;
   uint32_t GetArrayElementIndexOrZero(void) const;

   bool GetUseArrayElementIndex(void) const;

private:
   E_Type me_Type;
   bool mq_UseArrayElementIndex;
   uint32_t mu32_ArrayElementIndex;
   bool mq_IsValid; ///< Invalid flag,
   ///< should only be set by system definition
   ///< to system view synchronisation engine
   stw::opensyde_core::C_OscNodeDataPool::E_Type me_InvalidTypePlaceholder; ///< Type used in case of invalid
   QString mc_InvalidNamePlaceholder;                                       ///< Name used in case of invalid
   QString mc_HalChannelName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
