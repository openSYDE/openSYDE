//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connects the openSYDE protocol driver to a node with GUI specific functionality

   - Depending on the type of data pool element call the
     corresponding read function from C_OscDiagProtocolBase
   - Place the results into the defined data pool element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_SyvComDataDealer.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSvDbDataElementContent.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
   C_OscDataDealerNvmSafe()
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
C_SyvComDataDealer::C_SyvComDataDealer(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                                       C_OscDiagProtocolBase * const opc_DiagProtocol) :
   C_OscDataDealerNvmSafe(opc_Node, ou32_NodeIndex, opc_DiagProtocol)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDataDealer::~C_SyvComDataDealer(void) noexcept
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
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < opc_Widget->GetWidgetDataPoolElementCount(); ++u32_Counter)
      {
         C_PuiSvDbNodeDataPoolListElementId c_DpElementId;
         if (opc_Widget->GetDataPoolElementIndex(u32_Counter, c_DpElementId) == C_NO_ERR)
         {
            // Is it relevant for this data dealer (No handling of bus signals here)?
            if ((c_DpElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT) &&
                (c_DpElementId.u32_NodeIndex == this->GetNodeIndex()))
            {
               QMap<C_OscNodeDataPoolListElementId,
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

   Calls DataPoolRead of C_OscDataDealer and updates the registered widgets.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[in]     opc_DashboardWidget  Optional pointer to dashboard widget data element
                                             Valid pointer: read value only for this widget with this Datapool element
                                             NULL pointer:  read value for all widgets with this Datapool element
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
int32_t C_SyvComDataDealer::DataPoolReadWithWidget(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                   const uint16_t ou16_ElementIndex,
                                                   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DashboardWidget,
                                                   uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   s32_Return = C_OscDataDealer::DataPoolRead(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, opu8_NrCode);

   if (s32_Return == C_NO_ERR)
   {
      if (opc_DashboardWidget == NULL)
      {
         this->m_OnReadDataPoolEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      }
      else
      {
         this->m_OnReadDataPoolEventReceivedForWidget(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                                      opc_DashboardWidget);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's data pool and updates the associated widgets

   Calls DataPoolRead of C_OscDataDealer and updates the registered widgets.

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
int32_t C_SyvComDataDealer::DataPoolRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                         const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   s32_Return = C_OscDataDealer::DataPoolRead(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, opu8_NrCode);

   if (s32_Return == C_NO_ERR)
   {
      this->m_OnReadDataPoolEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's NVM and updates the associated widgets

   Calls NvmRead of C_OscDataDealer and updates the registered widgets.

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
int32_t C_SyvComDataDealer::NvmRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                    const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   s32_Return = C_OscDataDealer::NvmRead(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, opu8_NrCode);

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
int32_t C_SyvComDataDealer::NvmReadList(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                        uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   s32_Return = C_OscDataDealerNvm::NvmReadList(ou32_DataPoolIndex, ou32_ListIndex, opu8_NrCode);

   //Allow checksum errors (handled by param widget)
   if ((s32_Return == C_NO_ERR) || (s32_Return == C_CHECKSUM))
   {
      uint32_t u32_ElementCounter;
      const C_OscNodeDataPoolList & rc_List = this->mpc_Node->c_DataPools[ou32_DataPoolIndex].c_Lists[ou32_ListIndex];

      for (u32_ElementCounter = 0U; u32_ElementCounter < rc_List.c_Elements.size(); ++u32_ElementCounter)
      {
         this->m_OnReadDataPoolNvmEventReceived(static_cast<uint8_t>(ou32_DataPoolIndex),
                                                static_cast<uint16_t>(ou32_ListIndex),
                                                static_cast<uint16_t>(u32_ElementCounter));
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Short function description

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[in]     opc_DashboardWidget  Optional pointer to dashboard widget data element
                                             Valid pointer: read value only for this widget with this Datapool element
                                             NULL pointer:  read value for all widgets with this Datapool element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDataDealer::m_OnReadDataPoolEventReceivedForWidget(const uint8_t ou8_DataPoolIndex,
                                                                const uint16_t ou16_ListIndex,
                                                                const uint16_t ou16_ElementIndex,
                                                                C_PuiSvDbDataElementHandler * const opc_DashboardWidget)
{
   if ((this->mpc_Node != NULL) &&
       (opc_DashboardWidget != NULL))
   {
      const C_OscNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                       ou16_ElementIndex);
      const C_OscNodeDataPoolContent * const pc_ElementContent =
         &this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex)->c_Value;
      // Create the necessary instance with a copy of the content. The timestamp will be filled in the constructor
      const C_PuiSvDbDataElementContent c_DbContent(*pc_ElementContent);

      // Update the value
      opc_DashboardWidget->InsertNewValueIntoQueue(C_PuiSvDbNodeDataPoolListElementId(c_ElementId,
                                                                                      C_PuiSvDbNodeDataPoolListElementId
                                                                                      ::eDATAPOOL_ELEMENT, false, 0UL),
                                                   c_DbContent);
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvComDataDealer::m_OnReadDataPoolEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                       const uint16_t ou16_ElementIndex)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OscNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      const C_OscNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                       ou16_ElementIndex);

      // Get the correct list with all pointers to the widgets which are showing this datapool element
      c_ItElement = this->mc_AllWidgets.find(c_ElementId);

      if (c_ItElement != this->mc_AllWidgets.end())
      {
         const QList<C_PuiSvDbDataElementHandler *> & rc_ListWidgets = c_ItElement.value();
         QList<C_PuiSvDbDataElementHandler *>::const_iterator c_ItWidget;
         const C_OscNodeDataPoolContent * const pc_ElementContent =
            &this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex)->c_Value;
         // Create the necessary instance with a copy of the content. The timestamp will be filled in the constructor
         const C_PuiSvDbDataElementContent c_DbContent(*pc_ElementContent);

         for (c_ItWidget = rc_ListWidgets.begin(); c_ItWidget != rc_ListWidgets.end(); ++c_ItWidget)
         {
            // Update only read items here
            if ((*c_ItWidget)->IsReadItem() == true)
            {
               // Update the value
               (*c_ItWidget)->InsertNewValueIntoQueue(C_PuiSvDbNodeDataPoolListElementId(c_ElementId,
                                                                                         C_PuiSvDbNodeDataPoolListElementId
                                                                                         ::eDATAPOOL_ELEMENT, false,
                                                                                         0UL),
                                                      c_DbContent);
            }
         }
      }
      else
      {
         stw::scl::C_SclString c_Info;
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
void C_SyvComDataDealer::m_OnReadDataPoolEventErrorReceived(const uint8_t ou8_DataPoolIndex,
                                                            const uint16_t ou16_ListIndex,
                                                            const uint16_t ou16_ElementIndex,
                                                            const uint8_t ou8_ErrorCode)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OscNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      const C_OscNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
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
         C_OscDataDealerNvmSafe::m_OnReadDataPoolEventErrorReceived(ou8_DataPoolIndex, ou16_ListIndex,
                                                                    ou16_ElementIndex, ou8_ErrorCode);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvComDataDealer::m_OnReadDataPoolNvmEventReceived(const uint8_t ou8_DataPoolIndex,
                                                          const uint16_t ou16_ListIndex,
                                                          const uint16_t ou16_ElementIndex)
{
   if (this->mpc_Node != NULL)
   {
      QMap<C_OscNodeDataPoolListElementId,
           QList<C_PuiSvDbDataElementHandler *> >::const_iterator c_ItElement;
      const C_OscNodeDataPoolListElementId c_ElementId(this->mu32_NodeIndex, ou8_DataPoolIndex, ou16_ListIndex,
                                                       ou16_ElementIndex);

      // Get the correct list with all pointers to the widgets which are showing this datapool element
      c_ItElement = this->mc_AllWidgets.find(c_ElementId);

      if (c_ItElement != this->mc_AllWidgets.end())
      {
         const QList<C_PuiSvDbDataElementHandler *> & rc_ListWidgets = c_ItElement.value();
         QList<C_PuiSvDbDataElementHandler *>::const_iterator c_ItWidget;
         const C_OscNodeDataPoolContent * const pc_ElementContent =
            &this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex)->c_NvmValue;
         // Create the necessary instance with a copy of the content. The timestamp will be filled in the constructor
         const C_PuiSvDbDataElementContent c_DbContent(*pc_ElementContent);

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
         stw::scl::C_SclString c_Info;
         c_Info.PrintFormatted("C_SyvComDataDealer: incoming NVM data with no interested widget " \
                               "(dp: %d List: %d element: %d) !", ou8_DataPoolIndex, ou16_ListIndex,
                               ou16_ElementIndex);
         osc_write_log_info("Asynchronous communication", c_Info);
      }
   }
}
