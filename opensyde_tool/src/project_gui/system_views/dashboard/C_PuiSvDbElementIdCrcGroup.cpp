//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC

   Storage class to group element IDs with their CRC

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvDbElementIdCrcGroup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbElementIdCrcGroup::C_PuiSvDbElementIdCrcGroup() :
   C_SdNdeDalCopElementIdCrcGroupBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbElementIdCrcGroup::~C_PuiSvDbElementIdCrcGroup() noexcept
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get element id

   \return
   Element id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId & C_PuiSvDbElementIdCrcGroup::GetElementId() const
{
   return this->mc_ElementId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set element id

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbElementIdCrcGroup::SetElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_Value)
{
   this->mc_ElementId = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc CRC

   \param[out]  oru32_Result  Result

   \return
   STW error codes

   \retval   C_NO_ERR   Checksum calculated
   \retval   C_CONFIG   ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbElementIdCrcGroup::m_CalcCrc(uint32_t & oru32_Result) const
{
   int32_t s32_Retval = C_NO_ERR;

   oru32_Result = 0xFFFFFFFFU;

   if (this->mc_ElementId.GetIsValid())
   {
      const C_OscNodeDataPoolListElement * const pc_OscListElement =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_ElementId.u32_NodeIndex,
                                                                    this->mc_ElementId.u32_DataPoolIndex,
                                                                    this->mc_ElementId.u32_ListIndex,
                                                                    this->mc_ElementId.u32_ElementIndex);
      const C_PuiSdNodeDataPoolListElement * const pc_UiListElement =
         C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(this->mc_ElementId.u32_NodeIndex,
                                                                   this->mc_ElementId.u32_DataPoolIndex,
                                                                   this->mc_ElementId.u32_ListIndex,
                                                                   this->mc_ElementId.u32_ElementIndex);

      if ((pc_OscListElement != NULL) && (pc_UiListElement != NULL))
      {
         const uint32_t u32_Val = pc_OscListElement->GetArraySize();
         //Data element core
         if (this->mc_ElementId.GetUseArrayElementIndex())
         {
            pc_OscListElement->CalcHashElement(oru32_Result, this->mc_ElementId.GetArrayElementIndexOrZero());
         }
         else
         {
            pc_OscListElement->CalcHash(oru32_Result);
         }
         //Include array size
         stw::scl::C_SclChecksums::CalcCRC32(&u32_Val, sizeof(u32_Val), oru32_Result);
         //Data element UI
         pc_UiListElement->CalcHash(oru32_Result);
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Signal part
      if (this->mc_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL)
      {
         C_OscCanMessageIdentificationIndices c_MessageId;
         uint32_t u32_SignalIndex;
         C_PuiSdHandler::h_GetInstance()->ConvertElementIndexToSignalIndex(this->mc_ElementId, c_MessageId,
                                                                           u32_SignalIndex);
         {
            const C_OscCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId,
                                                                                                   u32_SignalIndex);

            if (pc_Signal != NULL)
            {
               pc_Signal->CalcHash(oru32_Result);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
      }
   }
   else
   {
      //No error because these can be copied
   }

   return s32_Retval;
}
