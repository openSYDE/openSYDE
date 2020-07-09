//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance model (implementation)

   Max performance model

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <sstream>
#include <iomanip>

#include <QBitArray>

#include "TGLTime.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_CamMetUtil.h"
#include "cam_constants.h"
#include "CCMONProtocol.h"
#include "C_CamMetTreeModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CamMetTreeModel::mhc_IconMessage = ":images/IconMessageInactive.svg";
const QString C_CamMetTreeModel::mhc_IconMessageSelected = "://images/IconMessageSelected.svg";
const QString C_CamMetTreeModel::mhc_IconSignal = ":images/IconSignalInactive.svg";
const QString C_CamMetTreeModel::mhc_IconSignalSelected = "://images/IconSignalSelected.svg";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeModel::C_CamMetTreeModel(QObject * const opc_Parent) :
   C_TblTreSimpleModel(opc_Parent),
   mq_DisplayTree(false),
   mq_UniqueMessageMode(false),
   mq_DisplayAsHex(false),
   mq_DisplayTimestampRelative(false),
   mq_DisplayTimestampAbsoluteTimeOfDay(false),
   mu32_TraceBufferSizeUsed(1000U),
   mu32_TraceBufferSizeConfig(1000U),
   mq_DataUnlocked(false),
   mu32_OldestItemIndex(0),
   ms32_SelectedParentRow(-1),
   mq_GrayOutPause(false),
   mu32_GrayOutPauseTimeStamp(0U),
   mpc_RootItemContinuous(new C_TblTreSimpleItem()),
   mpc_RootItemStatic(new C_TblTreSimpleItem()),
   msn_LastSearchedMessageRow(-1),
   msn_LastSearchedSignalRow(-1),
   msn_LastSearchedSignalRowMultiplexed(-1),
   mq_IsLastSearchedSignalLastOfMessage(true)
{
   sintn sn_Counter;

   this->m_AdaptTraceBufferSize();

   // Prepare the transparency colors for the gray out font
   this->mc_FontTransparcencyColors.resize((msn_TRACE_TRANSPARENCY_START - msn_TRACE_TRANSPARENCY_END) + 1,
                                           mc_STYLE_GUIDE_COLOR_1);

   // Value on position 0 equals no transparency and at the end equals maximum transparency
   for (sn_Counter = 0U; sn_Counter < static_cast<sintn>(this->mc_FontTransparcencyColors.size()); ++sn_Counter)
   {
      this->mc_FontTransparcencyColors[sn_Counter].setAlpha(msn_TRACE_TRANSPARENCY_START - sn_Counter);
   }

   //Default
   this->mpc_InvisibleRootItem = this->mpc_RootItemContinuous;

   connect(&this->mc_GrayOutTimer, &QTimer::timeout, this, &C_CamMetTreeModel::m_GrayOutTimer);
   this->mc_GrayOutTimer.start(msn_TRACE_TRANSPARENCY_REFRESH_TIME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeModel::~C_CamMetTreeModel(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continues the gray out engine

   Calculation of an offset to all saved unique messages timestamps to compensate the pause time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Continue(void)
{
   // ms resolution is enough
   // Using US as base because we want to compare with the US value of the CAN message.
   // The TGL_GetTickCountUS can have a difference of 1 to 2 seconds because of a different rounding
   const uint32 u32_CurrentTime = static_cast<uint32>(TGL_GetTickCountUS() / 1000ULL);
   // Get the time interval of the entire pause
   const stw_types::uint32 u32_GrayOutPauseOffset = u32_CurrentTime - this->mu32_GrayOutPauseTimeStamp;

   QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_It;

   // Adapt all timestamps used by the gray out engine
   for (c_It = this->mc_UniqueMessages.begin(); c_It != this->mc_UniqueMessages.end(); ++c_It)
   {
      C_CamMetTreeLoggerData & rc_Data = c_It.value();
      sintn sn_CounterDataByte;

      // Timestamp of CAN message
      // The original timestamp of the message can be adapted, it used only for the gray out engine
      // For showing the timestamp, the prepared and already calculated timestamps will be used
      rc_Data.c_CanMsg.u64_TimeStamp += static_cast<uint64>(u32_GrayOutPauseOffset) * 1000ULL;

      // Timestamp of all data bytes
      rc_Data.c_GreyOutInformation.u32_DataChangedTimeStamp += u32_GrayOutPauseOffset;

      // Timestamp of each data byte
      for (sn_CounterDataByte = 0; sn_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++sn_CounterDataByte)
      {
         rc_Data.c_GreyOutInformation.c_DataBytesChangedTimeStamps[sn_CounterDataByte] += u32_GrayOutPauseOffset;
      }
      // Update all timestamps of multiplexer map
      for (std::map<stw_types::sint32, stw_types::uint32>::iterator c_ItVal =
              rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.begin();
           c_ItVal != rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.end(); ++c_ItVal)
      {
         c_ItVal->second += u32_GrayOutPauseOffset;
      }
   }

   this->mq_GrayOutPause = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pauses the gray out engine
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Pause(void)
{
   this->mq_GrayOutPause = true;

   // ms resolution is enough
   // Using US as base because we want to compare with the US value of the CAN message.
   // The TGL_GetTickCountUS can have a difference of 1 to 2 seconds because of a different rounding
   this->mu32_GrayOutPauseTimeStamp = static_cast<uint32>(TGL_GetTickCountUS() / 1000ULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pauses the gray out engine

   Calculation of the offset is not necessary. Stop can only be followed by start
   and start resets the offset in any case.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Stop(void)
{
   this->mq_GrayOutPause = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continues the gray out engine and resets the pause offset

   The offset is not necessary on a restart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Start(void)
{
   this->mq_GrayOutPause = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to lock up all data requests (performance)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::LockData(void)
{
   this->mq_DataUnlocked = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to unlock up all data requests (performance)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::UnlockData(void)
{
   this->mq_DataUnlocked = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle action: clear data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::ActionClearData(void)
{
   this->beginResetModel();
   this->mu32_OldestItemIndex = 0UL;
   this->mc_DataBase.clear();
   this->mc_UniqueMessages.clear();
   this->mc_UniqueMessagesOrdering.clear();
   //Tree
   this->mpc_RootItemContinuous->ClearChildren();
   this->mpc_RootItemStatic->ClearChildren();
   //Every reset will clear the selection
   this->SetSelection(-1, -1);
   this->endResetModel();

   if (this->mu32_TraceBufferSizeUsed != this->mu32_TraceBufferSizeConfig)
   {
      this->m_AdaptTraceBufferSize();
   }

   this->m_ResetSearch();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified strings as rows

   \param[in]  orc_Data    New row content

   \return
   Indices of added rows (only valid if not in unique message mode)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_CamMetTreeModel::AddRows(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   const std::vector<sint32> c_Retval = this->m_AddRowsContinuousMode(orc_Data);

   this->m_AddRowsUnique(orc_Data);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current selection

   \param[in]  os32_SelectedParentRow     Current selected parent index
   \param[in]  os32_SelectedChildIndex    Current selected child index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetSelection(const sint32 os32_SelectedParentRow, const sint32 os32_SelectedChildIndex)
{
   const sint32 s32_ColIDData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   this->ms32_SelectedParentRow = os32_SelectedParentRow;
   this->mc_SelectedChildBytes.clear();
   if (os32_SelectedParentRow >= 0)
   {
      const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(os32_SelectedParentRow);

      if (pc_CurMessage != NULL)
      {
         //Check which child/signal should be selected
         const uint32 u32_ChildIndex = static_cast<uint32>(os32_SelectedChildIndex);

         //Check if the signal is valid
         if (u32_ChildIndex < pc_CurMessage->c_Signals.size())
         {
            std::set<uint16> c_Bits;
            const C_OSCComMessageLoggerDataSignal & rc_CurSignal = pc_CurMessage->c_Signals[u32_ChildIndex];
            //Get all bit positions as intel
            rc_CurSignal.c_OscSignal.GetDataBytesBitPositionsOfSignal(c_Bits);
            //Convert bit positions into byte indices
            for (std::set<uint16>::const_iterator c_ItBit = c_Bits.begin(); c_ItBit != c_Bits.end();
                 ++c_ItBit)
            {
               this->mc_SelectedChildBytes.insert(*c_ItBit / 8U);
            }
         }
      }
   }
   //Update data row
   this->dataChanged(this->index(0, s32_ColIDData), this->index(this->rowCount() - 1, s32_ColIDData));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display mode: display tree

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayTree(const bool oq_Value)
{
   //Avoid resetting the model if not necessary (avoid collapsing all items)
   if (this->mq_DisplayTree != oq_Value)
   {
      this->beginResetModel();
      this->mq_DisplayTree = oq_Value;
      //Every reset will clear the selection
      this->SetSelection(-1, -1);
      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display mode: display unique messages

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayUniqueMessages(const bool oq_Value)
{
   this->beginResetModel();
   this->mq_UniqueMessageMode = oq_Value;
   //Every reset will clear the selection
   this->SetSelection(-1, -1);
   //Tree
   if (this->mq_UniqueMessageMode)
   {
      this->mpc_InvisibleRootItem = this->mpc_RootItemStatic;
   }
   else
   {
      this->mpc_InvisibleRootItem = this->mpc_RootItemContinuous;
   }
   this->endResetModel();

   this->m_ResetSearch();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for CAN ID and CAN data

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayAsHex(const bool oq_Value)
{
   const sint32 s32_ColCanID = C_CamMetTreeModel::h_EnumToColumn(eCAN_ID);
   const sint32 s32_ColDataID = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   //Update value
   this->mq_DisplayAsHex = oq_Value;

   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColCanID), this->index(this->rowCount() - 1, s32_ColCanID));
   Q_EMIT this->dataChanged(this->index(0, s32_ColDataID), this->index(this->rowCount() - 1, s32_ColDataID));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for timestamp

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayTimestampRelative(const bool oq_Value)
{
   const sint32 s32_ColID = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);

   //Update value
   this->mq_DisplayTimestampRelative = oq_Value;
   //Update UI
   Q_EMIT (this->dataChanged(this->index(0, s32_ColID), this->index(this->rowCount() - 1, s32_ColID)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for absolute timestamp

   If relative timestamp is active, this mode does not change the trace

   \param[in]  oq_Value    New value
                           true: Timestamp with time of day
                           false: Timestamp beginning at start of measurement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayTimestampAbsoluteTimeOfDay(const bool oq_Value)
{
   //Update value
   this->mq_DisplayTimestampAbsoluteTimeOfDay = oq_Value;

   if (this->mq_DisplayTimestampRelative == false)
   {
      const sint32 s32_ColID = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      //Update UI
      Q_EMIT (this->dataChanged(this->index(0, s32_ColID), this->index(this->rowCount() - 1, s32_ColID)));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for CAN ID and CAN data

   \return
   true     Displaying hex
   false    Displaying dec
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::GetDisplayAsHex(void) const
{
   return this->mq_DisplayAsHex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for timestamp

   \return
   true     Displaying relative timestamp
   false    Displaying absolute timestamp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::GetDisplayTimestampRelative(void) const
{
   return this->mq_DisplayTimestampRelative;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for absolute timestamp

   \return
   true     Displaying absolute timestamp with time of day
   false    Displaying absolute timestamp beginning at start of measurement
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::GetDisplayTimestampAbsoluteTimeOfDay(void) const
{
   return this->mq_DisplayTimestampAbsoluteTimeOfDay;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets a new size of the trace buffer

   This will affect the maximum shown messages in the trace view.

   The new size will be used on the next call of function ActionClearData.

   \param[in]       ou32_Value     New size of trace buffer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetTraceBufferSize(const uint32 ou32_Value)
{
   this->mu32_TraceBufferSizeConfig = ou32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all messages

   Intended only for protocol changes

   \return
   All messages
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CamMetTreeLoggerData *> C_CamMetTreeModel::GetAllMessagesForProtocolChange(void)
{
   std::vector<C_CamMetTreeLoggerData *> c_Retval;
   const sintn sn_CompleteSize = static_cast<sintn>(this->mc_DataBase.size()) + this->mc_UniqueMessages.size();
   //Reserve
   c_Retval.reserve(sn_CompleteSize);
   //Append vector
   for (uint32 u32_ItVec = 0; u32_ItVec < this->mc_DataBase.size(); ++u32_ItVec)
   {
      c_Retval.push_back(&this->mc_DataBase[u32_ItVec]);
   }
   //Append map
   for (QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_It = this->mc_UniqueMessages.begin();
        c_It != this->mc_UniqueMessages.end(); ++c_It)
   {
      c_Retval.push_back(&c_It.value());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal a change of protocol related data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SignalProtocolChange(void)
{
   const sint32 s32_ColID = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColID), this->index(this->rowCount() - 1, s32_ColID));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamMetTreeModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                       const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))

      {
         switch (e_Col)
         {
         case eTIME_STAMP:
            c_Retval = C_GtGetText::h_GetText("Time");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("ID");
            break;
         case eCAN_NAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCAN_DIR:
            c_Retval = C_GtGetText::h_GetText("Dir");
            break;
         case eCAN_DLC:
            c_Retval = C_GtGetText::h_GetText("DLC");
            break;
         case eCAN_DATA:
            c_Retval = C_GtGetText::h_GetText("Data");
            break;
         case eCAN_COUNTER:
            c_Retval = C_GtGetText::h_GetText("Counter");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eTIME_STAMP:
            c_Retval = C_GtGetText::h_GetText("Time");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("Identifier");
            break;
         case eCAN_NAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCAN_DIR:
            c_Retval = C_GtGetText::h_GetText("Direction");
            break;
         case eCAN_DLC:
            c_Retval = C_GtGetText::h_GetText("Data Length Code");
            break;
         case eCAN_DATA:
            c_Retval = C_GtGetText::h_GetText("Data");
            break;
         case eCAN_COUNTER:
            c_Retval = C_GtGetText::h_GetText("Counter");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eTIME_STAMP:
            if (this->mq_DisplayTimestampAbsoluteTimeOfDay == false)
            {
               c_Retval = C_GtGetText::h_GetText("Absolute time (hh:mm:ss.ms.us) from the measurement start, or relative "
                                                 "to the previous event.");
            }
            else
            {
               c_Retval = C_GtGetText::h_GetText("Absolute time (hh:mm:ss.ms.us) with time of day, or relative "
                                                 "to the previous event.");
            }
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN identifier of the message.");
            break;
         case eCAN_NAME:
            c_Retval = C_GtGetText::h_GetText("Symbolic name of the CAN message. Empty if not defined in one of the "
                                              "active database(s).");
            break;
         case eCAN_DIR:
            c_Retval = C_GtGetText::h_GetText("Direction of the CAN message.\n"
                                              "   - Rx: Received message\n"
                                              "   - Tx: Sent by this instance of openSYDE CAN Monitor");
            break;
         case eCAN_DLC:
            c_Retval = C_GtGetText::h_GetText("Number of data bytes in decimal representation.");
            break;
         case eCAN_DATA:
            c_Retval = C_GtGetText::h_GetText("CAN message data in bytes.");
            break;
         case eCAN_COUNTER:
            c_Retval = C_GtGetText::h_GetText("Indicates the number of times the event has appeared since "
                                              "measurement start.");
            break;
         default:
            break;
         }
      }
      else
      {
         //No handling necessary
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data index

   \param[in]  osn_Row     Row
   \param[in]  osn_Column  Column
   \param[in]  orc_Parent  Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMetTreeModel::index(const sintn osn_Row, const sintn osn_Column, const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (this->mq_DisplayTree)
   {
      c_Retval = C_TblTreSimpleModel::index(osn_Row, osn_Column, orc_Parent);
   }
   else
   {
      c_Retval = this->createIndex(osn_Row, osn_Column);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get parent index

   \param[in]  orc_Index   Index

   \return
   Parent index (may be invalid = root level)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMetTreeModel::parent(const QModelIndex & orc_Index) const
{
   QModelIndex c_Retval;

   if (this->mq_DisplayTree)
   {
      c_Retval = C_TblTreSimpleModel::parent(orc_Index);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMetTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (orc_Parent.isValid() == false)
   {
      //Top level
      sn_Retval = 7;
   }
   else if (orc_Parent.parent().isValid() == false)
   {
      sn_Retval = 1;
   }
   else if (orc_Parent.parent().parent().isValid() == false)
   {
      sn_Retval = 1;
   }
   else
   {
      // Nothing to do
   }

   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMetTreeModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (this->mq_DisplayTree == true)
   {
      sn_Retval = C_TblTreSimpleModel::rowCount(orc_Parent);
   }
   else if (orc_Parent.isValid() == false)
   {
      //Top level
      if (this->mq_UniqueMessageMode == true)
      {
         sn_Retval = this->mc_UniqueMessages.size();
      }
      else
      {
         sn_Retval = this->mc_DataBase.size();
      }
   }
   else
   {
      // Nothing to do
   }

   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamMetTreeModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if ((mq_DataUnlocked == true) || (osn_Role == msn_USER_ROLE_SORT))
   {
      if (((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == msn_USER_ROLE_MARKER_TEXT)) ||
          (osn_Role == msn_USER_ROLE_SORT))
      {
         if (orc_Index.parent().isValid() == false)
         {
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Index.row());
            if (pc_CurMessage != NULL)
            {
               QString c_Text;
               const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
               switch (e_Col)
               {
               case eTIME_STAMP:
                  if (this->mq_DisplayTimestampRelative == true)
                  {
                     c_Retval = pc_CurMessage->c_TimeStampRelative.c_str();
                  }
                  else if (this->mq_DisplayTimestampAbsoluteTimeOfDay == false)
                  {
                     c_Retval = pc_CurMessage->c_TimeStampAbsoluteStart.c_str();
                  }
                  else
                  {
                     c_Retval = pc_CurMessage->c_TimeStampAbsoluteTimeOfDay.c_str();
                  }
                  break;
               case eCAN_ID:
                  if (osn_Role == msn_USER_ROLE_SORT)
                  {
                     //For number the display style is irrelevant
                     c_Retval = pc_CurMessage->c_CanIdDec.ToInt();
                  }
                  else
                  {
                     if (this->mq_DisplayAsHex == true)
                     {
                        c_Retval = pc_CurMessage->c_CanIdHex.c_str();
                     }
                     else
                     {
                        c_Retval = pc_CurMessage->c_CanIdDec.c_str();
                     }
                  }
                  break;
               case eCAN_NAME:
                  c_Retval = pc_CurMessage->c_Name.c_str();
                  break;
               case eCAN_DIR:
                  if (pc_CurMessage->q_IsTx == true)
                  {
                     c_Text = C_GtGetText::h_GetText("Tx");
                  }
                  else
                  {
                     c_Text = C_GtGetText::h_GetText("Rx");
                  }

                  if (pc_CurMessage->c_CanMsg.u8_RTR > 0U)
                  {
                     c_Text += C_GtGetText::h_GetText(" (RTR)");
                  }

                  c_Retval = c_Text;
                  break;
               case eCAN_DLC:
                  c_Retval = pc_CurMessage->c_CanDlc.c_str();
                  break;
               case eCAN_DATA:
                  if (pc_CurMessage->c_ProtocolTextDec == "")
                  {
                     if ((pc_CurMessage->c_CanMsg.u8_RTR == 0U) &&
                         ((orc_Index.data(msn_USER_ROLE_MARKER).toBitArray().isEmpty() == true) ||
                          (osn_Role == msn_USER_ROLE_MARKER_TEXT)))
                     {
                        if (this->mq_DisplayAsHex == true)
                        {
                           c_Retval = pc_CurMessage->c_CanDataHex.c_str();
                        }
                        else
                        {
                           c_Retval = pc_CurMessage->c_CanDataDec.c_str();
                        }
                     }
                  }
                  else
                  {
                     if (this->mq_DisplayAsHex == true)
                     {
                        c_Retval = pc_CurMessage->c_ProtocolTextHex.c_str();
                     }
                     else
                     {
                        c_Retval = pc_CurMessage->c_ProtocolTextDec.c_str();
                     }
                  }
                  break;
               case eCAN_COUNTER:
                  if (osn_Role == msn_USER_ROLE_SORT)
                  {
                     c_Retval = pc_CurMessage->c_Counter.ToInt64();
                  }
                  else
                  {
                     c_Retval = pc_CurMessage->c_Counter.c_str();
                  }
                  break;
               }
            }
         }
      }
      else if (((osn_Role == msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1) ||
                (osn_Role == msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_2)) ||
               (osn_Role == msn_USER_ROLE_STRING_PARTS))
      {
         if (orc_Index.parent().isValid() == true)
         {
            //Top level parent == message
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(
               orc_Index.parent().parent().isValid() ? orc_Index.parent().parent().row() : orc_Index.parent().row());

            if (pc_CurMessage != NULL)
            {
               if ((osn_Role == msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1) ||
                   (osn_Role == msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_2))
               {
                  const std::vector<QStringList> c_ColWidths = mh_GetCount(*pc_CurMessage);
                  if (c_ColWidths.size() == 2)
                  {
                     if (osn_Role == msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1)
                     {
                        c_Retval = c_ColWidths[0];
                     }
                     else
                     {
                        c_Retval = c_ColWidths[1];
                     }
                  }
               }
               else
               {
                  QStringList c_List;
                  //Replace row by signal index
                  const C_OSCComMessageLoggerDataSignal & rc_CurSignal =
                     pc_CurMessage->c_Signals[this->TranslateTreeRowsToSignalIndex(orc_Index)];
                  QString c_Comment = rc_CurSignal.c_Comment.c_str();
                  QString c_Value;
                  c_List.reserve(5);
                  c_Value = rc_CurSignal.c_Name.c_str();
                  c_List.push_back(c_Value);
                  c_Value = rc_CurSignal.c_Value.c_str();
                  c_List.push_back(c_Value);
                  c_Value = rc_CurSignal.c_Unit.c_str();
                  c_List.push_back(c_Value);
                  if (this->mq_DisplayAsHex == true)
                  {
                     c_Value = rc_CurSignal.c_RawValueHex.c_str();
                  }
                  else
                  {
                     c_Value = rc_CurSignal.c_RawValueDec.c_str();
                  }
                  c_List.push_back(c_Value);
                  c_Comment = c_Comment.remove("\n");
                  c_List.push_back(c_Comment);
                  c_Retval = c_List;
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         if (orc_Index.parent().isValid() == false)
         {
            const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
            switch (e_Col)
            {
            case eCAN_DATA:
               c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
               break;
            default:
               //Use parent value
               break;
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_MARKER)
      {
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         //Only allow selection in data column
         if (e_Col == eCAN_DATA)
         {
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Index.row());

            // if text is interpreted do not return anything
            if ((pc_CurMessage != NULL) && (pc_CurMessage->c_ProtocolTextDec == ""))
            {
               QBitArray c_Array;
               //Should always be DLC size
               c_Array.resize(pc_CurMessage->c_CanDlc.ToInt());
               //Check if there is an active selection
               if ((((this->ms32_SelectedParentRow >= 0) && (this->mc_SelectedChildBytes.size() > 0UL)) &&
                    (orc_Index.parent().isValid() == false)) && (orc_Index.row() == this->ms32_SelectedParentRow))
               {
                  //Convert byte indices into bit array
                  for (std::set<uint16>::const_iterator c_ItByte = this->mc_SelectedChildBytes.begin();
                       c_ItByte != this->mc_SelectedChildBytes.end();
                       ++c_ItByte)
                  {
                     if (*c_ItByte < c_Array.size())
                     {
                        //Set all affected bytes to be part of the highlighted area
                        c_Array.setBit(*c_ItByte, true);
                     }
                  }
               }
               else
               {
                  //Leave everything at zero if there is no highlighted area
               }
               c_Retval = c_Array;
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_MARKER_TRANSPARENCY)
      {
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         //Only allow selection in data column
         if (e_Col == eCAN_DATA)
         {
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Index.row());

            if (pc_CurMessage != NULL)
            {
               QByteArray c_Array;
               //Convert to QVariant compatible format
               //Reserve
               c_Array.reserve(pc_CurMessage->c_GreyOutInformation.c_GrayOutValueDataBytes.size());
               //Copy over
               for (std::vector<sintn>::const_iterator c_ItVal =
                       pc_CurMessage->c_GreyOutInformation.c_GrayOutValueDataBytes.begin();
                    c_ItVal != pc_CurMessage->c_GreyOutInformation.c_GrayOutValueDataBytes.end(); ++c_ItVal)
               {
                  c_Array.push_back(static_cast<charn>(*c_ItVal));
               }
               c_Retval = c_Array;
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         if (orc_Index.parent().isValid() == false)
         {
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Index.row());
            if (pc_CurMessage != NULL)
            {
               const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
               if (e_Col == eCAN_DLC)
               {
                  if (pc_CurMessage->q_CanDlcError == true)
                  {
                     // Error color
                     c_Retval = mc_STYLE_GUIDE_COLOR_18;
                  }
                  else
                  {
                     if (pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg <
                         static_cast<sintn>(this->mc_FontTransparcencyColors.size()))
                     {
                        c_Retval =
                           this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg];
                     }
                  }
               }
               else
               {
                  if (pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg <
                      static_cast<sintn>(this->mc_FontTransparcencyColors.size()))
                  {
                     c_Retval =
                        this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg];
                  }
               }
            }
         }
         else
         {
            const sint32 s32_MessageRow = static_cast<sint32>(
               orc_Index.parent().parent().isValid() ? orc_Index.parent().parent().row() : orc_Index.parent().row());
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(s32_MessageRow);
            if (pc_CurMessage != NULL)
            {
               const uint32 u32_SignalIndex = this->TranslateTreeRowsToSignalIndex(orc_Index);
               if (u32_SignalIndex < pc_CurMessage->c_Signals.size())
               {
                  const C_OSCComMessageLoggerDataSignal & rc_Signal = pc_CurMessage->c_Signals[u32_SignalIndex];
                  if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
                  {
                     //Use message value
                     c_Retval =
                        this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg];
                  }
                  else
                  {
                     //Special handling necessary
                     sint32 s32_MuxValue;
                     if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
                     {
                        s32_MuxValue = rc_Signal.c_RawValueDec.ToInt();
                     }
                     else
                     {
                        const uint32 u32_MultiplexerSignalIndex = this->TranslateTreeRowsToSignalIndex(
                           orc_Index.parent());
                        const C_OSCComMessageLoggerDataSignal & rc_MultiplexerSignal =
                           pc_CurMessage->c_Signals[u32_MultiplexerSignalIndex];
                        s32_MuxValue = rc_MultiplexerSignal.c_RawValueDec.ToInt();
                     }
                     const std::map<stw_types::sint32,
                                    stw_types::sintn>::const_iterator c_It =
                        pc_CurMessage->c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue.find(
                           s32_MuxValue);
                     //Return gray out value for this multiplexer value (if any)
                     if (c_It != pc_CurMessage->c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue.end())
                     {
                        c_Retval = this->mc_FontTransparcencyColors[c_It->second];
                     }
                     else
                     {
                        //Use message value as fallback
                        c_Retval =
                           this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.sn_GrayOutValueMsg];
                     }
                  }
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
      {
         //Although you should never see this icon we still need to return something so the default drawing engine
         // reserves the space for the manually drawn icon
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (e_Col == eTIME_STAMP)
         {
            if (orc_Index.parent().isValid() == false)
            {
               // Message
               c_Retval = C_CamMetTreeModel::mhc_IconMessage;
            }
            else
            {
               // Signal
               c_Retval = C_CamMetTreeModel::mhc_IconSignal;
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (e_Col == eTIME_STAMP)
         {
            QStringList c_Tmp;
            if (orc_Index.parent().isValid() == false)
            {
               // Message
               c_Tmp.append(C_CamMetTreeModel::mhc_IconMessage);
               c_Tmp.append(C_CamMetTreeModel::mhc_IconMessageSelected);
            }
            else
            {
               // Signal
               c_Tmp.append(C_CamMetTreeModel::mhc_IconSignal);
               c_Tmp.append(C_CamMetTreeModel::mhc_IconSignalSelected);
            }
            c_Retval = c_Tmp;
         }
      }
      else
      {
         //Nothing to do
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Column to enum conversion

   \param[in]  os32_Column    Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeModel::E_Columns C_CamMetTreeModel::h_ColumnToEnum(const sint32 os32_Column)
{
   C_CamMetTreeModel::E_Columns e_Retval;
   switch (os32_Column)
   {
   case 0:
      e_Retval = eTIME_STAMP;
      break;
   case 1:
      e_Retval = eCAN_ID;
      break;
   case 2:
      e_Retval = eCAN_NAME;
      break;
   case 3:
      e_Retval = eCAN_DIR;
      break;
   case 4:
      e_Retval = eCAN_DLC;
      break;
   case 5:
      e_Retval = eCAN_DATA;
      break;
   case 6:
   default:
      e_Retval = eCAN_COUNTER;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enum to column conversion

   \param[in]  oe_Value    Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetTreeModel::h_EnumToColumn(const C_CamMetTreeModel::E_Columns oe_Value)
{
   sint32 s32_Retval;

   switch (oe_Value)
   {
   case eTIME_STAMP:
      s32_Retval = 0;
      break;
   case eCAN_ID:
      s32_Retval = 1;
      break;
   case eCAN_NAME:
      s32_Retval = 2;
      break;
   case eCAN_DIR:
      s32_Retval = 3;
      break;
   case eCAN_DLC:
      s32_Retval = 4;
      break;
   case eCAN_DATA:
      s32_Retval = 5;
      break;
   case eCAN_COUNTER:
      s32_Retval = 6;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message at row

   \param[in]  osn_Row  Current row

   \return
   NULL Message not found
   Else Valid message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CamMetTreeLoggerData * C_CamMetTreeModel::GetMessageData(const sintn osn_Row) const
{
   const C_CamMetTreeLoggerData * pc_Retval;

   if (this->mq_UniqueMessageMode == false)
   {
      //If not unique messages: look in "queue"/vector
      const uint32 u32_Index = (this->mu32_OldestItemIndex + static_cast<uint32>(osn_Row)) %
                               this->mu32_TraceBufferSizeUsed;
      pc_Retval = &this->mc_DataBase[u32_Index];
   }
   else
   {
      //If unique messages: look in map
      const QMap<C_SCLString, C_SCLString>::const_iterator c_ItOrder =
         this->mc_UniqueMessagesOrdering.begin() + osn_Row;
      const QMap<C_SCLString, C_CamMetTreeLoggerData>::const_iterator c_It =
         this->mc_UniqueMessages.find(c_ItOrder.value());
      pc_Retval = &c_It.value();
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search a matching item

   \param[in]       orc_SearchString            String to search in model
   \param[in]       oq_Next                     Flag for search direction
                                                 true:  Search the next entry, forward
                                                 false: Search the previous entry, backward
   \param[out]      orsn_SignalRow               < 0: No signal of the message as search result
                                                >= 0: If orsn_MultiplexedSignalRow >= 0: Row of Multiplexer signal
                                                      If orsn_MultiplexedSignalRow < 0: Row of checked signal
   \param[out]      orsn_MultiplexedSignalRow   >= 0: Row of Multiplexed signal
                                                 < 0: Signal is not a multiplexed signal

   \retval  >= 0   Message found, row returned
   \retval  < 0    No Message found
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMetTreeModel::SearchMessageData(const QString & orc_SearchString, const bool oq_Next, sintn & orsn_SignalRow,
                                           sintn & orsn_MultiplexedSignalRow)
{
   sintn sn_Row = -1;
   const uint32 u32_NumberEntries = this->rowCount();
   uint32 u32_NumberEntriesToCheck = u32_NumberEntries;
   const sintn sn_START_AT_LAST_SIGNAL = -2;

   orsn_SignalRow = -1;
   orsn_MultiplexedSignalRow = -1;

   if (u32_NumberEntries > 0U)
   {
      uint32 u32_Counter;
      uint32 u32_StartIndex;
      // -1 means no signal was the match at the last search
      sintn sn_CurrentSignalIndex = -1;
      sintn sn_CurrentSignalIndexMultiplexed = -1;

      if ((this->mc_LastSearchedString == orc_SearchString) &&
          (this->msn_LastSearchedMessageRow >= 0))
      {
         // In case of the same search string, continue search with next or previous index
         if (oq_Next == true)
         {
            // Forward
            if (this->mq_IsLastSearchedSignalLastOfMessage == true)
            {
               // Start with message itself again
               u32_StartIndex = (static_cast<uint32>(this->msn_LastSearchedMessageRow) + 1) % u32_NumberEntries;
            }
            else
            {
               // Start search with next available signal of last message
               u32_StartIndex = static_cast<uint32>(this->msn_LastSearchedMessageRow);
               sn_CurrentSignalIndex = this->msn_LastSearchedSignalRow + 1;
               sn_CurrentSignalIndexMultiplexed = this->msn_LastSearchedSignalRowMultiplexed + 1;
               if (sn_CurrentSignalIndex >= 0)
               {
                  // In case of a wrap around search, the signals before the start signal must be checked at the end too
                  // No further abort condition for this scenario: Assuming the start condition is the found signal
                  // from the last search trigger, so in case of no further result, the same signal is found again
                  // which is no problem
                  ++u32_NumberEntriesToCheck;
               }
            }
         }
         else
         {
            // Check last signal first
            if (this->msn_LastSearchedSignalRow < 0)
            {
               // Prevent a minus value here
               if (this->msn_LastSearchedMessageRow == 0)
               {
                  u32_StartIndex = u32_NumberEntries - 1U;
               }
               else
               {
                  u32_StartIndex = (static_cast<uint32>(this->msn_LastSearchedMessageRow) - 1);
               }

               // special case: starting at the last signal of the message
               // but at this point the number of signals of the messages is here not available
               sn_CurrentSignalIndex = sn_START_AT_LAST_SIGNAL;
               sn_CurrentSignalIndexMultiplexed = sn_START_AT_LAST_SIGNAL;
            }
            else
            {
               // Start search with previous available signal of last message or the message itself (-1)
               u32_StartIndex = static_cast<uint32>(this->msn_LastSearchedMessageRow);
               sn_CurrentSignalIndex = this->msn_LastSearchedSignalRow - 1;
               sn_CurrentSignalIndexMultiplexed = this->msn_LastSearchedSignalRowMultiplexed - 1;

               // In case of a wrap around search, the signals after the start signal must be checked at the end too
               // No further abort condition for this scenario: Assuming the start condition is the found signal
               // from the last search trigger, so in case of no further result, the same signal is found again
               // which is no problem
               ++u32_NumberEntriesToCheck;
            }
         }
      }
      else if (oq_Next == false)
      {
         // Start index when searching backward is the end
         u32_StartIndex = u32_NumberEntries - 1U;
      }
      else
      {
         // Start index when searching forward is the begin
         u32_StartIndex = 0U;
      }

      for (u32_Counter = 0U; u32_Counter < u32_NumberEntriesToCheck; ++u32_Counter)
      {
         uint32 u32_Row;
         const C_CamMetTreeLoggerData * pc_Data;

         if (oq_Next == true)
         {
            // Search forward
            u32_Row = (u32_StartIndex + u32_Counter) % u32_NumberEntries;
         }
         else
         {
            // Search backward
            // Count from back
            const uint32 u32_Offset = (u32_NumberEntries - u32_StartIndex);
            const uint32 u32_RevCounter = (u32_NumberEntries - u32_Counter);

            // Compensate the start index
            if (u32_RevCounter >= u32_Offset)
            {
               u32_Row = u32_RevCounter - u32_Offset;
            }
            else
            {
               u32_Row = (u32_NumberEntries + u32_RevCounter) - u32_Offset;
            }

            if (u32_Counter > 0U)
            {
               // special case: starting at the last signal of the message due to backward search
               // the decision of the first message of the loop is set at the initial check of the start indexs
               sn_CurrentSignalIndex = sn_START_AT_LAST_SIGNAL;
               sn_CurrentSignalIndexMultiplexed = sn_START_AT_LAST_SIGNAL;
            }
         }

         pc_Data = this->GetMessageData(u32_Row);

         if (pc_Data != NULL)
         {
            bool q_SearchMessageResult;

            if (sn_CurrentSignalIndex <= sn_START_AT_LAST_SIGNAL)
            {
               // special case: starting at the last signal of the message due to backward search
               sn_CurrentSignalIndex = static_cast<sintn>(pc_Data->c_Signals.size()) - 1;
               sn_CurrentSignalIndexMultiplexed = sn_CurrentSignalIndex;
            }

            // Search the message and all of its signals till something was found or no signals are left to check
            q_SearchMessageResult = this->m_CheckDataForSearch(*pc_Data, orc_SearchString, oq_Next,
                                                               sn_CurrentSignalIndex,
                                                               this->mq_IsLastSearchedSignalLastOfMessage);

            if (q_SearchMessageResult == true)
            {
               sn_Row = static_cast<sintn>(u32_Row);

               // Save the match for next call
               this->msn_LastSearchedMessageRow = sn_Row;
               this->msn_LastSearchedSignalRow = sn_CurrentSignalIndex;
               // Set the multiplexed value in case of a non multiplexed message
               this->msn_LastSearchedSignalRowMultiplexed = this->msn_LastSearchedSignalRow;
               this->mc_LastSearchedString = orc_SearchString;

               if (sn_CurrentSignalIndex >= 0)
               {
                  // Check for multiplexer message
                  if (C_CamMetTreeModel::mh_IsMessageMultiplexed(*pc_Data) == true)
                  {
                     bool q_LastMessageDummy; // Not relevant of the second search
                     C_CamMetTreeLoggerData c_CopyData = *pc_Data;
                     // In this case the signals must be sorted
                     C_CamMetTreeModel::mh_SortMultiplexedSignals(c_CopyData.c_Signals);

                     // Search again
                     q_SearchMessageResult = this->m_CheckDataForSearch(c_CopyData, orc_SearchString, oq_Next,
                                                                        sn_CurrentSignalIndexMultiplexed,
                                                                        q_LastMessageDummy);

                     if (q_SearchMessageResult == true)
                     {
                        // Should be a true again
                        this->msn_LastSearchedSignalRowMultiplexed = sn_CurrentSignalIndexMultiplexed;

                        // A concrete signal of the message was found
                        // Get the correct row for the found signal
                        this->m_GetMultiplexedMsgSignalRow(c_CopyData.c_Signals,
                                                           static_cast<uint32>(sn_CurrentSignalIndexMultiplexed),
                                                           orsn_SignalRow, orsn_MultiplexedSignalRow);
                     }
                     else
                     {
                        // Should not happen
                        orsn_SignalRow = -1;
                        orsn_MultiplexedSignalRow = -1;
                     }
                  }
                  else
                  {
                     // No multiplexed message, order of vector equals order of tree
                     orsn_SignalRow = sn_CurrentSignalIndex;
                  }
               }

               break;
            }
            else
            {
               sn_CurrentSignalIndexMultiplexed = -1;
            }
         }
      }
   }

   if (sn_Row == -1)
   {
      this->m_ResetSearch();
   }

   return sn_Row;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate message tree indices to signal index

   \param[in]  orc_CurrentIndex  Current child index to get signal index for

   \return
   Translated signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_CamMetTreeModel::TranslateTreeRowsToSignalIndex(const QModelIndex & orc_CurrentIndex) const
{
   uint32 u32_Retval;

   if (orc_CurrentIndex.parent().isValid())
   {
      if (orc_CurrentIndex.parent().parent().isValid())
      {
         u32_Retval = TranslateTreeRowsToSignalIndex(orc_CurrentIndex.parent().parent().row(),
                                                     orc_CurrentIndex.parent().row(), orc_CurrentIndex.row());
      }
      else
      {
         u32_Retval = TranslateTreeRowsToSignalIndex(orc_CurrentIndex.parent().row(),
                                                     orc_CurrentIndex.row(), -1);
      }
   }
   else
   {
      //Unexpected
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate message tree indices to signal index

   \param[in]  os32_MessageIndex    Top level row
   \param[in]  os32_SignalIndex     Signal index
   \param[in]  os32_SignalIndexL2   Second level row (optional)

   \return
   Translated signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_CamMetTreeModel::TranslateTreeRowsToSignalIndex(const sint32 os32_MessageIndex, const sint32 os32_SignalIndex,
                                                         const sint32 os32_SignalIndexL2) const
{
   uint32 u32_Retval = 0UL;
   const C_CamMetTreeLoggerData * const pc_Message = this->GetMessageData(os32_MessageIndex);

   if (pc_Message != NULL)
   {
      u32_Retval = C_CamMetTreeModel::mh_TranslateTreeRowsToSignalIndex(pc_Message->c_Signals, os32_SignalIndex,
                                                                        os32_SignalIndexL2);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reserves new size by the necessary vectors and sets the configured buffer size as used value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_AdaptTraceBufferSize(void)
{
   // Activate the changed buffer size configuration
   this->mu32_TraceBufferSizeUsed = this->mu32_TraceBufferSizeConfig;

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mc_DataBase.reserve(this->mu32_TraceBufferSizeUsed);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_RootItemContinuous->ReserveChildrenSpace(this->mu32_TraceBufferSizeUsed);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified strings as rows

   Only handle continuous mode storage

   \param[in]  orc_Data    New row content

   \return
   Indices of added rows (only valid if not in unique message mode)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_CamMetTreeModel::m_AddRowsContinuousMode(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   std::vector<sint32> c_Retval;
   if (orc_Data.empty() == false)
   {
      //Columns which need to get updated on change of static message
      const uint32 u32_CompleteSize = static_cast<uint32>(this->mc_DataBase.size()) +
                                      static_cast<uint32>(orc_Data.size());

      //Check if only append case
      if (u32_CompleteSize <= this->mu32_TraceBufferSizeUsed)
      {
         //Appending so notify model of insert action
         if (this->mq_UniqueMessageMode == false)
         {
            this->beginInsertRows(QModelIndex(), this->rowCount(),
                                  this->rowCount() + (static_cast<sintn>(orc_Data.size()) - 1));
         }
         //Appending items in reserved space
         for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
              c_ItData != orc_Data.end(); ++c_ItData)
         {
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            this->mc_DataBase.push_back(*c_ItData);
            //Add tree item
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, false, -1);
            this->mpc_RootItemContinuous->AddChild(pc_Item);
         }
         //Appending so notify model of insert action
         if (this->mq_UniqueMessageMode == false)
         {
            this->endInsertRows();
         }
      }
      //Check if we need to append any items to reach the max storage size
      else if (this->mc_DataBase.size() < this->mu32_TraceBufferSizeUsed)
      {
         //Simple model reset notification (if replaced: look at two segments: adding new and queuing mechanism)
         if (this->mq_UniqueMessageMode == false)
         {
            this->beginResetModel();
         }
         //One time code doing both parts
         for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
              c_ItData != orc_Data.end(); ++c_ItData)
         {
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, false, -1);
            //1. insert until space is no longer available
            if (this->mc_DataBase.size() < this->mu32_TraceBufferSizeUsed)
            {
               //Append in reserved space
               this->mc_DataBase.push_back(*c_ItData);
               //Add tree item
               this->mpc_RootItemContinuous->AddChild(pc_Item);
            }
            //2. start replacing oldest ones
            else
            {
               //Shift item
               this->mc_DataBase[this->mu32_OldestItemIndex] = *c_ItData;
               //Replace tree item
               delete (this->mpc_RootItemContinuous->c_Children[0UL]);
               this->mpc_RootItemContinuous->c_Children.erase(this->mpc_RootItemContinuous->c_Children.begin());
               this->mpc_RootItemContinuous->AddChild(pc_Item);
               //Iterate
               ++this->mu32_OldestItemIndex;
               this->mu32_OldestItemIndex %= this->mu32_TraceBufferSizeUsed;
            }
         }
         //Simple model reset notification
         if (this->mq_UniqueMessageMode == false)
         {
            //Every reset will clear the selection
            this->SetSelection(-1, -1);
            this->endResetModel();
         }
      }
      else
      {
         //Begin queue shift
         if (this->mq_UniqueMessageMode == false)
         {
            //Use reset model to avoid console output, warning: does have negative side effects, but also faster
            this->beginResetModel();
            //this->beginRemoveRows(QModelIndex(), 0, orc_Data.size() - 1);
            //this->beginInsertRows(QModelIndex(), this->rowCount() - (orc_Data.size() - 1), this->rowCount());
         }
         //Shift item in queue
         for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
              c_ItData != orc_Data.end(); ++c_ItData)
         {
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            this->mc_DataBase[this->mu32_OldestItemIndex] = *c_ItData;
            //Replace tree item
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, false, -1);
            delete (this->mpc_RootItemContinuous->c_Children[0UL]);
            this->mpc_RootItemContinuous->c_Children.erase(this->mpc_RootItemContinuous->c_Children.begin());
            this->mpc_RootItemContinuous->AddChild(pc_Item);
            //Iterate
            ++this->mu32_OldestItemIndex;
            this->mu32_OldestItemIndex %= this->mu32_TraceBufferSizeUsed;
         }
         //End queue shift
         if (this->mq_UniqueMessageMode == false)
         {
            //Every reset will clear the selection
            this->SetSelection(-1, -1);
            //Use reset model to avoid console output, warning: reset model does have negative side effects
            this->endResetModel();
            //this->endInsertRows();
            //this->endRemoveRows();
         }
      }
      //Every time count the actual indices
      if (this->mq_UniqueMessageMode == false)
      {
         sint32 s32_Start = static_cast<sint32>(this->rowCount()) - (static_cast<sint32>(orc_Data.size()) - 1);
         c_Retval.reserve(orc_Data.size());
         for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
              c_ItData != orc_Data.end(); ++c_ItData)
         {
            c_Retval.push_back(s32_Start);
            ++s32_Start;
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified strings as rows

   Only handle unique mode storage

   \param[in]  orc_Data    New row content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_AddRowsUnique(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   if (orc_Data.empty() == false)
   {
      //Every time
      for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
           c_ItData != orc_Data.end(); ++c_ItData)
      {
         const QMap<stw_scl::C_SCLString,
                    C_CamMetTreeLoggerData>::const_iterator c_ItMessage = this->mc_UniqueMessages.find(
            c_ItData->c_CanIdDec);
         const sint32 s32_MuxValue = C_CamMetUtil::mh_GetMultiplexerValue(c_ItData->c_Signals);

         //Check if there is a new row
         if (c_ItMessage != this->mc_UniqueMessages.end())
         {
            this->m_HandleNewUniqueMessageForExistingUniqueMessage(*c_ItData, c_ItMessage.key(), s32_MuxValue);
         }
         else
         {
            this->m_HandleNewUniqueMessage(*c_ItData, s32_MuxValue);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new unique message for existing unique message entry

   \param[in]  orc_Message             New message
   \param[in]  orc_ExistingMessageKey  Key to use to find the existing message position
   \param[in]  os32_MultiplexerValue   Current multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_HandleNewUniqueMessageForExistingUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                                         const stw_scl::C_SCLString & orc_ExistingMessageKey,
                                                                         const sint32 os32_MultiplexerValue)
{
   sint32 s32_MessageCounter = 0L;
   bool q_UpdateDataTimeStamp;
   C_CamMetTreeLoggerData & rc_Message = this->mc_UniqueMessages[orc_Message.c_CanIdDec];
   const uint64 u64_PreviousAbsoluteTimestampValue = rc_Message.u64_TimeStampAbsoluteStart;
   const uint64 u64_NewAbsoluteTimestampValue = orc_Message.u64_TimeStampAbsoluteStart;
   uint64 u64_RelativeTimestamp;

   const uint32 u32_PrevMsgTimeStamp = static_cast<uint32>(rc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);
   const uint32 u32_NewMsgTimeStamp = static_cast<uint32>(orc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);

   // Save previous values which are not filled by C_SyvComMessageMonitor
   const C_CamMetTreeLoggerDataGreyOutInformation c_PreviousInfo =
      C_CamMetTreeModel::mh_ExtractPreviousGreyOutInformation(rc_Message, orc_Message, u32_PrevMsgTimeStamp,
                                                              u32_NewMsgTimeStamp,
                                                              os32_MultiplexerValue,
                                                              q_UpdateDataTimeStamp);

   //Keep not redundant signal information (multiplexer + multiplexed signal)
   C_CamMetTreeModel::mh_CopyMessageWhileKeepingUniqueSignals(rc_Message, orc_Message, os32_MultiplexerValue);

   C_CamMetTreeModel::mh_ApplyPreviousGreyOutInformation(rc_Message, c_PreviousInfo, q_UpdateDataTimeStamp,
                                                         u32_PrevMsgTimeStamp);

   //Handle relative timestamp value
   if (u64_NewAbsoluteTimestampValue > u64_PreviousAbsoluteTimestampValue)
   {
      u64_RelativeTimestamp = u64_NewAbsoluteTimestampValue - u64_PreviousAbsoluteTimestampValue;
   }
   else
   {
      u64_RelativeTimestamp = 0U;
   }
   rc_Message.c_TimeStampRelative = C_OSCComMessageLoggerData::h_GetTimestampAsString(u64_RelativeTimestamp);

   //Search existing item index
   for (QMap<stw_scl::C_SCLString,
             stw_scl::C_SCLString>::const_iterator c_ItMsg = this->mc_UniqueMessagesOrdering.begin();
        c_ItMsg != this->mc_UniqueMessagesOrdering.end(); ++c_ItMsg)
   {
      if (c_ItMsg.value() == orc_ExistingMessageKey)
      {
         break;
      }
      else
      {
         ++s32_MessageCounter;
      }
   }
   //Update existing tree item
   m_UpdateTreeItemBasedOnMessage(this->mpc_RootItemStatic->c_Children[static_cast<uint32>(s32_MessageCounter)],
                                  rc_Message, true, s32_MessageCounter);
   //Notify the model of data changes for all replaced items (current: all items)
   if (this->mq_UniqueMessageMode == true)
   {
      const sint32 s32_ColIDName = C_CamMetTreeModel::h_EnumToColumn(eCAN_NAME);
      const sint32 s32_ColIDDLC = C_CamMetTreeModel::h_EnumToColumn(eCAN_DLC);
      const sint32 s32_ColIDData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
      const sint32 s32_ColIDTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      const sint32 s32_ColIDCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
      //Notification for possible new child
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIDTime), this->index(s32_MessageCounter, s32_ColIDTime));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIDCounter),
                        this->index(s32_MessageCounter, s32_ColIDCounter));
      //Possible changes
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIDName), this->index(s32_MessageCounter, s32_ColIDName));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIDDLC), this->index(s32_MessageCounter, s32_ColIDDLC));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIDData), this->index(s32_MessageCounter, s32_ColIDData));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new unique message entry

   \param[in]  orc_Message             New message
   \param[in]  os32_MultiplexerValue   Current multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_HandleNewUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                 const sint32 os32_MultiplexerValue)
{
   bool q_NewRow = false;

   uint8 u8_DbCounter;

   QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_NewPos;
   //Handle begin!
   C_TblTreSimpleItem * const pc_NewItem = new C_TblTreSimpleItem();
   const sintn sn_EstimatedPosIndex = this->m_GetPosIndexForUniqueMessage(orc_Message.c_TimeStampAbsoluteStart);
   //Update tree with known index
   m_UpdateTreeItemBasedOnMessage(pc_NewItem, orc_Message, false, sn_EstimatedPosIndex);
   this->mpc_RootItemStatic->InsertChild(sn_EstimatedPosIndex, pc_NewItem);

   if (this->mq_UniqueMessageMode == true)
   {
      //handle new row with known position index
      q_NewRow = true;

      //Notify the new row count
      this->beginInsertRows(QModelIndex(), sn_EstimatedPosIndex, sn_EstimatedPosIndex);
   }

   //Insert new item
   this->mc_UniqueMessagesOrdering.insert(orc_Message.c_TimeStampAbsoluteStart, orc_Message.c_CanIdDec);
   c_NewPos = this->mc_UniqueMessages.insert(orc_Message.c_CanIdDec, orc_Message);
   // New message, new data. Update the timestamp of the CAN message data and its bytes
   c_NewPos->c_GreyOutInformation.u32_DataChangedTimeStamp =
      static_cast<uint32>(c_NewPos->c_CanMsg.u64_TimeStamp / 1000ULL);
   for (u8_DbCounter = 0; u8_DbCounter < c_NewPos->c_CanMsg.u8_DLC; ++u8_DbCounter)
   {
      c_NewPos->c_GreyOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter] =
         c_NewPos->c_GreyOutInformation.u32_DataChangedTimeStamp;
   }
   //Init specific timestamp of multiplexer map
   if (os32_MultiplexerValue >= 0)
   {
      c_NewPos->c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps[os32_MultiplexerValue] =
         c_NewPos->c_GreyOutInformation.u32_DataChangedTimeStamp;
   }

   //Check if begin was called so we properly close the insert operation
   if (q_NewRow)
   {
      this->endInsertRows();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message row index for the current timestamp (may be estimated)

   \param[in]  orc_AbsoluteTimeStamp   Time stamp to work with

   \return
   Index of new message (may be estimated)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetTreeModel::m_GetPosIndexForUniqueMessage(const C_SCLString & orc_AbsoluteTimeStamp) const
{
   sintn sn_EstimatedPosIndex = 0;
   //Get first valid item after the position it should be at
   const QMap<C_SCLString, C_SCLString>::const_iterator c_EstimatedPos = this->mc_UniqueMessagesOrdering.upperBound(
      orc_AbsoluteTimeStamp);

   //Calc estimated new position index
   for (QMap<C_SCLString, C_SCLString>::const_iterator c_ItUniqueMessages = this->mc_UniqueMessagesOrdering.begin();
        (c_ItUniqueMessages != this->mc_UniqueMessagesOrdering.end()) && (c_ItUniqueMessages != c_EstimatedPos);
        ++c_ItUniqueMessages)
   {
      ++sn_EstimatedPosIndex;
   }
   return sn_EstimatedPosIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Timer function for handling gray out engine
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_GrayOutTimer(void)
{
   if ((this->mq_GrayOutPause == false) &&
       (this->mq_UniqueMessageMode == true) &&
       (this->mc_UniqueMessages.size() > 0))
   {
      // ms resolution is enough
      // Using US as base because we want to compare with the US value of the CAN message.
      // The TGL_GetTickCountUS can have a difference of 1 to 2 seconds because of a different rounding
      const uint32 u32_CurrentTime = static_cast<uint32>(TGL_GetTickCountUS() / 1000ULL);
      QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_It;
      sintn sn_RowCounter = 0;

      for (c_It = this->mc_UniqueMessages.begin(); c_It != this->mc_UniqueMessages.end(); ++c_It)
      {
         C_CamMetTreeLoggerData & rc_Data = c_It.value();
         const uint32 u32_DiffMsg = u32_CurrentTime - static_cast<uint32>(rc_Data.c_CanMsg.u64_TimeStamp / 1000ULL);
         // TimeStamp for data is set by AddRows when new CAN messages are added to the model
         sintn sn_TransparencyStepMsg;
         sintn sn_CounterDataByte;
         bool q_DataByteChangedTransparency = false;

         // Check message gray out adaption
         sn_TransparencyStepMsg = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffMsg);

         // Use each value and check if gray out value changed
         for (std::map<stw_types::sint32, stw_types::uint32>::const_iterator c_ItValue =
                 rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.begin();
              c_ItValue != rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.end(); ++c_ItValue)
         {
            const uint32 u32_DiffDataByte = u32_CurrentTime - c_ItValue->second;
            sintn sn_TransparencyStepDataByte;

            sn_TransparencyStepDataByte = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffDataByte);

            if (sn_TransparencyStepDataByte !=
                rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue[c_ItValue->first])
            {
               QVector<sintn> c_Roles;
               const sint32 s32_Row = C_CamMetUtil::mh_GetRowForMultiplexerValue(rc_Data.c_Signals, c_ItValue->first);
               // Save the new value
               rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue[c_ItValue->first] =
                  sn_TransparencyStepDataByte;
               //Trigger ui update

               // Send the change only if the value changed really and let update the foreground only
               c_Roles.push_back(static_cast<sintn>(Qt::ForegroundRole));
               this->dataChanged(this->index(s32_Row, 0, this->index(sn_RowCounter, 0)),
                                 this->index(s32_Row, 0, this->index(sn_RowCounter, 0)), c_Roles);
            }
         }

         // Check data byte gray out adaption
         for (sn_CounterDataByte = 0; sn_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++sn_CounterDataByte)
         {
            const uint32 u32_DiffDataByte = u32_CurrentTime -
                                            rc_Data.c_GreyOutInformation.c_DataBytesChangedTimeStamps[sn_CounterDataByte
                                            ];
            sintn sn_TransparencyStepDataByte;

            sn_TransparencyStepDataByte = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffDataByte);

            if (sn_TransparencyStepDataByte != rc_Data.c_GreyOutInformation.c_GrayOutValueDataBytes[sn_CounterDataByte])
            {
               // At least one data byte has a changed transparency value
               q_DataByteChangedTransparency = true;
               // Save the new value
               rc_Data.c_GreyOutInformation.c_GrayOutValueDataBytes[sn_CounterDataByte] = sn_TransparencyStepDataByte;
            }
         }

         if (sn_TransparencyStepMsg != rc_Data.c_GreyOutInformation.sn_GrayOutValueMsg)
         {
            // Update the gray out value. The gray out value for data will be adapted too
            const sint32 s32_ColIDTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
            const sint32 s32_ColIDCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
            QVector<sintn> c_Roles;

            rc_Data.c_GreyOutInformation.sn_GrayOutValueMsg = sn_TransparencyStepMsg;

            // Send the change only if the value changed really and let update the foreground only
            c_Roles.push_back(static_cast<sintn>(Qt::ForegroundRole));
            this->dataChanged(this->index(sn_RowCounter, s32_ColIDTime), this->index(sn_RowCounter, s32_ColIDCounter),
                              c_Roles);
         }
         else if (q_DataByteChangedTransparency == true)
         {
            // Update the gray out value of CAN message data only.
            const sint32 s32_ColData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
            QVector<sintn> c_Roles;

            // Send the change only if the value changed really and let update the foreground only
            c_Roles.push_back(static_cast<sintn>(Qt::ForegroundRole));
            this->dataChanged(this->index(sn_RowCounter, s32_ColData), this->index(sn_RowCounter, s32_ColData),
                              c_Roles);
         }
         else
         {
            // Nothing to do
         }

         ++sn_RowCounter;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Detects the necessary transparency step dependent of the difference time

   \param[in]  ou32_DiffTime  Difference time

   \return
   Detected transparency step
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMetTreeModel::mh_GetTransparencyStep(const uint32 ou32_DiffTime)
{
   sintn sn_TransparencyStep;

   if (ou32_DiffTime > mu32_TRACE_TRANSPARENCY_TIME_START)
   {
      if (ou32_DiffTime >= mu32_TRACE_TRANSPARENCY_TIME_END)
      {
         // Maximum reached
         sn_TransparencyStep = msn_TRACE_TRANSPARENCY_STEPS;
      }
      else
      {
         // Interpolation
         const uint32 u32_DiffPart = ou32_DiffTime - mu32_TRACE_TRANSPARENCY_TIME_START;
         sn_TransparencyStep =
            static_cast<sintn>((u32_DiffPart * static_cast<uint32>(msn_TRACE_TRANSPARENCY_STEPS)) /
                               mu32_TRACE_TRANSPARENCY_TIME_DIFF);
      }
   }
   else
   {
      sn_TransparencyStep = 0;
   }

   return sn_TransparencyStep;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of strings

   \param[in]  orc_Message    Message to evaluate

   \return
   List of strings
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QStringList> C_CamMetTreeModel::mh_GetCount(const C_CamMetTreeLoggerData & orc_Message)
{
   std::vector<QStringList> c_Retval;
   QStringList c_W1;
   QStringList c_W2;
   //Handle three sizes
   c_Retval.reserve(2);
   c_W1.reserve(orc_Message.c_Signals.size());
   c_W2.reserve(orc_Message.c_Signals.size());
   //Append words
   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Message.c_Signals[u32_ItSig];
      c_W1.push_back(rc_Signal.c_Name.c_str());
      c_W2.push_back(rc_Signal.c_Unit.c_str());
   }
   c_Retval.push_back(c_W1);
   c_Retval.push_back(c_W2);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resize string to the specified maximum length if the string exceeds this limit

   \param[in,out]  orc_Str          String that will get resized if necessary
   \param[in]      os32_MaxLength   Maximum length the string may not exceed after calling this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_ResizeIfNecessary(QString & orc_Str, const sint32 os32_MaxLength)
{
   orc_Str.resize(std::min(orc_Str.count(), static_cast<sintn>(os32_MaxLength)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update tree item based on message

   \param[in,out]  opc_Item         Item to update
   \param[in]      orc_Message      Message to update from
   \param[in]      oq_SignalInsert  Flag if begin and end insert should be called
   \param[in]      os32_MessageRow  Message row index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_UpdateTreeItemBasedOnMessage(C_TblTreSimpleItem * const opc_Item,
                                                       const C_CamMetTreeLoggerData & orc_Message,
                                                       const bool oq_SignalInsert, const sint32 os32_MessageRow)
{
   const std::vector<sintn> c_Order = C_CamMetUtil::h_GetMultiplexerOrder(orc_Message.c_Signals);

   if (c_Order.size() == 0UL)
   {
      //Normal message
      if (opc_Item->c_Children.size() < orc_Message.c_Signals.size())
      {
         if ((oq_SignalInsert) && (os32_MessageRow >= 0))
         {
            this->beginInsertRows(this->index(os32_MessageRow, 0), static_cast<sintn>(opc_Item->c_Children.size()),
                                  static_cast<sintn>(opc_Item->c_Children.size() + orc_Message.c_Signals.size()));
         }
         //Add
         for (uint32 u32_ItSig = opc_Item->c_Children.size(); u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            opc_Item->AddChild(new C_TblTreSimpleItem());
         }
         if ((oq_SignalInsert) && (os32_MessageRow >= 0))
         {
            this->endInsertRows();
         }
      }
      else if (opc_Item->c_Children.size() > orc_Message.c_Signals.size())
      {
         if ((oq_SignalInsert) && (os32_MessageRow >= 0))
         {
            this->beginResetModel();
         }
         //Remove
         opc_Item->ClearChildren();
         for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            opc_Item->AddChild(new C_TblTreSimpleItem());
         }
         if ((oq_SignalInsert) && (os32_MessageRow >= 0))
         {
            this->endResetModel();
         }
      }
      else
      {
         //Nothing to do
      }
   }
   else
   {
      std::vector<uint32> c_Expected;
      std::vector<uint32> c_Current;
      //Multiplexer
      for (uint32 u32_ItOr = 0UL; u32_ItOr < c_Order.size(); ++u32_ItOr)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Message.c_Signals[u32_ItSig];
            if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               if (rc_SignalData.c_OscSignal.u16_MultiplexValue == c_Order[u32_ItOr])
               {
                  ++u32_Counter;
               }
            }
         }
         c_Expected.push_back(u32_Counter);
      }
      //Others
      for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
      {
         const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Message.c_Signals[u32_ItSig];
         if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
         {
            c_Expected.push_back(0UL);
         }
      }
      //Current state
      c_Current.reserve(opc_Item->c_Children.size());
      for (uint32 u32_ItChild = 0UL; u32_ItChild < opc_Item->c_Children.size(); ++u32_ItChild)
      {
         const C_TblTreSimpleItem * const pc_Child = opc_Item->c_Children[u32_ItChild];
         if (pc_Child != NULL)
         {
            c_Current.push_back(pc_Child->c_Children.size());
         }
      }
      //Compare
      if (c_Expected != c_Current)
      {
         uint32 u32_InsertAt = 0UL;
         uint32 u32_InsertNum = 0UL;
         const bool q_FixByInsertingNewChild = C_CamMetTreeModel::mh_CheckForFixByInsertingNewChild(c_Expected,
                                                                                                    c_Current,
                                                                                                    u32_InsertAt,
                                                                                                    u32_InsertNum);
         if (q_FixByInsertingNewChild)
         {
            if ((oq_SignalInsert) && (os32_MessageRow >= 0))
            {
               this->beginInsertRows(this->index(os32_MessageRow, 0), u32_InsertAt, u32_InsertAt);
            }
            C_TblTreSimpleItem * const pc_Parent = new C_TblTreSimpleItem();
            for (uint32 u32_ItChild = 0UL; u32_ItChild < u32_InsertNum; ++u32_ItChild)
            {
               pc_Parent->AddChild(new C_TblTreSimpleItem());
            }
            opc_Item->InsertChild(u32_InsertAt, pc_Parent);
            if ((oq_SignalInsert) && (os32_MessageRow >= 0))
            {
               this->endInsertRows();
            }
         }
         else
         {
            if ((oq_SignalInsert) && (os32_MessageRow >= 0))
            {
               this->beginResetModel();
            }
            //Clear
            opc_Item->ClearChildren();
            //Rebuild as expected
            for (uint32 u32_ItExpected = 0UL; u32_ItExpected < c_Expected.size(); ++u32_ItExpected)
            {
               C_TblTreSimpleItem * const pc_Parent = new C_TblTreSimpleItem();
               for (uint32 u32_ItChild = 0UL; u32_ItChild < c_Expected[u32_ItExpected]; ++u32_ItChild)
               {
                  pc_Parent->AddChild(new C_TblTreSimpleItem());
               }
               opc_Item->AddChild(pc_Parent);
            }
            if ((oq_SignalInsert) && (os32_MessageRow >= 0))
            {
               this->endResetModel();
            }
         }
      }
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares one data entry with the search string

   \param[in]     orc_Data              Message to search
   \param[in]     orc_SearchString      String to search in data for
   \param[in]     oq_Next               Flag for search direction of signals
                                          true:  Search the next entry, forward
                                          false: Search the previous entry, backward
   \param[in,out] orsn_SignalIndex      Input: >= 0: signal index to start search; -1: start with message
                                        Output: >= 0: signal matches; -1: no signal matched or the message matches
   \param[out]    orq_LastSignalChecked true: all signals were checked of the message, not necessary to check in next
                                              round
                                        false: something did match to the search string, but not all signal were
                                               checked. At least one further check is necessary for this message


   \retval   true    The message or one signal does match to the search string
   \retval   false   The message and its signals does not match to search string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::m_CheckDataForSearch(const C_CamMetTreeLoggerData & orc_Data, const QString & orc_SearchString,
                                             const bool oq_Next, stw_types::sintn & orsn_SignalIndex,
                                             bool & orq_LastSignalChecked) const
{
   bool q_Return = false;

   if (orc_SearchString != "")
   {
      // Check only the string variant, which are visible
      // Strings which has no letters, no upper case necessary
      const sintn osn_StartSignalRow = orsn_SignalIndex;
      bool q_SkipSignals = false;

      // Reset the signal row for not found as default output
      orsn_SignalIndex = -1;

      // Check message strings when no start signal is defined
      if (osn_StartSignalRow < 0)
      {
         q_Return = this->m_CheckMessageDataForSearch(orc_Data, orc_SearchString);

         if (oq_Next == false)
         {
            // Searching backward means in this case no signals of the message will be checked at this run
            q_SkipSignals = true;
         }
      }

      if (q_SkipSignals == false)
      {
         // Check signals
         if (orc_Data.c_Signals.size() > 0)
         {
            orq_LastSignalChecked = false;

            if (q_Return == false)
            {
               // Check signals
               uint32 u32_Counter = 0U;
               bool q_SearchFinished = false;

               if (osn_StartSignalRow > 0)
               {
                  // Set start signal
                  u32_Counter = static_cast<uint32>(osn_StartSignalRow);
               }

               do
               {
                  // Check each signal
                  q_Return = this->m_CheckSignalDataForSearch(orc_Data.c_Signals[u32_Counter], orc_SearchString);

                  if (q_Return == true)
                  {
                     // Found a match, search finished
                     orsn_SignalIndex = static_cast<sintn>(u32_Counter);
                     q_SearchFinished = true;
                  }
                  else
                  {
                     if (oq_Next == true)
                     {
                        // Searching forward
                        ++u32_Counter;

                        if (u32_Counter >= orc_Data.c_Signals.size())
                        {
                           q_SearchFinished = true;
                        }
                     }
                     else
                     {
                        // Searching backward
                        if (u32_Counter > 0UL)
                        {
                           --u32_Counter;
                        }
                        else
                        {
                           q_SearchFinished = true;
                        }
                     }
                  }
               }
               while (q_SearchFinished == false);

               // Check if the last signal was reached. Not relevant if it matched or not
               if (u32_Counter == (orc_Data.c_Signals.size() - 1UL))
               {
                  orq_LastSignalChecked = true;
               }

               // Special case: Searching backward, started with a signal and nothing found yet -> Search message now
               if ((oq_Next == false) &&
                   (q_Return == false))
               {
                  q_Return = this->m_CheckMessageDataForSearch(orc_Data, orc_SearchString);
               }
            }
         }
         else
         {
            // No signals available for checking
            orq_LastSignalChecked = true;
         }
      }
   }
   else
   {
      orq_LastSignalChecked = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares one data entry with the search string

   \param[in]     orc_Data              Message to search
   \param[in]     orc_SearchString     String to search in model

   \retval   true    The message does match to the search string
   \retval   false   The message does not match to search string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::m_CheckMessageDataForSearch(const C_CamMetTreeLoggerData & orc_Data,
                                                    const QString & orc_SearchString) const
{
   bool q_Return = false;

   // Compare all in upper case to be case insensitive
   const C_SCLString c_SearchString = orc_SearchString.toUpper().toStdString().c_str();

   if (orc_SearchString != "")
   {
      // Check only the string variant, which are visible
      // Strings which has no letters, no upper case necessary

      // Check message strings
      if ((orc_Data.c_Name.UpperCase().Pos(c_SearchString) > 0) ||
          (orc_Data.c_CanDlc.Pos(c_SearchString) > 0) ||
          ((this->mq_DisplayTimestampRelative == true) &&
           (orc_Data.c_TimeStampRelative.Pos(c_SearchString) > 0)) ||
          ((this->mq_DisplayTimestampAbsoluteTimeOfDay == true) &&
           (orc_Data.c_TimeStampAbsoluteTimeOfDay.Pos(c_SearchString) > 0)) ||
          ((this->mq_DisplayTimestampAbsoluteTimeOfDay == false) &&
           (orc_Data.c_TimeStampAbsoluteStart.Pos(c_SearchString) > 0)))
      {
         q_Return = true;
      }
      else
      {
         if (this->mq_DisplayAsHex == true)
         {
            // Check hex display specific strings
            if ((orc_Data.c_CanIdHex.UpperCase().Pos(c_SearchString) > 0) ||
                (orc_Data.c_CanDataHex.UpperCase().Pos(c_SearchString) > 0) ||
                (orc_Data.c_ProtocolTextHex.UpperCase().Pos(c_SearchString) > 0))
            {
               q_Return = true;
            }
         }
         else
         {
            // Check decimal display specific strings
            if ((orc_Data.c_CanIdDec.Pos(c_SearchString) > 0) ||
                (orc_Data.c_CanDataDec.Pos(c_SearchString) > 0) ||
                (orc_Data.c_ProtocolTextDec.UpperCase().Pos(c_SearchString) > 0))
            {
               q_Return = true;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares one data signal entry with the search string

   \param[in]     orc_SignalData       Message to search
   \param[in]     orc_SearchString     String to search in model

   \retval   true    The signal does match to the search string
   \retval   false   The signal does not match to search string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::m_CheckSignalDataForSearch(const C_OSCComMessageLoggerDataSignal & orc_SignalData,
                                                   const QString & orc_SearchString) const
{
   bool q_Return = false;

   // Compare all in upper case to be case insensitive
   const C_SCLString c_SearchString = orc_SearchString.toUpper().toStdString().c_str();

   if (orc_SearchString != "")
   {
      // Check only the string variant, which are visible
      // Strings which has no letters, no upper case necessary

      if ((orc_SignalData.c_Name.UpperCase().Pos(c_SearchString) > 0) ||
          (orc_SignalData.c_Comment.UpperCase().Pos(c_SearchString) > 0) ||
          (orc_SignalData.c_Unit.UpperCase().Pos(c_SearchString) > 0) ||
          (orc_SignalData.c_Value.UpperCase().Pos(c_SearchString) > 0))
      {
         q_Return = true;
      }
      else
      {
         if (this->mq_DisplayAsHex == true)
         {
            // Check hex display specific strings
            if (orc_SignalData.c_RawValueHex.UpperCase().Pos(c_SearchString) > 0)
            {
               q_Return = true;
            }
         }
         else
         {
            // Check decimal display specific strings
            if (orc_SignalData.c_RawValueDec.Pos(c_SearchString) > 0)
            {
               q_Return = true;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a signal for its multiplexer configuration and the resulting row position and tree layer position

   If orsn_MultiplexedSignalRow is >= 0, the checked  signal is a multiplexed signal and the signal is on
   the third level of the model. Then osn_SignalRow equals the row of the multiplexer signal.

   If orsn_MultiplexedSignalRow is < 0, the checked signal can be a normal signal or the multiplexer signal and is on
   the second level of the model.

   Condition: The vector of signals must be sorted by function mh_SortMultiplexedSignals or by default

   \param[in]       orc_Signals                All signals to compare and check with
   \param[in]       ou32_SignalIndexToCheck    The signal to check
   \param[out]      orsn_SignalRow              >= 0: If orsn_MultiplexedSignalRow >= 0: Row of Multiplexer signal
                                                       If orsn_MultiplexedSignalRow < 0: Row of checked signal
                                                 < 0: ou32_SignalIndexToCheck not valid
   \param[out]      orsn_MultiplexedSignalRow   >= 0: Row of Multiplexed signal
                                                 < 0: Signal is not a multiplexed signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_GetMultiplexedMsgSignalRow(const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals,
                                                     const uint32 ou32_SignalIndexToCheck, sintn & orsn_SignalRow,
                                                     sintn & orsn_MultiplexedSignalRow) const
{
   orsn_SignalRow = -1;
   orsn_MultiplexedSignalRow = -1;

   if (ou32_SignalIndexToCheck < orc_Signals.size())
   {
      uint32 u32_Counter;
      const C_OSCCanSignal & rc_OscSginalToCheck = orc_Signals[ou32_SignalIndexToCheck].c_OscSignal;

      // Multiplexer signals come first
      // Multiplexed signals has the matching multiplexer signal as parent
      // Default signals come after the multiplexer signals with its multiplexed signals
      if (rc_OscSginalToCheck.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
      {
         orsn_SignalRow = 0;

         for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
         {
            const C_OSCCanSignal & rc_OscSginal = orc_Signals[u32_Counter].c_OscSignal;

            // Count all multiplexer signals which are always at the beginning and all default signals
            // which have a lower index
            // Multiplexed signals are not on the same level
            if ((rc_OscSginal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL) ||
                ((rc_OscSginal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT) &&
                 (u32_Counter < ou32_SignalIndexToCheck)))
            {
               ++orsn_SignalRow;
            }
         }
      }
      else if (rc_OscSginalToCheck.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
      {
         orsn_MultiplexedSignalRow = 0;

         // The signal is on third level of the tree below the multiplexer signal
         for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
         {
            const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_Counter];
            const C_OSCCanSignal & rc_OscSginal = rc_Signal.c_OscSignal;

            if ((u32_Counter < ou32_SignalIndexToCheck) &&
                (rc_OscSginal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                (rc_OscSginal.u16_MultiplexValue == rc_OscSginalToCheck.u16_MultiplexValue))
            {
               // Search the multiplexed signals with the same multiplexer value with a lower index to get the row of
               // the checked signal below the multiplexer signal
               ++orsn_MultiplexedSignalRow;
            }
            else if (rc_OscSginal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
            {
               const sintn sn_Value = rc_Signal.c_RawValueDec.ToIntDef(-1);

               if (sn_Value >= 0)
               {
                  uint16 u16_Value = static_cast<uint16>(sn_Value);
                  if (u16_Value <= rc_OscSginalToCheck.u16_MultiplexValue)
                  {
                     // In this case, this is the row of the multiplexer signal
                     // Starting with -1 all multiplexer signals whit a concrete value lower or equal to the
                     // expected value count for the row
                     ++orsn_SignalRow;
                  }
               }
            }
            else
            {
               // Nothing to do
            }
         }
      }
      else
      {
         // Multiplexer signal
         const sintn sn_CheckValue = orc_Signals[ou32_SignalIndexToCheck].c_RawValueDec.ToIntDef(-1);

         if (sn_CheckValue >= 0)
         {
            for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
            {
               const C_OSCComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_Counter];
               const C_OSCCanSignal & rc_OscSginal = rc_Signal.c_OscSignal;
               const sintn sn_Value = rc_Signal.c_RawValueDec.ToIntDef(-1);

               if (sn_Value >= 0)
               {
                  // Starting with -1 all multiplexer signals whit a concrete value lower or equal to the
                  // expected value count for the row
                  if ((rc_OscSginal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
                      (sn_Value <= sn_CheckValue))
                  {
                     ++orsn_SignalRow;
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the last search information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_ResetSearch(void)
{
   this->mc_LastSearchedString = "";
   this->msn_LastSearchedMessageRow = -1;
   this->msn_LastSearchedSignalRow = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract previous gray out value information to handle new message

   \param[in]   orc_MessagePrev           Previous message information
   \param[in]   orc_MessageNew            New message information
   \param[in]   ou32_PrevMsgTimeStamp     Previous message timestamp
   \param[in]   ou32_NewMsgTimeStamp      New message time stamp
   \param[in]   os32_MultiplexerValue     New multiplexer value
   \param[out]  orq_UpdateDataTimeStamp   Flag if data timestamp should be updated

   \return
   Current gray out value information
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeLoggerDataGreyOutInformation C_CamMetTreeModel::mh_ExtractPreviousGreyOutInformation(
   const C_CamMetTreeLoggerData & orc_MessagePrev, const C_CamMetTreeLoggerData & orc_MessageNew,
   const uint32 ou32_PrevMsgTimeStamp, const uint32 ou32_NewMsgTimeStamp, const sint32 os32_MultiplexerValue,
   bool & orq_UpdateDataTimeStamp)
{
   C_CamMetTreeLoggerDataGreyOutInformation c_Retval;

   orq_UpdateDataTimeStamp = false;

   c_Retval.sn_GrayOutValueMsg = orc_MessagePrev.c_GreyOutInformation.sn_GrayOutValueMsg;
   c_Retval.c_MapMultiplexerValueToGrayOutValue =
      orc_MessagePrev.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue;
   c_Retval.c_GrayOutValueDataBytes =
      orc_MessagePrev.c_GreyOutInformation.c_GrayOutValueDataBytes;

   c_Retval.c_MapMultiplexerValueToChangedTimeStamps =
      orc_MessagePrev.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps;

   // Check if the CAN message data changed
   if (orc_MessagePrev.c_CanDataHex != orc_MessageNew.c_CanDataHex)
   {
      // Old message with changed data. Update the timestamp of the CAN message data
      orq_UpdateDataTimeStamp = true;
   }
   else
   {
      // Using the previous timestamp again. It may not be overwritten by the new message with an
      // unset timestamp
      c_Retval.u32_DataChangedTimeStamp = orc_MessagePrev.c_GreyOutInformation.u32_DataChangedTimeStamp;
   }

   // Check if the CAN message data bytes changed
   c_Retval.c_DataBytesChangedTimeStamps.resize(8);

   //if old DLC < than new DLC
   if (orc_MessagePrev.c_CanMsg.u8_DLC < orc_MessageNew.c_CanMsg.u8_DLC)
   {
      //reset the gap between old and new dlc
      for (uint8 u8_DbCounter = orc_MessagePrev.c_CanMsg.u8_DLC; u8_DbCounter < orc_MessageNew.c_CanMsg.u8_DLC;
           ++u8_DbCounter)
      {
         // Set the current timestamp for the changed byte
         c_Retval.c_DataBytesChangedTimeStamps[u8_DbCounter] = ou32_PrevMsgTimeStamp;
      }
   }

   //reset the gap between old and new dlc
   for (uint8 u8_DbCounter = 0; u8_DbCounter < orc_MessagePrev.c_CanMsg.u8_DLC; ++u8_DbCounter)
   {
      //replace timestamp when databyte has changed
      if (orc_MessagePrev.c_CanMsg.au8_Data[u8_DbCounter] != orc_MessageNew.c_CanMsg.au8_Data[u8_DbCounter])
      {
         // Set the current timestamp for the changed byte
         c_Retval.c_DataBytesChangedTimeStamps[u8_DbCounter] = ou32_PrevMsgTimeStamp;
      }
      else
      {
         // Use the previous timestamp
         c_Retval.c_DataBytesChangedTimeStamps[u8_DbCounter] =
            orc_MessagePrev.c_GreyOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter];
      }
   }
   //Update specific timestamp of multiplexer map
   if (os32_MultiplexerValue >= 0)
   {
      c_Retval.c_MapMultiplexerValueToChangedTimeStamps[os32_MultiplexerValue] = ou32_NewMsgTimeStamp;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply previous gray out value to new message

   \param[in,out]  orc_Message                     Message to overwrite
   \param[in]      orc_StoredGrayOutInformation    Stored gray out information
   \param[in]      oq_UpdateDataTimeStamp          Flag to update the data timestamps
   \param[in]      ou32_PrevMsgTimeStamp           Previous message timestamp
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_ApplyPreviousGreyOutInformation(C_CamMetTreeLoggerData & orc_Message,
                                                           const C_CamMetTreeLoggerDataGreyOutInformation & orc_StoredGrayOutInformation, const bool oq_UpdateDataTimeStamp,
                                                           const uint32 ou32_PrevMsgTimeStamp)
{
   if (oq_UpdateDataTimeStamp == true)
   {
      // Update the timestamp. Update it after the assignment to avoid further temporary copies of the instance
      orc_Message.c_GreyOutInformation.u32_DataChangedTimeStamp = ou32_PrevMsgTimeStamp;
   }
   else
   {
      // Restore the previous timestamp
      orc_Message.c_GreyOutInformation.u32_DataChangedTimeStamp = orc_StoredGrayOutInformation.u32_DataChangedTimeStamp;
   }

   // Reassign the data byte timestamps
   for (uint8 u8_DbCounter = 0; u8_DbCounter < 8U; ++u8_DbCounter)
   {
      orc_Message.c_GreyOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter] =
         orc_StoredGrayOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter];
   }

   // Restore previous values after the assignment to avoid further temporary copies of the instance
   orc_Message.c_GreyOutInformation.sn_GrayOutValueMsg = orc_StoredGrayOutInformation.sn_GrayOutValueMsg;
   orc_Message.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue =
      orc_StoredGrayOutInformation.c_MapMultiplexerValueToGrayOutValue;
   orc_Message.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps =
      orc_StoredGrayOutInformation.c_MapMultiplexerValueToChangedTimeStamps;
   orc_Message.c_GreyOutInformation.c_GrayOutValueDataBytes = orc_StoredGrayOutInformation.c_GrayOutValueDataBytes;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the difference between the two vectors can be fixed by inserting a new item

   \param[in]   orc_ExpectedVec  Expected vector
   \param[in]   orc_CurrentVec   Current vector
   \param[out]  oru32_InsertAt   Position to insert new item at
   \param[out]  oru32_InsertNum  New value to insert at this position

   \retval true  A valid solution was found
   \retval false No valid solution could be found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::mh_CheckForFixByInsertingNewChild(const std::vector<uint32> & orc_ExpectedVec,
                                                          const std::vector<uint32> & orc_CurrentVec,
                                                          uint32 & oru32_InsertAt, uint32 & oru32_InsertNum)
{
   bool q_FixByInsertingNewChild = false;

   oru32_InsertAt = 0UL;
   oru32_InsertNum = 0UL;
   if (orc_ExpectedVec.size() == (orc_CurrentVec.size() + 1UL))
   {
      uint32 u32_Counter = 0UL;
      bool q_FoundMismatch = false;
      //Assuming we hit no problem
      q_FixByInsertingNewChild = true;
      //Default
      if (orc_ExpectedVec.size() == 1UL)
      {
         //If orc_CurrentVec is empty, that's also valid but needs to be handled
         oru32_InsertNum = orc_ExpectedVec[0UL];
      }
      else
      {
         //If all valid, use last item
         oru32_InsertAt = orc_CurrentVec.size();
         oru32_InsertNum = orc_ExpectedVec[static_cast<std::vector<uint32>::size_type>(orc_ExpectedVec.size() - 1UL)];
      }
      for (uint32 u32_ItCurrent = 0; (u32_ItCurrent < orc_CurrentVec.size()) && q_FixByInsertingNewChild;
           ++u32_ItCurrent)
      {
         if (u32_Counter < orc_ExpectedVec.size())
         {
            if (orc_ExpectedVec[u32_Counter] == orc_CurrentVec[u32_ItCurrent])
            {
               //pass
               ++u32_Counter;
            }
            else
            {
               //Only accept one conflict
               if (q_FoundMismatch)
               {
                  //No match possible
                  q_FixByInsertingNewChild = false;
               }
               else
               {
                  //Try insert here
                  oru32_InsertAt = u32_ItCurrent;
                  oru32_InsertNum = orc_ExpectedVec[u32_Counter];
                  ++u32_Counter;
                  q_FoundMismatch = true;
                  //Check the next to match with the current
                  if (u32_Counter < orc_ExpectedVec.size())
                  {
                     if (orc_ExpectedVec[u32_Counter] == orc_CurrentVec[u32_ItCurrent])
                     {
                        //next element matches again so continue
                        //pass
                        ++u32_Counter;
                     }
                     else
                     {
                        //The current element and the next expected element also don't match, so at least two operations
                        // are necessary
                        //No match possible
                        q_FixByInsertingNewChild = false;
                     }
                  }
                  else
                  {
                     //The current vector contains one more element than the expected and also has a mismatch, so at
                     // least two operations are necessary
                     //No match possible
                     q_FixByInsertingNewChild = false;
                  }
               }
            }
         }
         else
         {
            //No match possible
            q_FixByInsertingNewChild = false;
         }
      }
   }
   return q_FixByInsertingNewChild;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the new message content while keeping relevant signals from the previous message content

   \param[in,out]  orc_PreviousMessage    Previous message content
   \param[in]      orc_NewMessage         New message content
   \param[in]      os32_MuxValue          Mux value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_CopyMessageWhileKeepingUniqueSignals(C_CamMetTreeLoggerData & orc_PreviousMessage,
                                                                const C_CamMetTreeLoggerData & orc_NewMessage,
                                                                const sint32 os32_MuxValue)

{
   std::vector<C_OSCComMessageLoggerDataSignal> c_KeptSignals;

   //Only if mux value valid there is a chance that some signals should be kept
   if (os32_MuxValue >= 0)
   {
      //Look for signals which might not be present in the new message
      for (std::vector<C_OSCComMessageLoggerDataSignal>::iterator c_ItSig = orc_PreviousMessage.c_Signals.begin();
           c_ItSig != orc_PreviousMessage.c_Signals.end(); ++c_ItSig)
      {
         const C_OSCComMessageLoggerDataSignal & rc_Sig = *c_ItSig;
         if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
         {
            //Guaranteed to be replaced
         }
         else
         {
            if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               //Multiplexed
               if (rc_Sig.c_OscSignal.u16_MultiplexValue == static_cast<uint16>(os32_MuxValue))
               {
                  //Will be replaced
               }
               else
               {
                  //Keep information
                  c_KeptSignals.push_back(rc_Sig);
               }
            }
            else
            {
               //Multiplexer
               if (rc_Sig.c_RawValueDec.ToInt() == static_cast<uint16>(os32_MuxValue))
               {
                  //Will be replaced
               }
               else
               {
                  //Keep information
                  c_KeptSignals.push_back(rc_Sig);
               }
            }
         }
      }
   }

   //No new row: just replace the last known "newest" message at this ID
   orc_PreviousMessage = orc_NewMessage;
   //Reserve
   orc_PreviousMessage.c_Signals.reserve(orc_PreviousMessage.c_Signals.size() + c_KeptSignals.size());
   //Append left signals to merge
   for (std::vector<C_OSCComMessageLoggerDataSignal>::const_iterator c_ItSig = c_KeptSignals.begin();
        c_ItSig != c_KeptSignals.end(); ++c_ItSig)
   {
      orc_PreviousMessage.c_Signals.push_back(*c_ItSig);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate tree rows to signal index

   \param[in]  orc_Signals          Signals
   \param[in]  os32_SignalIndex     Signal index
   \param[in]  os32_SignalIndexL2   Second level row (optional)

   \return
   Translated signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_CamMetTreeModel::mh_TranslateTreeRowsToSignalIndex(
   const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals, const sint32 os32_SignalIndex,
   const sint32 os32_SignalIndexL2)
{
   uint32 u32_Retval = 0UL;
   const std::vector<sintn> c_Order = C_CamMetUtil::h_GetMultiplexerOrder(orc_Signals);
   uint32 u32_Counter = 0UL;

   if (static_cast<uint32>(os32_SignalIndex) < c_Order.size())
   {
      //Part of multiplexer
      for (uint32 u32_ItSigL1 = 0UL; u32_ItSigL1 < orc_Signals.size(); ++u32_ItSigL1)
      {
         const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Signals[u32_ItSigL1];
         if (os32_SignalIndexL2 >= 0)
         {
            if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               //Multiplexed
               if (rc_SignalData.c_OscSignal.u16_MultiplexValue == c_Order[static_cast<uint32>(os32_SignalIndex)])
               {
                  if (u32_Counter == static_cast<uint32>(os32_SignalIndexL2))
                  {
                     u32_Retval = u32_ItSigL1;
                     break;
                  }
                  else
                  {
                     //Only count children (= multiplexed signals with this multiplexer value)
                     ++u32_Counter;
                  }
               }
            }
         }
         else
         {
            //Multiplexer
            if ((rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
                (rc_SignalData.c_RawValueDec.ToInt() == c_Order[static_cast<uint32>(os32_SignalIndex)]))
            {
               //Just use first multiplexer
               u32_Retval = u32_ItSigL1;
               //lint -e{1960} break is more efficient
               break;
            }
         }
      }
   }
   else
   {
      //Skip multiplexer in counting
      u32_Counter += c_Order.size();
      //Others
      for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Signals.size(); ++u32_ItSig)
      {
         const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Signals[u32_ItSig];
         if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_DEFAULT)
         {
            if (u32_Counter == static_cast<uint32>(os32_SignalIndex))
            {
               u32_Retval = u32_ItSig;
               break;
            }
            else
            {
               ++u32_Counter;
            }
         }
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a message is multiplexed

   \param[in]     orc_Data              Message to search

   \retval   true    The message or one signal does match to the search string
   \retval   false   The message and its signals does not match to search string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::mh_IsMessageMultiplexed(const C_CamMetTreeLoggerData & orc_Data)
{
   bool q_Return = false;
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < orc_Data.c_Signals.size(); ++u32_Counter)
   {
      if (orc_Data.c_Signals[u32_Counter].c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         q_Return = true;
         break;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sorts the signals in order of the tree

  Example:
   - Multiplexer signal
   -- Multiplexed signal
   -- Multiplexed signal
   -- Multiplexed signal
   - Multiplexer signal
   -- Multiplexed signal
   -- Multiplexed signal
   - Default signal
   - Default signal
   - Default signal

   \param[in,out]   orc_Signals   Vector with signals to sort (input sorted by start bit)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_SortMultiplexedSignals(std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals)
{
   uint32 u32_MultiplexerCounter = 0U;
   uint32 u32_SignalCounter;

   //lint -e{864} Call as expected by interface
   std::sort(orc_Signals.begin(), orc_Signals.end(), T_LoggerDataComparatorForMultiplexer());

   // Search all multiplexed signal for each the multiplexer. Iterate over the multiplexer signals
   while (u32_MultiplexerCounter < orc_Signals.size())
   {
      const C_OSCComMessageLoggerDataSignal & rc_MultiplexerSig = orc_Signals[u32_MultiplexerCounter];
      if (rc_MultiplexerSig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         const sintn sn_MultiplexerValue = rc_MultiplexerSig.c_RawValueDec.ToIntDef(-1);

         if (sn_MultiplexerValue >= 0)
         {
            // Compare the multiplexer value of this multiplexer signal with all multiplexed signals
            for (u32_SignalCounter = u32_MultiplexerCounter + 1U; u32_SignalCounter < orc_Signals.size();
                 ++u32_SignalCounter)
            {
               if ((orc_Signals[u32_SignalCounter].c_OscSignal.e_MultiplexerType ==
                    C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                   (orc_Signals[u32_SignalCounter].c_OscSignal.u16_MultiplexValue ==
                    static_cast<uint16>(sn_MultiplexerValue)))
               {
                  const C_OSCComMessageLoggerDataSignal c_SigCopy = orc_Signals[u32_SignalCounter];

                  // Remove the signal from the original position
                  orc_Signals.erase(orc_Signals.begin() + u32_SignalCounter);

                  // Increment the multiplexer counter, due to the insert
                  ++u32_MultiplexerCounter;

                  // Insert this signal after the multiplexer signal again
                  orc_Signals.insert(orc_Signals.begin() + u32_MultiplexerCounter, c_SigCopy);
               }
            }
         }

         // Check next element
         ++u32_MultiplexerCounter;
      }
      else
      {
         // All multiplexer signals checked. Finished
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Custom operator for sorting C_OSCComMessageLoggerDataSignal

   All multiplexer signals will be sorted to the beginning (less than) in order of the set multiplexer value

   \param[in]       orc_Data1     Detailed input parameter description
   \param[in]       orc_Data2     Detailed input parameter description

   \retval   true    orc_Data1 is less than orc_Data2
   \retval   false   orc_Data2 is less or equal than orc_Data1
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::T_LoggerDataComparatorForMultiplexer::operator ()(
   const stw_opensyde_core::C_OSCComMessageLoggerDataSignal & orc_Data1,
   const stw_opensyde_core::C_OSCComMessageLoggerDataSignal & orc_Data2) const
{
   bool q_Return;

   if ((orc_Data1.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
       (orc_Data2.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL))
   {
      // Both are multiplexer signals, compare the values
      const sintn sn_Value1 = orc_Data1.c_RawValueDec.ToIntDef(-1);
      const sintn sn_Value2 = orc_Data2.c_RawValueDec.ToIntDef(-1);

      q_Return = sn_Value1 < sn_Value2;
   }
   else if (orc_Data1.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
   {
      // orc_Data2 is not a multiplexer signal
      q_Return = true;
   }
   else if (orc_Data2.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
   {
      // orc_Data1 is not a multiplexer signal
      q_Return = false;
   }
   else
   {
      // Default compare
      q_Return = orc_Data1 < orc_Data2;
   }

   return q_Return;
}
