//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connects the openSYDE protocol driver to a node with GUI specific functionality

   - Depending on the type of data pool element call the
     corresponding read function from C_OSCDiagProtocolBase
   - Place the results into the defined data pool element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_SyvComDataDealer.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSvDbDataElementContent.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
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
C_SyvComDataDealer::C_SyvComDataDealer(void) :
   C_OSCDataDealerNvmSafe()
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
C_SyvComDataDealer::C_SyvComDataDealer(C_OSCNode * const opc_Node, const uint32 ou32_NodeIndex,
                                       C_OSCDiagProtocolBase * const opc_DiagProtocol) :
   C_OSCDataDealerNvmSafe(opc_Node, ou32_NodeIndex, opc_DiagProtocol)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDataDealer::~C_SyvComDataDealer(void)
{
   //nothing to do ...
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a widget to inform about new datapool events

   \param[in]     opc_Widget     Pointer to dashboard widget base
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDataDealer::RegisterWidget(C_PuiSvDbDataElementHandler * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < opc_Widget->GetWidgetDataPoolElementCount(); ++u32_Counter)
      {
         C_PuiSvDbNodeDataPoolListElementId c_DpElementId;
         if (opc_Widget->GetDataPoolElementIndex(u32_Counter, c_DpElementId) == C_NO_ERR)
         {
            // Is it relevant for this data dealer (No handling of bus signals here)?
            if ((c_DpElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT) &&
                (c_DpElementId.u32_NodeIndex == this->GetNodeIndex()))
            {
               QMap<C_OSCNodeDataPoolListElementId,
                    QList<C_PuiSvDbDataElementHandler *> >::iterator c_ItElement;

               // Is there an entry?
               c_ItElement = this->mc_AllWidgets.find(c_DpElementId);

               // Add the widget to the map
               if (c_ItElement != this->mc_AllWidgets.end())
               {
                  //Check if not already contained
                  if (c_ItElement.value().contains(opc_Widget) == false)
                  {
                     // There is a list for this datapool element already
                     c_ItElement.value().push_back(opc_Widget);
                  }
               }
               else
               {
                  // The map has no entry for this datapool element. Add a new list with this widget
                  QList<C_PuiSvDbDataElementHandler *> c_List;

                  c_List.push_back(opc_Widget);
                  this->mc_AllWidgets.insert(c_DpElementId, c_List);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's data pool and updates the associated widgets

   Calls DataPoolRead of C_OSCDataDealer and updates the registered widgets.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data read and placed in data pool
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the index range ?)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response reveived
   C_OVERFLOW  size of data received from server does not match size of specified data pool element
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDataDealer::DataPoolRead(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                        const uint16 ou16_ElementIndex, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   s32_Return = C_OSCDataDealer::DataPoolRead(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, opu8_NrCode);

   if (s32_Return == C_NO_ERR)
   {
      this->m_OnReadDataPoolEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's NVM and updates the associated widgets

   Calls NvmRead of C_OSCDataDealer and updates the registered widgets.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data read and placed in data pool
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the address range ?)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response received
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDataDealer::NvmRead(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                   const uint16 ou16_ElementIndex, uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   s32_Return = C_OSCDataDealer::NvmRead(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, opu8_NrCode);

   if (s32_Return == C_NO_ERR)
   {
      this->m_OnReadDataPoolNvmEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads a specific NVM list of ECU

   The CRC of the list will be checked.

   \param[in]     ou32_DataPoolIndex   Node datapool index
   \param[in]     ou32_ListIndex       Node datapool list index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    Reading of list successful
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     Input paramter invalid
   C_OVERFLOW  List has no elements. Nothing to read
   C_RD_WR     Datapool element size configuration does not match with count of read bytes
   C_CHECKSUM  Checksum of read datapool list is invalid
   C_TIMEOUT   Expected response not received within timeout
   C_NOACT     Could not send request (e.g. Tx buffer full)
   C_WARN      Error response or malformed protocol response
   C_COM       Pre-requisites not correct; e.g. driver not initialized or
               parameter out of range (checked by client side)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvComDataDealer::NvmReadList(const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex,
                                       uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   s32_Return = C_OSCDataDealerNvm::NvmReadList(ou32_DataPoolIndex, ou32_ListIndex, opu8_NrCode);

   //Allow checksum errors (handled by param widget)
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_CHECKSUM))
   {
      uint32 u32_ElementCounter;
      const C_OSCNodeDataPoolList & rc_List = this->mpc_Node->c_DataPools[ou32_DataPoolIndex].c_Lists[ou32_ListIndex];

      for (u32_ElementCounter = 0U; u32_ElementCounter < rc_List.c_Elements.size(); ++u32_ElementCounter)
      {
         this->m_OnReadDataPoolNvmEventReceived(static_cast<uint8>(ou32_DataPoolIndex),
                                                static_cast<uint16>(ou32_ListIndex),
                                                static_cast<uint16>(u32_ElementCounter));
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvComDataDealer::m_OnReadDataPoolEventReceived(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                       const uint16 ou16_ElementIndex)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OSCNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      C_OSCNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                 ou16_ElementIndex);

      // Get the correct list with all pointers to the widgets which are showing this datapool element
      c_ItElement = this->mc_AllWidgets.find(c_ElementId);

      if (c_ItElement != this->mc_AllWidgets.end())
      {
         const QList<C_PuiSvDbDataElementHandler *> & rc_ListWidgets = c_ItElement.value();
         QList<C_PuiSvDbDataElementHandler *>::const_iterator c_ItWidget;
         const C_OSCNodeDataPoolContent * const pc_ElementContent =
            &this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex)->c_Value;
         // Create the necessary instance with a copy of the content. The timestamp will be filled in the constructor
         C_PuiSvDbDataElementContent c_DbContent(*pc_ElementContent);

         for (c_ItWidget = rc_ListWidgets.begin(); c_ItWidget != rc_ListWidgets.end(); ++c_ItWidget)
         {
            // Update the value
            (*c_ItWidget)->InsertNewValueIntoQueue(C_PuiSvDbNodeDataPoolListElementId(c_ElementId,
                                                                                      C_PuiSvDbNodeDataPoolListElementId
                                                                                      ::eDATAPOOL_ELEMENT, false, 0UL),
                                                   c_DbContent);
         }
      }
      else
      {
         stw_scl::C_SCLString c_Info;
         c_Info.PrintFormatted("C_SyvComDataDealer: incoming data with no interested widget " \
                               "(dp: %d List: %d element: %d) !", ou8_DataPoolIndex, ou16_ListIndex,
                               ou16_ElementIndex);
         osc_write_log_info("Asynchronous communication", c_Info);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handler for reception of async error response

   Called by the DataDealer after it has received an error response for a cyclic transmission.

   \param[in]  ou8_DataPoolIndex   data pool index that data was received for
   \param[in]  ou16_ListIndex      list index that data was received for
   \param[in]  ou16_ElementIndex   element index that data was received for
   \param[in]  ou8_ErrorCode       received error code
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDataDealer::m_OnReadDataPoolEventErrorReceived(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                            const uint16 ou16_ElementIndex, const uint8 ou8_ErrorCode)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OSCNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      C_OSCNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                 ou16_ElementIndex);

      // Get the correct list with all pointers to the widgets which are showing this datapool element
      c_ItElement = this->mc_AllWidgets.find(c_ElementId);

      if (c_ItElement != this->mc_AllWidgets.end())
      {
         const QList<C_PuiSvDbDataElementHandler *> & rc_ListWidgets = c_ItElement.value();
         QList<C_PuiSvDbDataElementHandler *>::const_iterator c_ItWidget;

         for (c_ItWidget = rc_ListWidgets.begin(); c_ItWidget != rc_ListWidgets.end(); ++c_ItWidget)
         {
            // Update the value
            (*c_ItWidget)->RegisterDataPoolElementCyclicError(C_PuiSvDbNodeDataPoolListElementId(c_ElementId,
                                                                                                 C_PuiSvDbNodeDataPoolListElementId
                                                                                                 ::eDATAPOOL_ELEMENT,
                                                                                                 false, 0UL),
                                                              ou8_ErrorCode);
         }
      }
      else
      {
         C_OSCDataDealerNvmSafe::m_OnReadDataPoolEventErrorReceived(ou8_DataPoolIndex, ou16_ListIndex,
                                                                    ou16_ElementIndex, ou8_ErrorCode);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvComDataDealer::m_OnReadDataPoolNvmEventReceived(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                          const uint16 ou16_ElementIndex)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OSCNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      C_OSCNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                 ou16_ElementIndex);

      // Get the correct list with all pointers to the widgets which are showing this datapool element
      c_ItElement = this->mc_AllWidgets.find(c_ElementId);

      if (c_ItElement != this->mc_AllWidgets.end())
      {
         const QList<C_PuiSvDbDataElementHandler *> & rc_ListWidgets = c_ItElement.value();
         QList<C_PuiSvDbDataElementHandler *>::const_iterator c_ItWidget;
         const C_OSCNodeDataPoolContent * const pc_ElementContent =
            &this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex)->c_NvmValue;
         // Create the necessary instance with a copy of the content. The timestamp will be filled in the constructor
         C_PuiSvDbDataElementContent c_DbContent(*pc_ElementContent);

         for (c_ItWidget = rc_ListWidgets.begin(); c_ItWidget != rc_ListWidgets.end(); ++c_ItWidget)
         {
            // Update the value
            (*c_ItWidget)->AddNewNvmValueIntoQueue(C_PuiSvDbNodeDataPoolListElementId(c_ElementId,
                                                                                      C_PuiSvDbNodeDataPoolListElementId
                                                                                      ::eDATAPOOL_ELEMENT, false, 0UL),
                                                   c_DbContent);
         }
      }
      else
      {
         stw_scl::C_SCLString c_Info;
         c_Info.PrintFormatted("C_SyvComDataDealer: incoming NVM data with no interested widget " \
                               "(dp: %d List: %d element: %d) !", ou8_DataPoolIndex, ou16_ListIndex,
                               ou16_ElementIndex);
         osc_write_log_info("Asynchronous communication", c_Info);
      }
   }
}
