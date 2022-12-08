//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for comparison operation of sorting algorithm (implementation)

   Utility class for comparison operation of sorting algorithm

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_SdBueSortHelper.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
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
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueSortHelper::C_SdBueSortHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare message 1 to 2

   Primary sorting criteria: alphabetical

   \param[in]  orc_Message1   Message 1 identification indices
   \param[in]  orc_Message2   Message 2 identification indices

   \return
   true  Message 1 smaller
   false Message 1 greater or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueSortHelper::operator ()(const C_OscCanMessageIdentificationIndices & orc_Message1,
                                    const C_OscCanMessageIdentificationIndices & orc_Message2) const
{
   //Default: messages equal
   bool q_Retval = false;

   //Equal
   if (orc_Message1 == orc_Message2)
   {
      //Messages equal
      q_Retval = false;
   }
   else
   {
      const C_OscCanMessage * const pc_Message1 = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_Message1);
      const C_OscCanMessage * const pc_Message2 = C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_Message2);

      //String comparison
      tgl_assert((pc_Message1 != NULL) && (pc_Message2 != NULL));
      if ((pc_Message1 != NULL) && (pc_Message2 != NULL))
      {
         q_Retval = h_CompareString(pc_Message1->c_Name, pc_Message2->c_Name);
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare string 1 to 2

   Primary sorting criteria: alphabetical

   \param[in]  orc_String1    String 1
   \param[in]  orc_String2    String 2

   \return
   true  String 1 smaller
   false String 1 greater or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueSortHelper::h_CompareString(const stw::scl::C_SclString & orc_String1,
                                        const stw::scl::C_SclString & orc_String2)
{
   //Default: strings equal
   bool q_Retval = false;
   bool q_Equal = true;

   //Compare on character basis
   for (uint32_t u32_ItChar = 1; (u32_ItChar <= orc_String1.Length()) && (q_Equal == true); ++u32_ItChar)
   {
      if (u32_ItChar <= orc_String2.Length())
      {
         const char_t cn_Char1 = orc_String1[u32_ItChar];
         const char_t cn_Char2 = orc_String2[u32_ItChar];
         if (cn_Char1 == cn_Char2)
         {
            //Messages equal
            q_Retval = false;
         }
         else
         {
            q_Equal = false;
            if (static_cast<int32_t>(cn_Char1) < static_cast<int32_t>(cn_Char2))
            {
               //Message 1 smaller
               q_Retval = true;
            }
            else
            {
               //Message 1 greater
               q_Retval = false;
            }
         }
      }
      else
      {
         q_Equal = false;
         //Message 1 greater
         q_Retval = false;
      }
   }
   if (q_Equal == true)
   {
      if (orc_String1.Length() < orc_String2.Length())
      {
         //Message 1 smaller
         q_Retval = true;
      }
      else
      {
         //Messages equal
         q_Retval = false;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Message    Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueSortHelperSignal::C_SdBueSortHelperSignal(const C_OscCanMessageIdentificationIndices & orc_Message) :
   mc_Message(orc_Message)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare message 1 to 2

   Primary sorting criteria: alphabetical

   \param[out]  oru32_Signal1    Index of signal 1
   \param[in]   oru32_Signal2    Index of signal 2

   \return
   true  Message 1 smaller
   false Message 1 greater or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueSortHelperSignal::operator ()(const uint32_t & oru32_Signal1, const uint32_t & oru32_Signal2) const
{
   //Default: signals equal
   bool q_Retval = false;
   const C_OscCanSignal * const pc_Signal1 =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(this->mc_Message, oru32_Signal1);
   const C_OscCanSignal * const pc_Signal2 =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(this->mc_Message, oru32_Signal2);

   if ((pc_Signal1 != NULL) && (pc_Signal2 != NULL))
   {
      q_Retval = pc_Signal1->u16_ComBitStart < pc_Signal2->u16_ComBitStart;
      //q_Retval = C_SdBueSortHelper::h_CompareString(pc_Signal1->c_Name, pc_Signal2->c_Name);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort one message vector by name

   Algorithm: bubblesort
              Missing optimization: each iteration will result in the last element being at the correct position,
               so every iteration one less element should be compared

   \param[in,out]  orc_OscMessages  OSC Messages
   \param[in,out]  orc_UiMessages   UI Messages
   \param[in,out]  orc_OscList      OSC data pool part
   \param[in,out]  orc_UiList       UI data pool part

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueSortHelper::h_SortOneMessageVector(std::vector<C_OscCanMessage> & orc_OscMessages,
                                                  std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                                  C_OscNodeDataPoolList & orc_OscList,
                                                  C_PuiSdNodeDataPoolList & orc_UiList)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_OscMessages.size() > 0UL)
   {
      while ((C_SdBueSortHelper::mh_CheckMessagesSorted(orc_OscMessages) == false) && (s32_Retval == C_NO_ERR))
      {
         //Compare each element and swap if necessary
         for (uint32_t u32_ItMessagePair = 0;
              (u32_ItMessagePair < (static_cast<uint32_t>(orc_OscMessages.size()) - 1UL)) && (s32_Retval == C_NO_ERR);
              ++u32_ItMessagePair)
         {
            const C_OscCanMessage & rc_CurrentMessage = orc_OscMessages[u32_ItMessagePair];
            const C_OscCanMessage & rc_NextMessage =
               orc_OscMessages[static_cast<std::vector< C_OscCanMessage>::size_type > (u32_ItMessagePair + 1UL)];
            //Compare
            if ((h_CompareString(rc_CurrentMessage.c_Name, rc_NextMessage.c_Name) == false) &&
                (rc_CurrentMessage.c_Name != rc_NextMessage.c_Name))
            {
               //Swap
               s32_Retval = mh_SwapMessages(u32_ItMessagePair, u32_ItMessagePair + 1UL, orc_OscMessages, orc_UiMessages,
                                            orc_OscList, orc_UiList);
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if messages sorted properly by name

   \param[in]  orc_OscMessages   OSC Messages

   \return
   True  Messages sorted by name
   False At least one message is not sorted properly by name
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueSortHelper::mh_CheckMessagesSorted(const std::vector<C_OscCanMessage> & orc_OscMessages)
{
   bool q_Retval = true;

   if (orc_OscMessages.size() > 0UL)
   {
      for (uint32_t u32_ItMessagePair = 0; u32_ItMessagePair < (static_cast<uint32_t>(orc_OscMessages.size()) - 1UL);
           ++u32_ItMessagePair)
      {
         const C_OscCanMessage & rc_CurrentMessage = orc_OscMessages[u32_ItMessagePair];
         const C_OscCanMessage & rc_NextMessage =
            orc_OscMessages[static_cast<std::vector< C_OscCanMessage>::size_type > (u32_ItMessagePair + 1UL)];
         //Compare
         if ((h_CompareString(rc_CurrentMessage.c_Name,
                              rc_NextMessage.c_Name) == false) && (rc_CurrentMessage.c_Name != rc_NextMessage.c_Name))
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Swap two messages

   \param[in]      ou32_MessageIndex1  Message 1 index (ID in OSC Messages)
   \param[in]      ou32_MessageIndex2  Message 2 index (ID in OSC Messages)
   \param[in,out]  orc_OscMessages     OSC Messages
   \param[in,out]  orc_UiMessages      UI Messages
   \param[in,out]  orc_OscList         OSC data pool part
   \param[in,out]  orc_UiList          UI data pool part

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueSortHelper::mh_SwapMessages(const uint32_t ou32_MessageIndex1, const uint32_t ou32_MessageIndex2,
                                           std::vector<C_OscCanMessage> & orc_OscMessages,
                                           std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                           C_OscNodeDataPoolList & orc_OscList, C_PuiSdNodeDataPoolList & orc_UiList)
{
   int32_t s32_Retval = C_NO_ERR;

   //Consistency checks
   if (((ou32_MessageIndex1 < orc_OscMessages.size()) &&
        ((ou32_MessageIndex2 < orc_OscMessages.size()) &&
         ((orc_OscMessages.size() == orc_UiMessages.size()) &&
          (orc_OscList.c_Elements.size() == orc_UiList.c_DataPoolListElements.size())))) &&
       (ou32_MessageIndex1 != ou32_MessageIndex2))
   {
      std::vector<C_OscNodeDataPoolListElement> c_OscMessage1SignalCopy;
      std::vector<C_OscNodeDataPoolListElement> c_OscMessage2SignalCopy;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UiMessage1SignalCopy;
      std::vector<C_PuiSdNodeDataPoolListElement> c_UiMessage2SignalCopy;
      std::vector<uint32_t> c_SignalIndices;
      uint32_t u32_FirstIndex;
      uint32_t u32_SecondIndex;
      //Swap
      //----
      //Messages
      //--------
      //Copy!
      const C_OscCanMessage c_OscMessage1 = orc_OscMessages[ou32_MessageIndex1];
      const C_OscCanMessage c_OscMessage2 = orc_OscMessages[ou32_MessageIndex2];
      const C_PuiSdNodeCanMessage c_UiMessage1 = orc_UiMessages[ou32_MessageIndex1];
      const C_PuiSdNodeCanMessage c_UiMessage2 = orc_UiMessages[ou32_MessageIndex2];
      if (ou32_MessageIndex1 < ou32_MessageIndex2)
      {
         u32_FirstIndex = ou32_MessageIndex1;
         u32_SecondIndex = ou32_MessageIndex2;
      }
      else
      {
         u32_FirstIndex = ou32_MessageIndex2;
         u32_SecondIndex = ou32_MessageIndex1;
      }
      //Erase starting at last
      orc_OscMessages.erase(orc_OscMessages.begin() + u32_SecondIndex);
      orc_UiMessages.erase(orc_UiMessages.begin() + u32_SecondIndex);
      orc_OscMessages.erase(orc_OscMessages.begin() + u32_FirstIndex);
      orc_UiMessages.erase(orc_UiMessages.begin() + u32_FirstIndex);

      //Insert starting at first
      orc_OscMessages.insert(orc_OscMessages.begin() + u32_FirstIndex, c_OscMessage2);
      orc_UiMessages.insert(orc_UiMessages.begin() + u32_FirstIndex, c_UiMessage2);
      orc_OscMessages.insert(orc_OscMessages.begin() + u32_SecondIndex, c_OscMessage1);
      orc_UiMessages.insert(orc_UiMessages.begin() + u32_SecondIndex, c_UiMessage1);
      //Signals
      //-------
      //Reserve
      c_OscMessage1SignalCopy.reserve(c_OscMessage1.c_Signals.size());
      c_UiMessage1SignalCopy.reserve(c_OscMessage1.c_Signals.size());
      c_OscMessage2SignalCopy.reserve(c_OscMessage2.c_Signals.size());
      c_UiMessage2SignalCopy.reserve(c_OscMessage2.c_Signals.size());
      c_SignalIndices.reserve(c_OscMessage1.c_Signals.size() + c_OscMessage2.c_Signals.size());
      //Copy - 1
      for (uint32_t u32_ItSignal = 0; (u32_ItSignal < c_OscMessage1.c_Signals.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItSignal)
      {
         const C_OscCanSignal & rc_Signal = c_OscMessage1.c_Signals[u32_ItSignal];
         if (rc_Signal.u32_ComDataElementIndex < orc_OscList.c_Elements.size())
         {
            c_OscMessage1SignalCopy.push_back(orc_OscList.c_Elements[rc_Signal.u32_ComDataElementIndex]);
            c_UiMessage1SignalCopy.push_back(orc_UiList.c_DataPoolListElements[rc_Signal.u32_ComDataElementIndex]);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         c_SignalIndices.push_back(rc_Signal.u32_ComDataElementIndex);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Copy - 2
         for (uint32_t u32_ItSignal = 0; (u32_ItSignal < c_OscMessage2.c_Signals.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItSignal)
         {
            const C_OscCanSignal & rc_Signal = c_OscMessage2.c_Signals[u32_ItSignal];
            if (rc_Signal.u32_ComDataElementIndex < orc_OscList.c_Elements.size())
            {
               c_OscMessage2SignalCopy.push_back(orc_OscList.c_Elements[rc_Signal.u32_ComDataElementIndex]);
               c_UiMessage2SignalCopy.push_back(
                  orc_UiList.c_DataPoolListElements[rc_Signal.u32_ComDataElementIndex]);
            }
            else
            {
               s32_Retval = C_RANGE;
            }
            c_SignalIndices.push_back(rc_Signal.u32_ComDataElementIndex);
         }
         if (s32_Retval == C_NO_ERR)
         {
            uint32_t u32_SignalCounter = 0;
            //Sort signal indices
            c_SignalIndices = C_Uti::h_UniquifyAndSortDescending(c_SignalIndices);
            //Erase signals, erase the biggest index first
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < c_SignalIndices.size(); ++u32_ItSignal)
            {
               orc_OscList.c_Elements.erase(orc_OscList.c_Elements.begin() + c_SignalIndices[u32_ItSignal]);
               orc_UiList.c_DataPoolListElements.erase(orc_UiList.c_DataPoolListElements.begin() +
                                                       c_SignalIndices[u32_ItSignal]);
            }
            //Recalculate data element indices and insert signals at the appropriate point
            for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_OscMessages.size(); ++u32_ItMessage)
            {
               C_OscCanMessage & rc_Message = orc_OscMessages[u32_ItMessage];
               if (u32_ItMessage == u32_FirstIndex)
               {
                  //First index -> insert signals for 2
                  for (uint32_t u32_ItSignalStorage = 0; u32_ItSignalStorage < c_OscMessage2SignalCopy.size();
                       ++u32_ItSignalStorage)
                  {
                     orc_OscList.c_Elements.insert(
                        orc_OscList.c_Elements.begin() + u32_SignalCounter + u32_ItSignalStorage,
                        c_OscMessage2SignalCopy[u32_ItSignalStorage]);
                     orc_UiList.c_DataPoolListElements.insert(
                        orc_UiList.c_DataPoolListElements.begin() + u32_SignalCounter + u32_ItSignalStorage,
                        c_UiMessage2SignalCopy[u32_ItSignalStorage]);
                  }
               }
               else if (u32_ItMessage == u32_SecondIndex)
               {
                  //Second index -> insert signals for 1
                  for (uint32_t u32_ItSignalStorage = 0; u32_ItSignalStorage < c_OscMessage1SignalCopy.size();
                       ++u32_ItSignalStorage)
                  {
                     orc_OscList.c_Elements.insert(
                        orc_OscList.c_Elements.begin() + u32_SignalCounter + u32_ItSignalStorage,
                        c_OscMessage1SignalCopy[u32_ItSignalStorage]);
                     orc_UiList.c_DataPoolListElements.insert(
                        orc_UiList.c_DataPoolListElements.begin() + u32_SignalCounter + u32_ItSignalStorage,
                        c_UiMessage1SignalCopy[u32_ItSignalStorage]);
                  }
               }
               else
               {
                  //No special handling necessary
               }
               //ALWAYS update the signal index as well (assuming the new signals were already inserted)
               for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
               {
                  C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
                  rc_Signal.u32_ComDataElementIndex = u32_SignalCounter;
                  //Iterate signal index
                  ++u32_SignalCounter;
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
