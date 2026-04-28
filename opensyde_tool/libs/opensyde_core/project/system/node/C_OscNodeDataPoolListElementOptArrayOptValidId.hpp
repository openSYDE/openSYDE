//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       All indices to identify a data element stored in a node including an optional array and a valid flag
   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTELEMENTOPTARRAYOPTVALIDID_HPP
#define C_OSCNODEDATAPOOLLISTELEMENTOPTARRAYOPTVALIDID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeDataPoolListElementOptArrayId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolListElementOptArrayOptValidId :
   public C_OscNodeDataPoolListElementOptArrayId
{
public:
   C_OscNodeDataPoolListElementOptArrayOptValidId();
   C_OscNodeDataPoolListElementOptArrayOptValidId(const C_OscNodeDataPoolListElementId & orc_Base,
                                                  const bool oq_UseArrayElementIndex,
                                                  const uint32_t ou32_ArrayElementIndex, const bool oq_IsValid = true);
   C_OscNodeDataPoolListElementOptArrayOptValidId(const C_OscNodeDataPoolListElementOptArrayId & orc_Base,
                                                  const bool oq_IsValid = true);
   C_OscNodeDataPoolListElementOptArrayOptValidId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                  const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                  const bool oq_UseArrayElementIndex,
                                                  const uint32_t ou32_ArrayElementIndex, const bool oq_IsValid = true);

   virtual bool operator < (const C_OscNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OscNodeDataPoolId & orc_Cmp) const;

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   void MarkInvalid(void);

   bool GetIsValid(void) const;

private:
   bool mq_IsValid; ///< Invalid flag, should only be set to invalid or together with other values
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
