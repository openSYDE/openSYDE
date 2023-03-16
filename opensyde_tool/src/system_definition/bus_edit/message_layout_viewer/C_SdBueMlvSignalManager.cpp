//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Managing the graphics items for a signal in the message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_SdBueMlvSignalManager.hpp"

#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_SdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
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

   \param[in]     opc_SyncManager         Message sync manager
   \param[in]     orc_MessageId           Message identification indices
   \param[in]     ou16_MaximumCountBits   Maximumt count of bits bounded by DLC
   \param[in]     of64_Space              Space configuration between signals
   \param[in]     opc_Parent              Optional parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::C_SdBueMlvSignalManager(C_PuiSdNodeCanMessageSyncManager * const opc_SyncManager,
                                                 const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint16_t ou16_MaximumCountBits, const float64_t of64_Space,
                                                 QObject * const opc_Parent) :
   QObject(opc_Parent),
   mpc_MessageSyncManager(opc_SyncManager),
   mc_MessageId(orc_MessageId),
   mu16_MaximumCountBits(ou16_MaximumCountBits),
   mu32_SignalIndex(0U),
   mc_Name(""),
   mq_MultiplexerSignal(false),
   mu16_LastBit(0U),
   mq_Resizeable(true),
   mq_Hovered(false),
   mf64_Space(of64_Space),
   ms16_MaximumLength(64),
   ms16_MaximumLengthMultiplexer(16)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::~C_SdBueMlvSignalManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads the signal information and initialize the signal manager and its sub items

   Shall be used first when the signals are connected

   \param[in]     ou32_SignalIndex        Index of the signal
   \param[in]     orc_ColorConfiguration  Color configuration
   \param[in]     oq_Resizeable           Flag if signal shall be resizeable or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::LoadSignal(const uint32_t ou32_SignalIndex,
                                         const C_SignalItemColors & orc_ColorConfiguration, const bool oq_Resizeable)
{
   const C_OscCanSignal * const pc_Signal =
      C_PuiSdHandler::h_GetInstance()->GetCanSignal(this->mc_MessageId, ou32_SignalIndex);
   const stw::opensyde_core::C_OscNodeDataPoolListElement * const pc_DpListElement =
      C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(this->mc_MessageId,
                                                                    ou32_SignalIndex);

   this->mu32_SignalIndex = ou32_SignalIndex;

   this->mc_ColorConfiguration = orc_ColorConfiguration;

   // get and save the signal
   if (pc_Signal != NULL)
   {
      this->mc_Signal = *pc_Signal;
      this->mc_UiSignal.u8_ColorIndex = orc_ColorConfiguration.u8_Index;
   }

   if (pc_DpListElement != NULL)
   {
      // get name from datapool list element
      this->mc_Name = pc_DpListElement->c_Name.c_str();

      if ((pc_DpListElement->c_Value.GetType() == stw::opensyde_core::C_OscNodeDataPoolContent::eFLOAT32) ||
          (pc_DpListElement->c_Value.GetType() == stw::opensyde_core::C_OscNodeDataPoolContent::eFLOAT64))
      {
         this->mq_Resizeable = false;
      }
      else
      {
         this->mq_Resizeable = oq_Resizeable;
      }
   }
   this->mq_MultiplexerSignal = (this->mc_Signal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL);

   // initialize the signal manager
   this->mu16_LastBit = this->GetDataBytesBitPosOfSignalBit(this->mc_Signal.u16_ComBitLength - 1U);
   this->m_UpdateItemConfiguration();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a specific color configuration

   \param[in]     orc_ColorConfiguration  Color configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::SetColorConfiguration(
   const C_SdBueMlvSignalManager::C_SignalItemColors & orc_ColorConfiguration)
{
   int32_t s32_Counter;

   this->mc_ColorConfiguration = orc_ColorConfiguration;
   this->mc_UiSignal.u8_ColorIndex = orc_ColorConfiguration.u8_Index;

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      this->mc_VecSignalItems[s32_Counter]->SetColors(this->mc_ColorConfiguration.c_BackgroundColor,
                                                      this->mc_ColorConfiguration.c_FontColor,
                                                      this->mc_ColorConfiguration.c_ResizeItemColor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the name of the signal

   \param[in]     orc_Name          Name of the signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::SetName(const QString & orc_Name)
{
   this->mc_Name = orc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the position of the first bit of the signal for resizing

   The last bit must be set already.

   \param[in]     ou16_Position      Position of the first bit

   \return
   true  Signal was changed
   false Signal was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalManager::SetStartBit(const uint16_t ou16_Position)
{
   bool q_Return = true;
   const int16_t s16_Diff = static_cast<int16_t>(this->mc_Signal.u16_ComBitStart) - static_cast<int16_t>(ou16_Position);

   if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
   {
      const int16_t s16_Length = static_cast<int16_t>(this->mc_Signal.u16_ComBitLength) + s16_Diff;
      if (this->m_IsLengthValid(s16_Length) == true)
      {
         this->m_SetNewLength(s16_Length);
      }
      else
      {
         q_Return = false;
      }
   }
   else
   {
      // Motorola format has an offset in the difference if more than one row is involved
      const int16_t s16_RowOldPosition = static_cast<int16_t>(this->mc_Signal.u16_ComBitStart) / 8;
      const int16_t s16_RowNewPosition = static_cast<int16_t>(ou16_Position) / 8;
      const int16_t s16_RowDiffOffset = static_cast<int16_t>((s16_RowOldPosition - s16_RowNewPosition) * 16);
      const int16_t s16_Length = static_cast<int16_t>(this->mc_Signal.u16_ComBitLength) -
                                 (s16_Diff - s16_RowDiffOffset);

      if (this->m_IsLengthValid(s16_Length) == true)
      {
         this->m_SetNewLength(s16_Length);
      }
      else
      {
         q_Return = false;
      }
   }

   if (q_Return == true)
   {
      this->mc_Signal.u16_ComBitStart = ou16_Position;

      this->m_UpdateItemConfiguration();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the position of the last bit of the signal for resizing

   The start bit must be set already.

   \param[in]     ou16_Position      Position of the last bit

   \return
   true  Signal was changed
   false Signal was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalManager::SetLastBit(const uint16_t ou16_Position)
{
   bool q_Return = true;
   const int16_t s16_Diff = static_cast<int16_t>(ou16_Position) - static_cast<int16_t>(this->mu16_LastBit);

   if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
   {
      const int16_t s16_Length = static_cast<int16_t>(this->mc_Signal.u16_ComBitLength) + s16_Diff;
      if (this->m_IsLengthValid(s16_Length) == true)
      {
         this->m_SetNewLength(s16_Length);
      }
      else
      {
         q_Return = false;
      }
   }
   else
   {
      // Motorola format has an offset in the difference if more than one row is involved
      const int16_t s16_RowOldPosition = static_cast<int16_t>(this->mu16_LastBit) / 8;
      const int16_t s16_RowNewPosition = static_cast<int16_t>(ou16_Position) / 8;
      const int16_t s16_RowDiffOffset = static_cast<int16_t>((s16_RowOldPosition - s16_RowNewPosition) * 16);
      const int16_t s16_Length = static_cast<int16_t>(this->mc_Signal.u16_ComBitLength) -
                                 (s16_Diff + s16_RowDiffOffset);

      if (this->m_IsLengthValid(s16_Length) == true)
      {
         this->m_SetNewLength(s16_Length);
      }
      else
      {
         q_Return = false;
      }
   }

   if (q_Return == true)
   {
      this->mu16_LastBit = ou16_Position;

      this->m_UpdateItemConfiguration();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the ZOrder value for all visible items

   \param[in]     of64_ZetOrder      New ZOrder value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::SetZeOrder(const float64_t of64_ZetOrder) const
{
   int32_t s32_Counter;

   for (s32_Counter = 0; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      this->mc_VecSignalItems[s32_Counter]->setZValue(of64_ZetOrder);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the ZOrder value for all visible items

   \return
   Actual ZOrder value
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_SdBueMlvSignalManager::GetZeOrder(void) const
{
   float64_t f64_Return = 0.0;

   if (this->mc_VecSignalItems.size() > 0)
   {
      f64_Return = this->mc_VecSignalItems[0]->zValue();
   }

   return f64_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the visualization to show the selection state

   \param[in]     oq_Selected     input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::SetSelected(const bool oq_Selected)
{
   int32_t s32_Counter;

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      this->mc_VecSignalItems[s32_Counter]->SetSelected(oq_Selected);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Moves the signal

   \param[in]     os32_Offset          Move distance

   \return
   true  Signal was moved
   false Signal was not moved
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalManager::MoveSignal(const int32_t os32_Offset)
{
   bool q_Return = false;
   int32_t s32_NewStartBit = static_cast<int32_t>(this->mc_Signal.u16_ComBitStart) + os32_Offset;
   const uint16_t u16_NewLastBit = this->GetDataBytesBitPosOfSignalBit(static_cast<uint16_t>(s32_NewStartBit),
                                                                       this->GetLength() - 1U);

   // special cases for Motorola byte order
   if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_MOTOROLA)
   {
      // avoid jumping of the signal
      // when the start bit is moved over the edge, it must be moved by an offset of 15 instead of 1
      if (((this->mc_Signal.u16_ComBitStart % 8U) == 0) &&
          (os32_Offset == -1))
      {
         s32_NewStartBit = static_cast<int32_t>(this->mc_Signal.u16_ComBitStart) + 15;
      }

      if (((this->mc_Signal.u16_ComBitStart % 8U) == 7) &&
          (os32_Offset == 1))
      {
         s32_NewStartBit = static_cast<int32_t>(this->mc_Signal.u16_ComBitStart) - 15;
      }
   }

   if ((s32_NewStartBit >= 0) &&
       (s32_NewStartBit < this->mu16_MaximumCountBits))
   {
      // new position is in valid range
      // update the item
      this->mc_Signal.u16_ComBitStart = static_cast<uint16_t>(s32_NewStartBit);
      this->mu16_LastBit = u16_NewLastBit;

      this->m_UpdateItemConfiguration();

      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the signal name

   \return
   Name of signal
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMlvSignalManager::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the start bit position

   \return
   Start bit
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvSignalManager::GetStartBit(void) const
{
   return this->mc_Signal.u16_ComBitStart;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the last bit position

   \return
   Last bit
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvSignalManager::GetLastBit(void) const
{
   return this->mu16_LastBit;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the length

   \return
   Length
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvSignalManager::GetLength(void) const
{
   return this->mc_Signal.u16_ComBitLength;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the signal index

   \return
   Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueMlvSignalManager::GetSignalIndex(void) const
{
   return this->mu32_SignalIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the background color

   \return
   Color configuration
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::C_SignalItemColors C_SdBueMlvSignalManager::GetColorConfiguration(void) const
{
   return this->mc_ColorConfiguration;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the interaction mode dependent of the mouse position

   \param[in]  orc_MousePos   Mouse position

   \return
   eIAM_NONE
   eIAM_RESIZELEFT
   eIAM_RESIZERIGHT
   eIAM_MOVE
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::E_InterActionMode C_SdBueMlvSignalManager::GetInteractionMode(const QPointF & orc_MousePos)
const
{
   E_InterActionMode e_Return = eIAM_NONE;
   int32_t s32_Counter;

   // search the item which was clicked
   for (s32_Counter = 0; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      if (this->mc_VecSignalItems[s32_Counter]->ContainsPoint(orc_MousePos) == true)
      {
         // check which part of the item was clicked
         if ((this->mq_Resizeable == true) &&
             (this->mc_VecSignalItems[s32_Counter]->ContainsLeftResizeItemPoint(orc_MousePos) == true))
         {
            if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
            {
               e_Return = eIAM_RESIZELEFT_INTEL;
            }
            else
            {
               e_Return = eIAM_RESIZELEFT_MOTOROLA;
            }
         }
         else if ((this->mq_Resizeable == true) &&
                  (this->mc_VecSignalItems[s32_Counter]->ContainsRightResizeItemPoint(orc_MousePos) == true))
         {
            if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
            {
               e_Return = eIAM_RESIZERIGHT_INTEL;
            }
            else
            {
               e_Return = eIAM_RESIZERIGHT_MOTOROLA;
            }
         }
         else
         {
            e_Return = eIAM_MOVE;
         }

         break;
      }
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns flag if mouse is hovering over signal

   \return
   true   Mouse is hovering above signal
   false  Mouse is not hovering above signal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalManager::IsHovered(void) const
{
   return this->mq_Hovered;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   generate custom tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::GenerateHint(void)
{
   this->SetDefaultToolTipHeading(this->mc_Name);
   this->SetDefaultToolTipContent(C_SdUtil::h_GetToolTipContentSignal(this->mc_MessageId, this->mu32_SignalIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the size and position of all sub items

   \param[in]     of64_SingleItemWidth      New item width
   \param[in]     of64_SingleItemHeight     New item height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::Update(const float64_t of64_SingleItemWidth, const float64_t of64_SingleItemHeight)
{
   QVector<C_SdBueMlvSignalItem *>::const_iterator pc_ItItem;

   // updating the position and size of the items
   for (pc_ItItem = this->mc_VecSignalItems.begin(); pc_ItItem != this->mc_VecSignalItems.end(); ++pc_ItItem)
   {
      (*pc_ItItem)->Update(of64_SingleItemWidth, of64_SingleItemHeight);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deletes all sub items

   Deleting the items not in the destructor. The destructor may be called later when the sub items are deleted
   by their parents already.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::ClearItems(void)
{
   QVector<C_SdBueMlvSignalItem *>::iterator pc_ItItem;

   // remove all signal items from the scene and delete the items
   for (pc_ItItem = this->mc_VecSignalItems.begin(); pc_ItItem != this->mc_VecSignalItems.end(); ++pc_ItItem)
   {
      Q_EMIT (this->SigRemoveItem(*pc_ItItem));
      (*pc_ItItem)->setParentItem(NULL);
      delete (*pc_ItItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the actual state of the signal in its message in the core
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::SaveSignal(void) const
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      this->mpc_MessageSyncManager->SetCanSignalPosition(this->mc_MessageId, this->mu32_SignalIndex, this->mc_Signal,
                                                         this->mc_UiSignal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the position of a signal bit in the data bytes

   \param[in]     ou16_SignalBitPosition   Signal bit position starting at 0

   \return
   Data bytes bit position
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvSignalManager::GetDataBytesBitPosOfSignalBit(const uint16_t ou16_SignalBitPosition) const
{
   return this->mc_Signal.GetDataBytesBitPosOfSignalBit(ou16_SignalBitPosition);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the position of a signal bit in the data bytes

   \param[in]     ou16_StartBit            New start bit for the signal
   \param[in]     ou16_SignalBitPosition   Signal bit position starting at 0

   \return
   Data bytes bit position
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvSignalManager::GetDataBytesBitPosOfSignalBit(const uint16_t ou16_StartBit,
                                                                const uint16_t ou16_SignalBitPosition) const
{
   return this->mc_Signal.GetDataBytesBitPosOfSignalBit(ou16_StartBit, ou16_SignalBitPosition);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the positions of all signal bits in the data bytes

   \param[out]     orc_SetPositions   Signal bit positions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::GetDataBytesBitPositionsOfSignal(std::set<uint16_t> & orc_SetPositions) const
{
   this->mc_Signal.GetDataBytesBitPositionsOfSignal(orc_SetPositions);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::m_UpdateItemConfiguration(void)
{
   C_SdBueMlvSignalItem * pc_Item;
   int32_t s32_Counter = 0;
   bool q_AllRowsVisible = true;

   std::set<uint16_t> c_SetPositions;
   std::set<uint16_t>::iterator c_ItSetPosition;
   std::set<uint16_t>::reverse_iterator c_ItReverseSetPosition;

   // get the positions of the signal
   this->GetDataBytesBitPositionsOfSignal(c_SetPositions);

   // remove all position which are not in the usable area (because of DLC)
   while (c_SetPositions.size() > 0)
   {
      // start the search at the end with the highest elements
      c_ItReverseSetPosition = c_SetPositions.rbegin();

      if ((*c_ItReverseSetPosition) >= this->mu16_MaximumCountBits)
      {
         // the erase function needs the base iterator, but the base iterator has an offset of -1
         std::advance(c_ItReverseSetPosition, 1);
         c_SetPositions.erase(c_ItReverseSetPosition.base());
         q_AllRowsVisible = false;
      }
      else
      {
         break;
      }
   }

   this->m_SetError(!q_AllRowsVisible);

   // updating the number of items
   while (c_SetPositions.size() > 0)
   {
      uint16_t u16_StartBitInRow;
      uint16_t u16_LastBitInRow;
      uint16_t u16_BitPosition;
      uint16_t u16_ByteRow;

      if ((s32_Counter + 1) <= this->mc_VecSignalItems.size())
      {
         // reuse item
         pc_Item = this->mc_VecSignalItems[s32_Counter];
      }
      else
      {
         QString c_ShownText = this->mc_Name;

         if (this->mq_MultiplexerSignal == true)
         {
            c_ShownText += C_GtGetText::h_GetText(" (Multiplexer)");
         }

         // new item necessary
         pc_Item = new C_SdBueMlvSignalItem(this->mc_ColorConfiguration.c_BackgroundColor,
                                            this->mc_ColorConfiguration.c_FontColor,
                                            this->mc_ColorConfiguration.c_ResizeItemColor, c_ShownText,
                                            this->mf64_Space);
         connect(pc_Item, &C_SdBueMlvSignalItem::SigItemHovered, this, &C_SdBueMlvSignalManager::m_SignalItemHovered);
         connect(pc_Item, &C_SdBueMlvSignalItem::SigChangeCursor, this, &C_SdBueMlvSignalManager::SigChangeCursor);
         this->mc_VecSignalItems.push_back(pc_Item);
         // add the item to the scene
         Q_EMIT (this->SigAddItem(pc_Item));
      }

      // get the first bit in this row
      c_ItSetPosition = c_SetPositions.begin();
      u16_StartBitInRow = *c_ItSetPosition % 8U;
      u16_ByteRow = *c_ItSetPosition / 8U;
      // save the raw bit position for the compare in the next loop
      u16_BitPosition = *c_ItSetPosition;
      c_SetPositions.erase(c_ItSetPosition);

      // if no further bit is available in this row, the start bit is the last bit in this row too
      // it will be used for the compare too
      u16_LastBitInRow = u16_StartBitInRow;

      // search the last bit in this row
      while (c_SetPositions.size() > 0)
      {
         c_ItSetPosition = c_SetPositions.begin();

         if (((*c_ItSetPosition - 1U) == u16_BitPosition) &&
             ((*c_ItSetPosition % 8U) > 0U))
         {
            // the bit is in the same row
            u16_LastBitInRow = *c_ItSetPosition % 8U;
            u16_BitPosition = *c_ItSetPosition;
            c_SetPositions.erase(c_ItSetPosition);
         }
         else
         {
            // the bit is in the next row. do not erase the bit from the set for the next row
            // the previous bit is the last bit of this row
            break;
         }
      }

      // set the calcuated bit positions for the item
      pc_Item->SetBitPosition(u16_ByteRow, u16_StartBitInRow, u16_LastBitInRow);
      pc_Item->SetHovered(this->mq_Hovered);

      ++s32_Counter;
   }

   // delete not needed obsolete items
   while (s32_Counter < this->mc_VecSignalItems.size())
   {
      // remove the item from the scene
      Q_EMIT (this->SigRemoveItem(this->mc_VecSignalItems[s32_Counter]));
      disconnect(this->mc_VecSignalItems[s32_Counter], &C_SdBueMlvSignalItem::SigItemHovered,
                 this, &C_SdBueMlvSignalManager::m_SignalItemHovered);
      disconnect(this->mc_VecSignalItems[s32_Counter], &C_SdBueMlvSignalItem::SigChangeCursor, this,
                 &C_SdBueMlvSignalManager::SigChangeCursor);
      delete this->mc_VecSignalItems[s32_Counter];
      this->mc_VecSignalItems.remove(s32_Counter);
   }

   // configure order dependent settings in the signal items
   if ((this->mc_VecSignalItems.size() > 1) ||
       ((this->mc_VecSignalItems.size() > 0) && (q_AllRowsVisible == false)))
   {
      // items at the start and end
      if (this->mc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
      {
         // resize icons and configure significant bit information
         this->mc_VecSignalItems[0]->SetResizeItem(false, true, this->mq_Resizeable);
         this->mc_VecSignalItems[0]->SetShowSignificantBit(false, true);

         if (q_AllRowsVisible == true)
         {
            // only if the complete signal is visible
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetResizeItem(true, false,
                                                                                       this->mq_Resizeable);
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetShowSignificantBit(true, false);
         }
         else if (this->mc_VecSignalItems.size() > 1)
         {
            // in this case the last item is only the last visible item, but not the real last item
            // this would be below the inactive items (because of DLC)
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetResizeItem(false, false,
                                                                                       this->mq_Resizeable);
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetShowSignificantBit(false, false);
         }
         else
         {
            // nothing to do
         }
      }
      else
      {
         // resize icons and configure significant bit information
         this->mc_VecSignalItems[0]->SetResizeItem(true, false, this->mq_Resizeable);
         this->mc_VecSignalItems[0]->SetShowSignificantBit(true, false);

         if (q_AllRowsVisible == true)
         {
            // only if the complete signal is visible
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetResizeItem(false, true,
                                                                                       this->mq_Resizeable);
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetShowSignificantBit(false, true);
         }
         else if (this->mc_VecSignalItems.size() > 1)
         {
            // in this case the last item is only the last visible item, but not the real last item
            // this would be below the inactive items (because of DLC)
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetResizeItem(false, false,
                                                                                       this->mq_Resizeable);
            this->mc_VecSignalItems[this->mc_VecSignalItems.size() - 1]->SetShowSignificantBit(false, false);
         }
         else
         {
            // nothing to do
         }
      }

      // items in the "middle"
      for (s32_Counter = 1; s32_Counter < (this->mc_VecSignalItems.size() - 1); ++s32_Counter)
      {
         this->mc_VecSignalItems[s32_Counter]->SetResizeItem(false, false, this->mq_Resizeable);
         this->mc_VecSignalItems[s32_Counter]->SetShowSignificantBit(false, false);
      }
   }
   else if (this->mc_VecSignalItems.size() == 1)
   {
      this->mc_VecSignalItems[0]->SetResizeItem(true, true, this->mq_Resizeable);
      if (this->mc_Signal.u16_ComBitLength > 1U)
      {
         this->mc_VecSignalItems[0]->SetShowSignificantBit(true, true);
      }
      else
      {
         this->mc_VecSignalItems[0]->SetShowSignificantBit(false, false);
      }
   }
   else
   {
      // nothing to do
   }

   // update the error state
   this->m_SetError(!q_AllRowsVisible);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::m_SignalItemHovered(const bool oq_Hover)
{
   int32_t s32_Counter;

   this->mq_Hovered = oq_Hover;

   if (this->mq_Hovered == false)
   {
      Q_EMIT this->SigHideToolTip();
   }

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      this->mc_VecSignalItems[s32_Counter]->SetHovered(oq_Hover);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::m_SetNewLength(const int16_t os16_Lenth)
{
   if (os16_Lenth > 0)
   {
      this->mc_Signal.u16_ComBitLength = static_cast<uint16_t>(os16_Lenth);
   }
   else
   {
      this->mc_Signal.u16_ComBitLength = 1U;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvSignalManager::m_SetError(const bool oq_Active)
{
   int32_t s32_Counter;

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignalItems.size(); ++s32_Counter)
   {
      this->mc_VecSignalItems[s32_Counter]->SetError(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks new length of signal

   In case of a multiplexer signal, the signal shall not be longer than ms16_MaximumLengthMultiplexer. If it
   is a non multiplexed or a multiplexed signal, the signal shall not be longer than ms16_MaximumLength

   \param[in]       os16_Length     New length of signal

   \retval   true            Length is valid
   \retval   false           Length is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvSignalManager::m_IsLengthValid(const int16_t os16_Length) const
{
   bool q_Return = false;

   if ((os16_Length <= ms16_MaximumLength) &&
       ((this->mq_MultiplexerSignal == false) || (os16_Length <= ms16_MaximumLengthMultiplexer)))
   {
      q_Return = true;
   }

   return q_Return;
}
