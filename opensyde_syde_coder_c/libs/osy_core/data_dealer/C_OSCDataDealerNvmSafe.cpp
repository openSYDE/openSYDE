//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulates safety relevant sequences for NVM access. (implementation)

   As the "NVM" classes inherit from the data dealer we can only handle the safe data of one node;
   as a result the application will need to handle parameters for multiple nodes
   (e.g. multiple files with parameter set values; separate sequences for each node)

   Example for editing individual values:
   - read values from all nodes
   - write changes values to all nodes
   - read back values from all nodes
   - list the read back values from all nodes for the user to confirm
   - update the CRCs of all nodes

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCParamSetHandler.h"
#include "C_OSCParamSetRawNodeFiler.h"
#include "C_OSCParamSetInterpretedNodeFiler.h"
#include "C_OSCDataDealerNvmSafe.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCDataDealerNvmSafe::C_OSCDataDealerNvmSafe(void) :
   C_OSCDataDealerNvm(),
   me_CreateParameterSetWorkflowState(C_OSCDataDealerNvmSafe::eCPSFS_IDLE),
   me_ParameterSetFileState(C_OSCDataDealerNvmSafe::ePSFS_IDLE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements

   \param[in]     opc_Node          Pointer to node of data dealer
   \param[in]     ou32_NodeIndex    Index of node of data dealer
   \param[in]     opc_DiagProtocol  Pointer to used diagnostic protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCDataDealerNvmSafe::C_OSCDataDealerNvmSafe(C_OSCNode * const opc_Node, const uint32 ou32_NodeIndex,
                                               C_OSCDiagProtocolBase * const opc_DiagProtocol) :
   C_OSCDataDealerNvm(opc_Node, ou32_NodeIndex, opc_DiagProtocol),
   me_CreateParameterSetWorkflowState(C_OSCDataDealerNvmSafe::eCPSFS_IDLE),
   me_ParameterSetFileState(C_OSCDataDealerNvmSafe::ePSFS_IDLE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCDataDealerNvmSafe::~C_OSCDataDealerNvmSafe(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking all CRCs of "NVM" datapool lists of one node

   The function calculates and checks the CRCs over all datapool lists with the flag q_NvMCRCActive is set to true
   of all datapools of type "NVM".

   \param[in]  orc_Node    Node with datapools for checking the CRCs

   \return
   C_NO_ERR    All list checksums are valid
   C_CHECKSUM  At least one checksum of a list is invalid
   C_RANGE     At least one datapool has the flag q_IsSafety set to true and
               at least one of its list has the flag q_NvMCRCActive set to false
   C_CONFIG    No diagnostic protocol are known (was this class properly Initialize()d ?)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeCheckCrcs(const C_OSCNode & orc_Node) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_DiagProtocol == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint32 u32_DataPoolCounter;

      for (u32_DataPoolCounter = 0U; u32_DataPoolCounter < orc_Node.c_DataPools.size(); ++u32_DataPoolCounter)
      {
         if ((orc_Node.c_DataPools[u32_DataPoolCounter].e_Type == C_OSCNodeDataPool::eNVM) ||
             (orc_Node.c_DataPools[u32_DataPoolCounter].e_Type == C_OSCNodeDataPool::eHALC_NVM))
         {
            uint32 u32_ListCounter;

            for (u32_ListCounter = 0U;
                 u32_ListCounter < orc_Node.c_DataPools[u32_DataPoolCounter].c_Lists.size();
                 ++u32_ListCounter)
            {
               const C_OSCNodeDataPoolList & rc_List =
                  orc_Node.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter];
               if (rc_List.q_NvMCRCActive == true)
               {
                  const uint16 u16_CalcCrc = this->NvmCalcCrc(rc_List);

                  if (rc_List.u32_NvMCRC != static_cast<uint32>(u16_CalcCrc))
                  {
                     // Checksum of list is invalid
                     s32_Return = C_CHECKSUM;
                  }
               }
               else if (orc_Node.c_DataPools[u32_DataPoolCounter].q_IsSafety == true)
               {
                  // A list in a safety datapool shall have a CRC
                  s32_Return = C_RANGE;
               }
               else
               {
                  // Nothing to do
               }

               if (s32_Return != C_NO_ERR)
               {
                  break;
               }
            }

            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Writing of changed NVM values

   The function writes all values of all datapool elements of all lists of all datapools of
   the original node of the "C_OSCDataDealer" marked as "changed" to the ECU NVM without updating the CRC(s).
   Only lists in datapools of type "NVM" will be considered.

   \param[out]  orc_ChangedElements         Container with all detected changed elements.
                                            The container will not be cleared.
   \param[in]   opc_AdditionalListsToUpdate Container with additional lists to update via this process
                                            even in the case they don't have any changed elements
                                            The specified node indices will be ignored.
   \param[out]    opu8_NrCode               if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    Writing successful
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialized ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_OVERFLOW  At least one value lies outside of the defined minimum and maximum range. (checked by client side)
   C_BUSY      No changed value found and no additional lists specified (checked by client side)
   C_TIMEOUT   Expected server response not received within timeout
   C_NOACT     Could not send request (e.g. Tx buffer full)
   C_WARN      server sent error response
   C_RD_WR     unexpected content in server response
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeWriteChangedValues(
   std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedElements,
   const std::vector<C_OSCNodeDataPoolListId> * const opc_AdditionalListsToUpdate, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   // Reset the container for changed lists. Will be used by NvmSafeReadValues.
   this->mc_ChangedLists.clear();

   if ((this->mpc_Node == NULL) || (this->mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //Check the additional parameter first to not write anything if this one does contain invalid indices
      if ((opc_AdditionalListsToUpdate != NULL) && (opc_AdditionalListsToUpdate->size() > 0UL))
      {
         s32_Return = C_NO_ERR;
         for (uint32 u32_ItAdditionalIndex = 0;
              (u32_ItAdditionalIndex < opc_AdditionalListsToUpdate->size()) && (s32_Return == C_NO_ERR);
              ++u32_ItAdditionalIndex)
         {
            const C_OSCNodeDataPoolListId & rc_CurAdditionalIndex =
               (*opc_AdditionalListsToUpdate)[u32_ItAdditionalIndex];
            if (rc_CurAdditionalIndex.u32_DataPoolIndex < this->mpc_Node->c_DataPools.size())
            {
               const C_OSCNodeDataPool & rc_CurDataPool =
                  this->mpc_Node->c_DataPools[rc_CurAdditionalIndex.u32_DataPoolIndex];
               if (rc_CurAdditionalIndex.u32_ListIndex < rc_CurDataPool.c_Lists.size())
               {
                  //No problem found
                  this->mc_ChangedLists.insert(rc_CurAdditionalIndex);
               }
               else
               {
                  s32_Return = C_CONFIG;
               }
            }
            else
            {
               s32_Return = C_CONFIG;
            }
         }
      }
      else
      {
         s32_Return = C_BUSY;
      }

      if (s32_Return != C_CONFIG)
      {
         uint32 u32_DataPoolCounter;
         C_OSCNodeDataPoolListElementId c_ElementId;

         c_ElementId.u32_NodeIndex = this->mu32_NodeIndex;

         for (u32_DataPoolCounter = 0U; u32_DataPoolCounter < this->mpc_Node->c_DataPools.size(); ++u32_DataPoolCounter)
         {
            uint32 u32_ListCounter;
            uint32 u32_Elementcounter;
            uint16 u16_AccessCount = 0U;

            c_ElementId.u32_DataPoolIndex = u32_DataPoolCounter;

            C_OSCNodeDataPool * const pc_DataPool = &this->mpc_Node->c_DataPools[u32_DataPoolCounter];
            if ((pc_DataPool->e_Type == C_OSCNodeDataPool::eNVM) ||
                (pc_DataPool->e_Type == C_OSCNodeDataPool::eHALC_NVM))
            {
               // Count the changed elements
               for (u32_ListCounter = 0U; u32_ListCounter < pc_DataPool->c_Lists.size(); ++u32_ListCounter)
               {
                  for (u32_Elementcounter = 0U;
                       u32_Elementcounter < pc_DataPool->c_Lists[u32_ListCounter].c_Elements.size();
                       ++u32_Elementcounter)
                  {
                     if (pc_DataPool->c_Lists[u32_ListCounter].c_Elements[u32_Elementcounter].q_NvMValueChanged == true)
                     {
                        // Element was changed and must be written.
                        ++u16_AccessCount;
                     }
                  }
               }

               if (u16_AccessCount > 0U)
               {
                  // Prepare the transaction for this datapool
                  s32_Return = this->mpc_DiagProtocol->NvmWriteStartTransaction(static_cast<uint8>(u32_DataPoolCounter),
                                                                                u16_AccessCount);
                  if (s32_Return == C_NO_ERR)
                  {
                     // Write the concrete elements
                     for (u32_ListCounter = 0U; u32_ListCounter < pc_DataPool->c_Lists.size(); ++u32_ListCounter)
                     {
                        C_OSCNodeDataPoolList * const pc_List = &pc_DataPool->c_Lists[u32_ListCounter];
                        C_OSCNodeDataPoolListId c_ListId;

                        c_ElementId.u32_ListIndex = u32_ListCounter;
                        c_ListId = c_ElementId;

                        for (u32_Elementcounter = 0U;
                             u32_Elementcounter < pc_List->c_Elements.size();
                             ++u32_Elementcounter)
                        {
                           C_OSCNodeDataPoolListElement * const pc_Element = &pc_List->c_Elements[u32_Elementcounter];

                           // Check range if value was changed
                           if (pc_Element->q_NvMValueChanged == true)
                           {
                              if (pc_Element->CheckNvmValueRange() == C_NO_ERR)
                              {
                                 std::vector<uint8> c_ElementData;

                                 //convert to native endianness depending on the type ...
                                 //no possible problem we did not check for already ...
                                 if (this->mpc_DiagProtocol->GetEndianness() ==
                                     C_OSCDiagProtocolBase::mhu8_ENDIANNESS_BIG)
                                 {
                                    pc_Element->c_NvmValue.GetValueAsBigEndianBlob(c_ElementData);
                                 }
                                 else
                                 {
                                    pc_Element->c_NvmValue.GetValueAsLittleEndianBlob(c_ElementData);
                                 }

                                 // Element value was changed and is valid
                                 s32_Return =
                                    this->mpc_DiagProtocol->NvmWrite(pc_Element->u32_NvMStartAddress, c_ElementData,
                                                                     opu8_NrCode);
                                 // Adapt errorcode
                                 s32_Return = this->m_AdaptProtocolReturnValue(s32_Return);

                                 if (s32_Return == C_NO_ERR)
                                 {
                                    // Element written
                                    // Return the entire element id
                                    c_ElementId.u32_ElementIndex = u32_Elementcounter;
                                    orc_ChangedElements.push_back(c_ElementId);

                                    // Reset the flag
                                    pc_Element->q_NvMValueChanged = false;

                                    // Save the list index as changed list. Will be used by NvmSafeReadValues.
                                    this->mc_ChangedLists.insert(c_ListId);
                                 }
                              }
                              else
                              {
                                 s32_Return = C_OVERFLOW;
                              }
                           }

                           if (s32_Return != C_NO_ERR)
                           {
                              // Service failed. Abort writing.
                              break;
                           }
                        }

                        if (s32_Return != C_NO_ERR)
                        {
                           // Service failed. Abort writing.
                           break;
                        }
                     }
                  }
                  else
                  {
                     // Start service failed. Adapt errorcode
                     s32_Return = this->m_AdaptProtocolReturnValue(s32_Return);
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     // All elements of datapool written. Finish this transaction.
                     s32_Return = this->mpc_DiagProtocol->NvmWriteFinalizeTransaction();
                     // Adapt return value
                     s32_Return = this->m_AdaptProtocolReturnValue(s32_Return);
                  }
               }

               //Stop if service failure, continue with C_BUSY (don't stop if no elements found in first datapool)
               if ((s32_Return != C_NO_ERR) && (s32_Return != C_BUSY))
               {
                  // Service failed. Abort writing.
                  break;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads values of all changed lists of ECU

   The function reads the values of all datapool elements of all datapool lists that
   contain datapool elements that were written by the preceding call to
   "NvmSafeWriteChangedValues". The function stores the read values in the copy of
   the original "C_OSCNode" instance of "C_OSCDataDealer".
   Read values are marked by setting the "Valid" flag to true.

   \param[out] orpc_NodeCopy Parameter to access the read values
   \param[out] opu8_NrCode   if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    Values read successful
   C_CONFIG    No node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_OVERFLOW  At least one list has no elements.
   C_RD_WR     No list contains datapool elements that were
               written by the preceding call to "NvmSafeWriteChangedValues"
   C_RANGE     At least one index of a datapool or a list of changed lists is invalid or
               datapool element size configuration does not match with count of read bytes
   C_TIMEOUT   Expected server response not received within timeout
   C_NOACT     Could not send request (e.g. Tx buffer full)
   C_WARN      Server sent error response
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeReadValues(const C_OSCNode * (&orpc_NodeCopy), uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   if ((this->mpc_Node == NULL) || (this->mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = C_RD_WR;

      // Copy the original instance
      this->mc_NodeCopy = *this->mpc_Node;

      if (this->mc_ChangedLists.size() > 0)
      {
         std::set<C_OSCNodeDataPoolListId>::const_iterator c_ItChangedList;

         // Read all changed lists
         for (c_ItChangedList = this->mc_ChangedLists.begin();
              c_ItChangedList != this->mc_ChangedLists.end();
              ++c_ItChangedList)
         {
            if (((*c_ItChangedList).u32_DataPoolIndex < this->mc_NodeCopy.c_DataPools.size()) &&
                ((*c_ItChangedList).u32_ListIndex <
                 this->mc_NodeCopy.c_DataPools[(*c_ItChangedList).u32_DataPoolIndex].c_Lists.size()))
            {
               C_OSCNodeDataPoolList & rc_List =
                  this->mc_NodeCopy.c_DataPools[(*c_ItChangedList).u32_DataPoolIndex].c_Lists[(*c_ItChangedList).
                                                                                              u32_ListIndex];
               std::vector<uint8> c_Values;
               uint32 u32_ElementCounter;

               // Reset of all valid flags
               for (u32_ElementCounter = 0U; u32_ElementCounter < rc_List.c_Elements.size(); ++u32_ElementCounter)
               {
                  rc_List.c_Elements[u32_ElementCounter].q_NvmValueIsValid = false;
               }

               s32_Return = this->m_NvmReadListRaw(rc_List, c_Values, opu8_NrCode);

               if (s32_Return == C_NO_ERR)
               {
                  // Store the read values into the copy without checking and updating the CRC.
                  // The valid flag will be set to true if the element was read
                  s32_Return = this->m_SaveDumpValuesToListValues(c_Values, rc_List);

                  if (s32_Return == C_RD_WR)
                  {
                     // Remap the error
                     s32_Return = C_RANGE;
                  }
               }
            }
            else
            {
               s32_Return = C_RANGE;
            }

            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      orpc_NodeCopy = &this->mc_NodeCopy;
   }
   else
   {
      orpc_NodeCopy = NULL;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the CRCs of changed lists and writes them to the ECU

   The function calculates the CRCs over all datapool lists read by the preceding call to
   "NvmSafeReadValues" and writes them to the ECU NVM.

   \param[out]   opu8_NrCode   if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    CRCs written successfully
   C_CONFIG    No node or diagnostic protocol are known (was this class properly Initialized ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_CHECKSUM  At least one datapool of type "NVM" has the flag q_IsSafety set to true and
               at least one of its lists has the flag q_NvMCRCActive set to false.
   C_RANGE     The size of at least one list of NVM datapools of the copy of C_OSCNode differs to the original instance.
               The count of lists of NVM datapools of the copy of C_OSCNode differs to the original instance.
               The count of the datapools of the copy of C_OSCNode differs to the original instance.
   C_BUSY      No list contains datapool elements that were
               written by the preceding call to "NvmSafeWriteChangedValues"
   C_OVERFLOW  At least one changed list has no elements.
   C_DEFAULT   At least one element of the changed lists has the flag q_IsValid set to false.
   C_TIMEOUT   Expected server response not received within timeout
   C_NOACT     Could not send request (e.g. Tx buffer full)
   C_WARN      Server sent error response
   C_RD_WR     unexpected content in server response
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeWriteCrcs(uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   if ((this->mpc_Node == NULL) || (this->mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint32 u32_DataPoolCounter;
      uint32 u32_ListCounter;
      std::set<C_OSCNodeDataPoolListId>::const_iterator c_ItChangedList;

      s32_Return = C_NO_ERR;

      // Check all common preconditions
      if (this->mc_NodeCopy.c_DataPools.size() == this->mpc_Node->c_DataPools.size())
      {
         // Check the datapool configuartion
         for (u32_DataPoolCounter = 0U;
              u32_DataPoolCounter < this->mc_NodeCopy.c_DataPools.size();
              ++u32_DataPoolCounter)
         {
            if (this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists.size() ==
                this->mpc_Node->c_DataPools[u32_DataPoolCounter].c_Lists.size())
            {
               // Check the list configuration
               for (u32_ListCounter = 0U;
                    u32_ListCounter < this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists.size();
                    ++u32_ListCounter)
               {
                  if (this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].u32_NvMSize !=
                      this->mpc_Node->c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].u32_NvMSize)
                  {
                     // NVM size of list different
                     s32_Return = C_RANGE;
                     break;
                  }
               }
            }
            else
            {
               s32_Return = C_RANGE;
            }

            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }

      // Check pre conditions of changed lists
      if (s32_Return == C_NO_ERR)
      {
         // Read all changed lists
         for (c_ItChangedList = this->mc_ChangedLists.begin();
              c_ItChangedList != this->mc_ChangedLists.end();
              ++c_ItChangedList)
         {
            u32_DataPoolCounter = (*c_ItChangedList).u32_DataPoolIndex;
            u32_ListCounter = (*c_ItChangedList).u32_ListIndex;

            if ((u32_DataPoolCounter < this->mc_NodeCopy.c_DataPools.size()) &&
                (u32_ListCounter < this->mc_NodeCopy.c_DataPools[(*c_ItChangedList).u32_DataPoolIndex].c_Lists.size()))
            {
               if (this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].c_Elements.size() == 0)
               {
                  // No elements in list
                  s32_Return = C_OVERFLOW;
               }
               else if ((this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].q_IsSafety == true) &&
                        (this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].q_NvMCRCActive
                         ==
                         false))
               {
                  // Checksum is deactivated in a safety datapool
                  s32_Return = C_CHECKSUM;
               }
               else
               {
                  uint32 u32_ElementCounter;

                  for (u32_ElementCounter = 0U;
                       u32_ElementCounter <
                       this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].c_Elements.size();
                       ++u32_ElementCounter)
                  {
                     if (this->mc_NodeCopy.c_DataPools[u32_DataPoolCounter].c_Lists[u32_ListCounter].c_Elements[
                            u32_ElementCounter].q_NvmValueIsValid == false)
                     {
                        // Element was not read from ECU
                        s32_Return = C_BUSY;
                        break;
                     }
                  }
               }
            }
            else
            {
               s32_Return = C_RANGE;
            }

            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }

      // Writing of CRC to ECU
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_BUSY;

         for (c_ItChangedList = this->mc_ChangedLists.begin();
              c_ItChangedList != this->mc_ChangedLists.end();
              ++c_ItChangedList)
         {
            C_OSCNodeDataPoolList & rc_List =
               this->mc_NodeCopy.c_DataPools[(*c_ItChangedList).u32_DataPoolIndex].c_Lists[(*c_ItChangedList).
                                                                                           u32_ListIndex];

            if (rc_List.q_NvMCRCActive == true)
            {
               std::vector<uint8> c_CrcData;

               // Calc the CRC
               rc_List.u32_NvMCRC = this->NvmCalcCrc(rc_List);

               if (this->mpc_DiagProtocol->GetEndianness() == C_OSCDiagProtocolBase::mhu8_ENDIANNESS_BIG)
               {
                  rc_List.GetCRCAsBigEndianBlob(c_CrcData);
               }
               else
               {
                  rc_List.GetCRCAsLittleEndianBlob(c_CrcData);
               }

               // TODO: KEFEX position of CRC can be different
               s32_Return = this->mpc_DiagProtocol->NvmWrite(rc_List.u32_NvMStartAddress, c_CrcData, opu8_NrCode);

               if (s32_Return != C_NO_ERR)
               {
                  // Adapt errorcode
                  s32_Return = this->m_AdaptProtocolReturnValue(s32_Return);
                  break;
               }
            }
            else
            {
               // No CRC active
               s32_Return = C_NO_ERR;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCDataDealerNvmSafe::NvmSafeClearInternalContent(void)
{
   //Update state
   this->me_ParameterSetFileState = C_OSCDataDealerNvmSafe::ePSFS_DATA_RESET;
   this->mc_ImageFileHandler.ClearContent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create parameter set file based on current ECU values

   Note: Not set CRCs are supported

   \param[in]  orc_ListIds List IDs
   \param[out] opu8_NrCode if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   Data prepared for file
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Data pool list IDs invalid
   C_CHECKSUM CRC over the values of a parameter list read from the ECU does not match those values
   C_CONFIG   No valid diagnostic protocol is set
               or no valid pointer to the original instance of "C_OSCNode" is set in "C_OSCDataDealer"
   C_NOACT    Server communication protocol service could not be requested
   C_TIMEOUT  Server communication protocol service has timed out
   C_WARN     Server communication protocol service error response was received
   C_COM      expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeReadParameterValues(const std::vector<C_OSCNodeDataPoolListId> & orc_ListIds,
                                                          uint8 * const opu8_NrCode)
{
   sint32 s32_Retval = C_NO_ERR;

   //Reset state
   this->me_CreateParameterSetWorkflowState = C_OSCDataDealerNvmSafe::eCPSFS_IDLE;
   if (C_OSCDataDealerNvmSafe::me_ParameterSetFileState == C_OSCDataDealerNvmSafe::ePSFS_DATA_RESET)
   {
      if (orc_ListIds.size() > 0)
      {
         if ((this->mpc_Node != NULL) && (this->mpc_DiagProtocol != NULL))
         {
            C_OSCParamSetRawNode c_RawNode;
            c_RawNode.c_Entries.reserve(orc_ListIds.size() * 2);
            //Add invalid CRC entries
            for (uint32 u32_ItList = 0; (u32_ItList < orc_ListIds.size()) && (s32_Retval == C_NO_ERR); ++u32_ItList)
            {
               const C_OSCNodeDataPoolListId & rc_DataPoolListId = orc_ListIds[u32_ItList];
               if ((rc_DataPoolListId.u32_NodeIndex == this->mu32_NodeIndex) &&
                   (rc_DataPoolListId.u32_DataPoolIndex < this->mpc_Node->c_DataPools.size()))
               {
                  C_OSCNodeDataPool & rc_DataPool =
                     this->mpc_Node->c_DataPools[rc_DataPoolListId.u32_DataPoolIndex];
                  if (rc_DataPoolListId.u32_ListIndex < rc_DataPool.c_Lists.size())
                  {
                     C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[rc_DataPoolListId.u32_ListIndex];
                     if (rc_List.q_NvMCRCActive == true)
                     {
                        C_OSCParamSetRawEntry c_RawEntry;
                        c_RawEntry.u32_StartAddress = rc_List.u32_NvMStartAddress;
                        c_RawEntry.c_Bytes.clear();
                        c_RawEntry.c_Bytes.reserve(2);
                        c_RawEntry.c_Bytes.push_back(0);
                        c_RawEntry.c_Bytes.push_back(0);
                        c_RawNode.c_Entries.push_back(c_RawEntry);
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            if (s32_Retval == C_NO_ERR)
            {
               //Read list values
               for (uint32 u32_ItList = 0; (u32_ItList < orc_ListIds.size()) && (s32_Retval == C_NO_ERR); ++u32_ItList)
               {
                  const C_OSCNodeDataPoolListId & rc_DataPoolListId = orc_ListIds[u32_ItList];
                  if ((rc_DataPoolListId.u32_NodeIndex == this->mu32_NodeIndex) &&
                      (rc_DataPoolListId.u32_DataPoolIndex < this->mpc_Node->c_DataPools.size()))
                  {
                     C_OSCNodeDataPool & rc_DataPool =
                        this->mpc_Node->c_DataPools[rc_DataPoolListId.u32_DataPoolIndex];
                     if (rc_DataPoolListId.u32_ListIndex < rc_DataPool.c_Lists.size())
                     {
                        C_OSCParamSetRawEntry c_RawEntry;
                        C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[rc_DataPoolListId.u32_ListIndex];
                        s32_Retval = m_CreateRawEntryAndPrepareInterpretedData(rc_List, c_RawEntry, opu8_NrCode);
                        c_RawNode.c_Entries.push_back(c_RawEntry);
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  std::vector<uint32> c_AlreadyUsedDataPoolIndices;
                  C_OSCParamSetInterpretedNode c_InterpretedNode;
                  //Prepare data
                  //Node
                  c_RawNode.c_Name = this->mpc_Node->c_Properties.c_Name;
                  c_InterpretedNode.c_Name = this->mpc_Node->c_Properties.c_Name;
                  //Data pools
                  for (uint32 u32_ItCurListId = 0; (u32_ItCurListId < orc_ListIds.size()) && (s32_Retval == C_NO_ERR);
                       ++u32_ItCurListId)
                  {
                     const C_OSCNodeDataPoolListId & rc_DataPoolListId = orc_ListIds[u32_ItCurListId];
                     if (rc_DataPoolListId.u32_DataPoolIndex < this->mpc_Node->c_DataPools.size())
                     {
                        C_OSCNodeDataPool & rc_DataPool =
                           this->mpc_Node->c_DataPools[rc_DataPoolListId.u32_DataPoolIndex];
                        if (rc_DataPoolListId.u32_ListIndex < rc_DataPool.c_Lists.size())
                        {
                           C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[rc_DataPoolListId.u32_ListIndex];
                           bool q_DataPoolFound = false;
                           //Case 1: Data pool exists
                           //------------------------
                           tgl_assert(c_AlreadyUsedDataPoolIndices.size() == c_InterpretedNode.c_DataPools.size());
                           for (uint32 u32_ItCurrentDataPool = 0;
                                (u32_ItCurrentDataPool < c_InterpretedNode.c_DataPools.size()) &&
                                (s32_Retval == C_NO_ERR);
                                ++u32_ItCurrentDataPool)
                           {
                              if (c_AlreadyUsedDataPoolIndices[u32_ItCurrentDataPool] ==
                                  rc_DataPoolListId.u32_DataPoolIndex)
                              {
                                 bool q_ListFound = false;
                                 q_DataPoolFound = true;
                                 //Interpreted
                                 for (uint32 u32_ItListId = 0; u32_ItListId < orc_ListIds.size(); ++u32_ItListId)
                                 {
                                    const C_OSCNodeDataPoolListId & rc_ListId = orc_ListIds[u32_ItListId];
                                    //If not same index but otherwise same ID abort
                                    if ((u32_ItCurListId != u32_ItListId) &&
                                        ((rc_ListId.u32_DataPoolIndex == rc_DataPoolListId.u32_DataPoolIndex) &&
                                         (rc_ListId.u32_ListIndex ==
                                          rc_DataPoolListId.u32_ListIndex)))
                                    {
                                       q_ListFound = true;
                                    }
                                 }
                                 if (q_ListFound == false)
                                 {
                                    //List
                                    C_OSCParamSetInterpretedDataPool & rc_InterpretedDataPool =
                                       c_InterpretedNode.c_DataPools[u32_ItCurrentDataPool];
                                    C_OSCParamSetInterpretedList c_NewList;
                                    mh_CreateInterpretedList(rc_List, c_NewList);
                                    rc_InterpretedDataPool.c_Lists.push_back(c_NewList);
                                 }
                                 else
                                 {
                                    s32_Retval = C_RANGE;
                                 }
                              }
                           }
                           //Case 2: New data pool
                           //---------------------
                           if (q_DataPoolFound == false)
                           {
                              C_OSCParamSetDataPoolInfo c_DataPoolInfo;
                              C_OSCParamSetInterpretedDataPool c_NewInterpretedDataPool;
                              C_OSCParamSetInterpretedList c_NewList;
                              //Info
                              //Data pool crc
                              c_DataPoolInfo.u32_DataPoolCrc = 0;
                              rc_DataPool.CalcDefinitionHash(c_DataPoolInfo.u32_DataPoolCrc);
                              c_DataPoolInfo.c_Name = rc_DataPool.c_Name;
                              c_DataPoolInfo.u32_NvMSize = rc_DataPool.u32_NvMSize;
                              c_DataPoolInfo.u32_NvMStartAddress = rc_DataPool.u32_NvMStartAddress;
                              c_DataPoolInfo.au8_Version[0] = rc_DataPool.au8_Version[0];
                              c_DataPoolInfo.au8_Version[1] = rc_DataPool.au8_Version[1];
                              c_DataPoolInfo.au8_Version[2] = rc_DataPool.au8_Version[2];
                              //Raw
                              c_RawNode.c_DataPools.push_back(c_DataPoolInfo);
                              //Interpreted
                              c_NewInterpretedDataPool.c_DataPoolInfo = c_DataPoolInfo;
                              //List
                              mh_CreateInterpretedList(rc_List, c_NewList);
                              c_NewInterpretedDataPool.c_Lists.push_back(c_NewList);
                              c_InterpretedNode.c_DataPools.push_back(c_NewInterpretedDataPool);
                              //Update indices
                              c_AlreadyUsedDataPoolIndices.push_back(rc_DataPoolListId.u32_DataPoolIndex);
                           }
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  //Write data
                  if (s32_Retval == C_NO_ERR)
                  {
                     if (this->mc_ImageFileHandler.AddRawDataForNode(c_RawNode) == C_NO_ERR)
                     {
                        if (this->mc_ImageFileHandler.AddInterpretedDataForNode(c_InterpretedNode) == C_NO_ERR)
                        {
                           //Finished
                           //Update state
                           this->me_CreateParameterSetWorkflowState = C_OSCDataDealerNvmSafe::eCPSFS_FILE_CREATED;
                        }
                        else
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_OVERFLOW;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create clean file from internally stored content without adding a CRC

   Note: this function handles a file step and there is only one parameter set file for each parametrization process,
         so this function needs to only be called once,
         so all participating data dealers can continue with the next step (if there is any)

   \param[in] orc_Path     Parameter set file path
   \param[in] orc_FileInfo Optional general file information

   \return
   C_NO_ERR   data saved
   C_RANGE    file already exists
   C_OVERFLOW Wrong sequence of function calls
   C_BUSY     file already exists
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeCreateCleanFileWithoutCRC(const C_SCLString & orc_Path,
                                                                const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo)
{
   sint32 s32_Retval;

   if (TGL_FileExists(orc_Path) == false)
   {
      if (this->me_ParameterSetFileState == C_OSCDataDealerNvmSafe::ePSFS_DATA_RESET)
      {
         this->mc_ImageFileHandler.AddInterpretedFileData(orc_FileInfo);
         s32_Retval = this->mc_ImageFileHandler.CreateCleanFileWithoutCRC(orc_Path);
         if (s32_Retval == C_NO_ERR)
         {
            //Update internal variable
            this->mc_ParameterSetFilePath = orc_Path;
            //Update state
            this->me_ParameterSetFileState = C_OSCDataDealerNvmSafe::ePSFS_FILE_CREATED;
         }
      }
      else
      {
         s32_Retval = C_OVERFLOW;
      }
   }
   else
   {
      s32_Retval = C_BUSY;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read file and update internally stored content (cleared at start)

   Warning: CRC is not checked

   Note: this function handles a file step and there is only one parameter set file for each parametrization process,
         so this function needs to only be called once,
         so all participating data dealers can continue with the next step (if there is any)

   \param[in] orc_Path   Parameter file path

   \return
   C_NO_ERR   data read
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   file does not contain essential information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeReadFileWithoutCRC(const C_SCLString & orc_Path)
{
   sint32 s32_Retval;

   if (this->me_ParameterSetFileState == C_OSCDataDealerNvmSafe::ePSFS_FILE_CREATED)
   {
      if (this->mc_ParameterSetFilePath == orc_Path)
      {
         s32_Retval = this->mc_ImageFileHandler.ReadFile(orc_Path, true);
         if (s32_Retval == C_NO_ERR)
         {
            //Update state
            this->me_ParameterSetFileState = C_OSCDataDealerNvmSafe::ePSFS_FILE_READ_WITHOUT_CRC;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_OVERFLOW;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The function reads the contents of a parameter set file without checking the file CRC

   \param[in]  orc_Path          File path
   \param[out] orc_DataPoolLists Loaded data pool lists (Always cleared at start)

   \return
   C_NO_ERR   Lists valid
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
   C_CONFIG   Mismatch of data with current node
               or no valid pointer to the original instance of "C_OSCNode" is set in "C_OSCDataDealer"
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeCheckParameterFileContents(const C_SCLString & orc_Path,
                                                                 std::vector<C_OSCNodeDataPoolListId> & orc_DataPoolLists)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_DataPoolLists.clear();
   if (this->me_CreateParameterSetWorkflowState == C_OSCDataDealerNvmSafe::eCPSFS_FILE_CREATED)
   {
      if (C_OSCDataDealerNvmSafe::mc_ParameterSetFilePath == orc_Path)
      {
         if (this->mpc_Node != NULL)
         {
            const C_OSCParamSetRawNode * const pc_Node = this->mc_ImageFileHandler.GetRawDataForNode(
               this->mpc_Node->c_Properties.c_Name);
            if (pc_Node != NULL)
            {
               if (m_CheckParameterFileContent(*pc_Node) == C_NO_ERR)
               {
                  //For each raw data pool
                  //For each raw entry
                  for (uint32 u32_ItRawEntry =
                          0; (u32_ItRawEntry < pc_Node->c_Entries.size()) && (s32_Retval == C_NO_ERR);
                       ++u32_ItRawEntry)
                  {
                     bool q_Found = false;
                     const C_OSCParamSetRawEntry & rc_CurRawEntry = pc_Node->c_Entries[u32_ItRawEntry];
                     //Find data pool list
                     for (uint32 u32_ItDataPool = 0;
                          (u32_ItDataPool < this->mpc_Node->c_DataPools.size()) && (s32_Retval == C_NO_ERR);
                          ++u32_ItDataPool)
                     {
                        const C_OSCNodeDataPool & rc_DataPool = this->mpc_Node->c_DataPools[u32_ItDataPool];
                        //Check data pool address range and for NVM datapool
                        if (((rc_DataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
                             (rc_DataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM)) &&
                            (rc_DataPool.u32_NvMStartAddress <= rc_CurRawEntry.u32_StartAddress) &&
                            ((rc_DataPool.u32_NvMStartAddress + rc_DataPool.u32_NvMSize) >
                             rc_CurRawEntry.u32_StartAddress))
                        {
                           for (uint32 u32_ItList = 0;
                                (u32_ItList < rc_DataPool.c_Lists.size()) && (s32_Retval == C_NO_ERR); ++u32_ItList)
                           {
                              const C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u32_ItList];
                              //Check list address range
                              if ((rc_List.u32_NvMStartAddress <= rc_CurRawEntry.u32_StartAddress) &&
                                  ((rc_List.u32_NvMStartAddress + rc_List.u32_NvMSize) >
                                   rc_CurRawEntry.u32_StartAddress))
                              {
                                 if ((rc_CurRawEntry.c_Bytes.size() == rc_List.GetNumBytesUsed()) ||
                                     (rc_CurRawEntry.c_Bytes.size() == 2))
                                 {
                                    //Data pool list found
                                    const C_OSCNodeDataPoolListId c_Value(this->mu32_NodeIndex, u32_ItDataPool,
                                                                          u32_ItList);
                                    q_Found = true;
                                    orc_DataPoolLists.push_back(c_Value);
                                 }
                                 else
                                 {
                                    s32_Retval = C_CONFIG;
                                 }
                              }
                           }
                        }
                     }
                     if (q_Found == false)
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Remove duplicates
                     const std::vector<C_OSCNodeDataPoolListId> c_Copy = orc_DataPoolLists;
                     orc_DataPoolLists.clear();
                     orc_DataPoolLists.reserve(c_Copy.size());
                     for (uint32 u32_ItCopy = 0; u32_ItCopy < c_Copy.size(); ++u32_ItCopy)
                     {
                        bool q_Found = false;
                        const C_OSCNodeDataPoolListId & rc_CopyId = c_Copy[u32_ItCopy];
                        //Search if already appended
                        for (uint32 u32_ItExisting = 0; u32_ItExisting < orc_DataPoolLists.size(); ++u32_ItExisting)
                        {
                           const C_OSCNodeDataPoolListId & rc_ExistingId = orc_DataPoolLists[u32_ItExisting];
                           if (((rc_ExistingId.u32_NodeIndex == rc_CopyId.u32_NodeIndex) &&
                                (rc_ExistingId.u32_DataPoolIndex == rc_CopyId.u32_DataPoolIndex)) &&
                               (rc_ExistingId.u32_ListIndex == rc_CopyId.u32_ListIndex))
                           {
                              q_Found = true;
                           }
                        }
                        if (q_Found == false)
                        {
                           orc_DataPoolLists.push_back(rc_CopyId);
                        }
                     }
                     //As not every list might require a CRC the new list size cannot be exactly matched
                     if ((orc_DataPoolLists.size() >= (c_Copy.size() / 2)) &&
                         (orc_DataPoolLists.size() <= c_Copy.size()))
                     {
                        //Finished
                        this->me_CreateParameterSetWorkflowState = C_OSCDataDealerNvmSafe::eCPSFS_FILE_CHECKED;
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_OVERFLOW;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update/add CRC for provided file

   Note: this function handles a file step and there is only one parameter set file for each parametrization process,
         so this function needs to only be called once,
         so all participating data dealers can continue with the next step (if there is any)

   \param[in] orc_Path   Parameter set file path

   \return
   C_NO_ERR   CRC updated
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeUpdateCRCForFile(const C_SCLString & orc_Path)
{
   sint32 s32_Retval;

   if (this->me_ParameterSetFileState == C_OSCDataDealerNvmSafe::ePSFS_FILE_READ_WITHOUT_CRC)
   {
      if (this->mc_ParameterSetFilePath == orc_Path)
      {
         s32_Retval = C_OSCParamSetHandler::h_UpdateCRCForFile(orc_Path);
         if (s32_Retval == C_NO_ERR)
         {
            //Update state
            this->me_ParameterSetFileState = C_OSCDataDealerNvmSafe::ePSFS_IDLE;
         }
         else
         {
            s32_Retval = C_RD_WR;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_OVERFLOW;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read file and update internally stored content (cleared at start)

   After loading this functions checks whether the file contains data for exactly one node.
   Otherwise it will fail.

   \param[in] orc_Path   Parameter set file path

   \return
   C_NO_ERR   data read
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   file does not contain data for exactly one node (zero or more than one)
   C_CHECKSUM specified file is present but checksum is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeReadFileWithCRC(const C_SCLString & orc_Path)
{
   sint32 s32_Retval = this->mc_ImageFileHandler.ReadFile(orc_Path, false);

   if (s32_Retval == C_NO_ERR)
   {
      //data for one file contained ?
      if (this->mc_ImageFileHandler.GetNumberOfNodes() != 1U)
      {
         C_SCLString c_Error;
         c_Error.PrintFormatted(
            "File \"%s\"  Expected: contains parameters for one device  Found: contains parameters for %d devices\n",
            orc_Path.c_str(), this->mc_ImageFileHandler.GetNumberOfNodes());
         this->mc_ImageFileHandler.ClearContent();
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading parameter set file", c_Error);
      }
      else
      {
         //Update state
         this->me_ParameterSetFileState = C_OSCDataDealerNvmSafe::ePSFS_FILE_READ_WITH_CRC;
         this->mc_ParameterSetFilePath = orc_Path;
      }
   }
   else if (s32_Retval == C_CHECKSUM)
   {
      //Same return value
   }
   else
   {
      s32_Retval = C_RD_WR;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read a parameter set file and writes the contained values and CRCs to the ECU's NVM

   Sequence:
   * read parameter set file
   * write to NVM

   \param[in]  orc_Path           File path
   \param[out] ors32_ResultDetail Result detail

   \return
   Return        Error Detail
   C_NO_ERR      1            File successfully written to ECU
   C_OVERFLOW    5            Wrong sequence of function calls
                 6            Path mismatch with previous function call
   C_CONFIG      1            No valid diagnostic protocol is set in "C_OSCDataDealer"
                 2            No valid pointer to the original instance of "C_OSCNode" is set in "C_OSCDataDealer"
   C_NOACT       1            Communication protocol service could not be requested
   C_TIMEOUT     1            Communication protocol service has timed out
   C_WARN        1            Communication protocol service error response was received
   C_UNKNOWN_ERR <undefined>  Communication protocol failed with non-specified error code
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::NvmSafeWriteParameterSetFile(const C_SCLString & orc_Path, sint32 & ors32_ResultDetail)
{
   sint32 s32_Retval = C_NO_ERR;

   ors32_ResultDetail = -1;
   if (this->mpc_Node != NULL)
   {
      if (this->mpc_DiagProtocol != NULL)
      {
         if (C_OSCDataDealerNvmSafe::me_ParameterSetFileState == C_OSCDataDealerNvmSafe::ePSFS_FILE_READ_WITH_CRC)
         {
            if (C_OSCDataDealerNvmSafe::mc_ParameterSetFilePath == orc_Path)
            {
               //when we get here we can be sure there was data for exactly one node contained in the file
               // as this is checked by NvmSafeReadFileWithCRC
               const C_OSCParamSetRawNode * const pc_Node = this->mc_ImageFileHandler.GetRawDataForNode(0U);
               tgl_assert(pc_Node != NULL);
               if (pc_Node != NULL)
               {
                  //write the actual values:
                  for (uint32 u32_ItEntry = 0; u32_ItEntry < pc_Node->c_Entries.size(); u32_ItEntry++)
                  {
                     const C_OSCParamSetRawEntry & rc_Entry = pc_Node->c_Entries[u32_ItEntry];
                     s32_Retval = this->mpc_DiagProtocol->NvmWrite(rc_Entry.u32_StartAddress, rc_Entry.c_Bytes, NULL);
                     //Map error codes
                     switch (s32_Retval)
                     {
                     case C_TIMEOUT:
                        ors32_ResultDetail = 1;
                        break;
                     case C_NOACT:
                     case C_CONFIG:
                        s32_Retval = C_NOACT;
                        ors32_ResultDetail = 1;
                        break;
                     case C_RANGE:
                     case C_RD_WR:
                     case C_WARN:
                        s32_Retval = C_WARN;
                        ors32_ResultDetail = 1;
                        break;
                     case C_NO_ERR: //positive result
                        ors32_ResultDetail = 1;
                        break;
                     default:
                        //Not documented error was returned by function
                        s32_Retval = C_UNKNOWN_ERR;
                        osc_write_log_info("Parametrization", "Not documented error code " +
                                           C_SCLString::IntToStr(s32_Retval) + " was returned by NvmWrite");
                        break;
                     }
                     if (s32_Retval != C_NO_ERR)
                     {
                        break;
                     }
                  }
               }
            }
            else
            {
               s32_Retval = C_OVERFLOW;
               ors32_ResultDetail = 6;
            }
         }
         else
         {
            s32_Retval = C_OVERFLOW;
            ors32_ResultDetail = 5;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
         ors32_ResultDetail = 1;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      ors32_ResultDetail = 2;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create interpreted list

   \param[in]  orc_List            List data
   \param[out] orc_InterpretedList New interpreted list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCDataDealerNvmSafe::mh_CreateInterpretedList(const C_OSCNodeDataPoolList & orc_List,
                                                      C_OSCParamSetInterpretedList & orc_InterpretedList)
{
   orc_InterpretedList.c_Name = orc_List.c_Name;
   orc_InterpretedList.c_Elements.reserve(orc_List.c_Elements.size());
   for (uint32 u32_ItElement = 0; u32_ItElement < orc_List.c_Elements.size(); ++u32_ItElement)
   {
      C_OSCParamSetInterpretedElement c_NewElement;
      const C_OSCNodeDataPoolListElement & rc_Element = orc_List.c_Elements[u32_ItElement];
      c_NewElement.c_Name = rc_Element.c_Name;
      c_NewElement.c_NvmValue = rc_Element.c_NvmValue;
      orc_InterpretedList.c_Elements.push_back(c_NewElement);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if raw node parameters match internal node

   \param[in] orc_Node Raw node parameters to check

   \return
   C_NO_ERR   Match
   C_CONFIG   Node not set
   C_RANGE    Node mismatch
   C_TIMEOUT  Data pool not found
   C_CHECKSUM Data pool CRC mismatch
   C_RD_WR    Data pool version mismatch
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::m_CheckParameterFileContent(const C_OSCParamSetRawNode & orc_Node)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mpc_Node != NULL)
   {
      if ((this->mpc_Node->c_Properties.c_Name == orc_Node.c_Name) && (orc_Node.c_DataPools.size() > 0))
      {
         //Check all data pool info segments
         for (uint32 u32_ItReadDataPool = 0U;
              (u32_ItReadDataPool < orc_Node.c_DataPools.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItReadDataPool)
         {
            const C_OSCParamSetDataPoolInfo & rc_DataPoolInfo = orc_Node.c_DataPools[u32_ItReadDataPool];
            bool q_Found = false;
            //Find matching data pool
            for (uint32 u32_ItNodeDataPool = 0U;
                 (u32_ItNodeDataPool < this->mpc_Node->c_DataPools.size()) && (s32_Retval == C_NO_ERR);
                 ++u32_ItNodeDataPool)
            {
               const C_OSCNodeDataPool & rc_NodeDataPool = this->mpc_Node->c_DataPools[u32_ItNodeDataPool];
               if (rc_NodeDataPool.c_Name == rc_DataPoolInfo.c_Name)
               {
                  uint32 u32_Crc = 0U;
                  rc_NodeDataPool.CalcDefinitionHash(u32_Crc);
                  q_Found = true;
                  //Check content
                  if (rc_DataPoolInfo.u32_DataPoolCrc != u32_Crc)
                  {
                     s32_Retval = C_CHECKSUM;
                  }
                  else if ((((rc_DataPoolInfo.au8_Version[0] != rc_NodeDataPool.au8_Version[0])) ||
                            (rc_DataPoolInfo.au8_Version[1] != rc_NodeDataPool.au8_Version[1])) ||
                           (rc_DataPoolInfo.au8_Version[2] != rc_NodeDataPool.au8_Version[2]))
                  {
                     s32_Retval = C_RD_WR;
                  }
                  else
                  {
                     //Match
                  }
               }
            }
            if (q_Found == false)
            {
               s32_Retval = C_TIMEOUT;
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create raw entry for list and write bytes to interpreted data

   \param[in]  orc_List    List to set raw entry from
   \param[out] orc_Entry   Raw entry from list
   \param[out] opu8_NrCode if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   Entry successfully created
   C_RANGE    List has no elements. Nothing to read.
              Datapool element size configuration does not match with count of read bytes
   C_TIMEOUT  Expected response not received within timeout
   C_NOACT    Could not send request (e.g. Tx buffer full)
   C_WARN     Error response or malformed protocol response
   C_CONFIG   Pre-requisites not correct; e.g. driver not initialized or
              parameter out of range (checked by client side)
   C_RD_WR    Datapool element size configuration does not match with count of read bytes
   C_CHECKSUM Checksum of read datapool list is invalid
   C_COM      expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCDataDealerNvmSafe::m_CreateRawEntryAndPrepareInterpretedData(C_OSCNodeDataPoolList & orc_List,
                                                                         C_OSCParamSetRawEntry & orc_Entry,
                                                                         uint8 * const opu8_NrCode)
{
   std::vector<uint8> c_Values;
   sint32 s32_Retval = this->m_NvmReadListRaw(orc_List, c_Values, opu8_NrCode);
   if (s32_Retval == C_NO_ERR)
   {
      //Raw
      orc_Entry.u32_StartAddress = orc_List.u32_NvMStartAddress;
      orc_Entry.c_Bytes = c_Values;
      s32_Retval = this->m_SaveDumpToList(c_Values, orc_List);
      if (s32_Retval != C_NO_ERR)
      {
         //Translate error values if necessary
         switch (s32_Retval)
         {
         case C_RD_WR:
            s32_Retval = C_RANGE;
            break;
         case C_CHECKSUM:
            s32_Retval = C_CHECKSUM;
            break;
         default:
            //Not documented error was returned by function
            s32_Retval = C_UNKNOWN_ERR;
            osc_write_log_info("parametrization",
                               "Not documented error was returned by m_InterpretDumbToList");
            break;
         }
      }
   }
   else
   {
      //Translate error values if necessary
      switch (s32_Retval)
      {
      case C_TIMEOUT:
      case C_WARN:
      case C_NOACT:
         //Same error value
         break;
      case C_OVERFLOW:
         s32_Retval = C_RANGE;
         break;
      default:
         //Not documented error was returned by function
         s32_Retval = C_UNKNOWN_ERR;
         osc_write_log_info("parametrization",
                            "Not documented error was returned by m_NvmReadListRaw");
         break;
      }
   }
   return s32_Retval;
}
