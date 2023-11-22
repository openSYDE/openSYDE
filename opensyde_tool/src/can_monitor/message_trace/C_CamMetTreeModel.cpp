//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance model (implementation)

   Max performance model

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <sstream>
#include <iomanip>

#include <QBitArray>

#include "TglTime.hpp"
#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_CamMetUtil.hpp"
#include "cam_constants.hpp"
#include "C_CanMonProtocol.hpp"
#include "C_CamMetTreeModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CamMetTreeModel::mhc_ICON_MESSAGE = ":images/IconMessageInactive.svg";
const QString C_CamMetTreeModel::mhc_ICON_MESSAGE_SELECTED = "://images/IconMessageSelected.svg";
const QString C_CamMetTreeModel::mhc_ICON_SIGNAL = ":images/IconSignalInactive.svg";
const QString C_CamMetTreeModel::mhc_ICON_SIGNAL_SELECTED = "://images/IconSignalSelected.svg";

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
   ms32_LastSearchedMessageRow(-1),
   ms32_LastSearchedSignalRow(-1),
   ms32_LastSearchedSignalRowMultiplexed(-1),
   mq_IsLastSearchedSignalLastOfMessage(true)
{
   int32_t s32_Counter;

   this->m_AdaptTraceBufferSize();

   // Prepare the transparency colors for the gray out font
   this->mc_FontTransparcencyColors.resize((ms32_TRACE_TRANSPARENCY_START - ms32_TRACE_TRANSPARENCY_END) + 1,
                                           mc_STYLE_GUIDE_COLOR_1);

   // Value on position 0 equals no transparency and at the end equals maximum transparency
   for (s32_Counter = 0U; s32_Counter < static_cast<int32_t>(this->mc_FontTransparcencyColors.size()); ++s32_Counter)
   {
      this->mc_FontTransparcencyColors[s32_Counter].setAlpha(ms32_TRACE_TRANSPARENCY_START - s32_Counter);
   }

   //Default
   this->mpc_InvisibleRootItem = this->mpc_RootItemContinuous;

   connect(&this->mc_GrayOutTimer, &QTimer::timeout, this, &C_CamMetTreeModel::m_GrayOutTimer);
   this->mc_GrayOutTimer.start(ms32_TRACE_TRANSPARENCY_REFRESH_TIME);
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
   // The TglGetTickCountUs can have a difference of 1 to 2 seconds because of a different rounding
   const uint32_t u32_CurrentTime = static_cast<uint32_t>(TglGetTickCountUs() / 1000ULL);
   // Get the time interval of the entire pause
   const uint32_t u32_GrayOutPauseOffset = u32_CurrentTime - this->mu32_GrayOutPauseTimeStamp;

   QMap<C_SclString, C_CamMetTreeLoggerData>::iterator c_It;

   // Adapt all timestamps used by the gray out engine
   for (c_It = this->mc_UniqueMessages.begin(); c_It != this->mc_UniqueMessages.end(); ++c_It)
   {
      C_CamMetTreeLoggerData & rc_Data = c_It.value();
      int32_t s32_CounterDataByte;

      // Timestamp of CAN message
      // The original timestamp of the message can be adapted, it used only for the gray out engine
      // For showing the timestamp, the prepared and already calculated timestamps will be used
      rc_Data.c_CanMsg.u64_TimeStamp += static_cast<uint64_t>(u32_GrayOutPauseOffset) * 1000ULL;

      // Timestamp of all data bytes
      rc_Data.c_GreyOutInformation.u32_DataChangedTimeStamp += u32_GrayOutPauseOffset;

      // Timestamp of each data byte
      for (s32_CounterDataByte = 0; s32_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++s32_CounterDataByte)
      {
         rc_Data.c_GreyOutInformation.c_DataBytesChangedTimeStamps[s32_CounterDataByte] += u32_GrayOutPauseOffset;
      }
      // Update all timestamps of multiplexer map
      for (std::map<int32_t, uint32_t>::iterator c_ItVal =
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
   // The TglGetTickCountUs can have a difference of 1 to 2 seconds because of a different rounding
   this->mu32_GrayOutPauseTimeStamp = static_cast<uint32_t>(TglGetTickCountUs() / 1000ULL);
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
std::vector<int32_t> C_CamMetTreeModel::AddRows(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   const std::vector<int32_t> c_Retval = this->m_AddRowsContinuousMode(orc_Data);

   this->m_AddRowsUnique(orc_Data);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current selection

   \param[in]  os32_SelectedParentRow     Current selected parent index
   \param[in]  os32_SelectedChildIndex    Current selected child index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetSelection(const int32_t os32_SelectedParentRow, const int32_t os32_SelectedChildIndex)
{
   const int32_t s32_ColIdData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   this->ms32_SelectedParentRow = os32_SelectedParentRow;
   this->mc_SelectedChildBytes.clear();
   if (os32_SelectedParentRow >= 0)
   {
      const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(os32_SelectedParentRow);

      if (pc_CurMessage != NULL)
      {
         //Check which child/signal should be selected
         const uint32_t u32_ChildIndex = static_cast<uint32_t>(os32_SelectedChildIndex);

         //Check if the signal is valid
         if (u32_ChildIndex < pc_CurMessage->c_Signals.size())
         {
            std::set<uint16_t> c_Bits;
            const C_OscComMessageLoggerDataSignal & rc_CurSignal = pc_CurMessage->c_Signals[u32_ChildIndex];
            //Get all bit positions as intel
            rc_CurSignal.c_OscSignal.GetDataBytesBitPositionsOfSignal(c_Bits);
            //Convert bit positions into byte indices
            for (std::set<uint16_t>::const_iterator c_ItBit = c_Bits.begin(); c_ItBit != c_Bits.end();
                 ++c_ItBit)
            {
               this->mc_SelectedChildBytes.insert(*c_ItBit / 8U);
            }
         }
      }
   }
   //Update data row
   this->dataChanged(this->index(0, s32_ColIdData), this->index(this->rowCount() - 1, s32_ColIdData));
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
   const int32_t s32_ColCanId = C_CamMetTreeModel::h_EnumToColumn(eCAN_ID);
   const int32_t s32_ColDataId = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   //Update value
   this->mq_DisplayAsHex = oq_Value;

   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColCanId), this->index(this->rowCount() - 1, s32_ColCanId));
   Q_EMIT this->dataChanged(this->index(0, s32_ColDataId), this->index(this->rowCount() - 1, s32_ColDataId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for timestamp

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayTimestampRelative(const bool oq_Value)
{
   const int32_t s32_ColId = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);

   //Update value
   this->mq_DisplayTimestampRelative = oq_Value;
   //Update UI
   Q_EMIT (this->dataChanged(this->index(0, s32_ColId), this->index(this->rowCount() - 1, s32_ColId)));
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
      const int32_t s32_ColId = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      //Update UI
      Q_EMIT (this->dataChanged(this->index(0, s32_ColId), this->index(this->rowCount() - 1, s32_ColId)));
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
void C_CamMetTreeModel::SetTraceBufferSize(const uint32_t ou32_Value)
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
   const int32_t s32_CompleteSize = static_cast<int32_t>(this->mc_DataBase.size()) + this->mc_UniqueMessages.size();
   //Reserve
   c_Retval.reserve(s32_CompleteSize);
   //Append vector
   for (uint32_t u32_ItVec = 0; u32_ItVec < this->mc_DataBase.size(); ++u32_ItVec)
   {
      c_Retval.push_back(&this->mc_DataBase[u32_ItVec]);
   }
   //Append map
   for (QMap<C_SclString, C_CamMetTreeLoggerData>::iterator c_It = this->mc_UniqueMessages.begin();
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
   const int32_t s32_ColId = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColId), this->index(this->rowCount() - 1, s32_ColId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamMetTreeModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                       const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))

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
         case eCAN_STATUS:
            c_Retval = C_GtGetText::h_GetText("Status");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
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
         case eCAN_STATUS:
            c_Retval = C_GtGetText::h_GetText("Status");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
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
         case eCAN_STATUS:
            c_Retval = C_GtGetText::h_GetText("Indicates the validity of a (ECeS/ECoS) message.");
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

   \param[in]  os32_Row     Row
   \param[in]  os32_Column  Column
   \param[in]  orc_Parent  Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMetTreeModel::index(const int32_t os32_Row, const int32_t os32_Column,
                                     const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (this->mq_DisplayTree)
   {
      c_Retval = C_TblTreSimpleModel::index(os32_Row, os32_Column, orc_Parent);
   }
   else
   {
      c_Retval = this->createIndex(os32_Row, os32_Column);
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
int32_t C_CamMetTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (orc_Parent.isValid() == false)
   {
      //Top level
      s32_Retval = 8;
   }
   else if (orc_Parent.parent().isValid() == false)
   {
      s32_Retval = 1;
   }
   else if (orc_Parent.parent().parent().isValid() == false)
   {
      s32_Retval = 1;
   }
   else
   {
      // Nothing to do
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMetTreeModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (this->mq_DisplayTree == true)
   {
      s32_Retval = C_TblTreSimpleModel::rowCount(orc_Parent);
   }
   else if (orc_Parent.isValid() == false)
   {
      //Top level
      if (this->mq_UniqueMessageMode == true)
      {
         s32_Retval = this->mc_UniqueMessages.size();
      }
      else
      {
         s32_Retval = this->mc_DataBase.size();
      }
   }
   else
   {
      // Nothing to do
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_CamMetTreeModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if ((mq_DataUnlocked == true) || (os32_Role == ms32_USER_ROLE_SORT))
   {
      if (((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == ms32_USER_ROLE_MARKER_TEXT)) ||
          (os32_Role == ms32_USER_ROLE_SORT))
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
                  if (os32_Role == ms32_USER_ROLE_SORT)
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
                         ((orc_Index.data(ms32_USER_ROLE_MARKER).toBitArray().isEmpty() == true) ||
                          (os32_Role == ms32_USER_ROLE_MARKER_TEXT)))
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
                  if (os32_Role == ms32_USER_ROLE_SORT)
                  {
                     c_Retval = pc_CurMessage->c_Counter.ToInt64();
                  }
                  else
                  {
                     c_Retval = pc_CurMessage->c_Counter.c_str();
                  }
                  break;
               case eCAN_STATUS:
                  c_Retval = pc_CurMessage->c_Status.c_str();
                  break;
               default:
                  break;
               }
            }
         }
      }
      else if (((os32_Role == ms32_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1) ||
                (os32_Role == ms32_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_2)) ||
               (os32_Role == ms32_USER_ROLE_STRING_PARTS))
      {
         if (orc_Index.parent().isValid() == true)
         {
            //Top level parent == message
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(
               orc_Index.parent().parent().isValid() ? orc_Index.parent().parent().row() : orc_Index.parent().row());

            if (pc_CurMessage != NULL)
            {
               if ((os32_Role == ms32_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1) ||
                   (os32_Role == ms32_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_2))
               {
                  const std::vector<QStringList> c_ColWidths = mh_GetCount(*pc_CurMessage);
                  if (c_ColWidths.size() == 2)
                  {
                     if (os32_Role == ms32_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1)
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
                  const C_OscComMessageLoggerDataSignal & rc_CurSignal =
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
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         if (orc_Index.parent().isValid() == false)
         {
            const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
            if (e_Col == eCAN_DATA)
            {
               c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
            }
            else
            {
               //Use parent value
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_MARKER)
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
                  for (std::set<uint16_t>::const_iterator c_ItByte = this->mc_SelectedChildBytes.begin();
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
      else if (os32_Role == ms32_USER_ROLE_MARKER_TRANSPARENCY)
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
               for (std::vector<int32_t>::const_iterator c_ItVal =
                       pc_CurMessage->c_GreyOutInformation.c_GrayOutValueDataBytes.begin();
                    c_ItVal != pc_CurMessage->c_GreyOutInformation.c_GrayOutValueDataBytes.end(); ++c_ItVal)
               {
                  c_Array.push_back(static_cast<char_t>(*c_ItVal));
               }
               c_Retval = c_Array;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
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
                     if (pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg <
                         static_cast<int32_t>(this->mc_FontTransparcencyColors.size()))
                     {
                        c_Retval =
                           this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg];
                     }
                  }
               }
               else if (e_Col == eCAN_STATUS)
               {
                  if (this->m_IsStatusValid(*pc_CurMessage) == false)
                  {
                     c_Retval = mc_STYLE_GUIDE_COLOR_18;
                  }
               }
               else if (e_Col == eCAN_NAME)
               {
                  if (this->m_IsStatusValid(*pc_CurMessage) == false)
                  {
                     c_Retval = mc_STYLE_GUIDE_COLOR_18;
                  }
               }
               else
               {
                  if (pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg <
                      static_cast<int32_t>(this->mc_FontTransparcencyColors.size()))
                  {
                     c_Retval =
                        this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg];
                  }
               }
            }
         }
         else
         {
            const int32_t s32_MessageRow = static_cast<int32_t>(
               orc_Index.parent().parent().isValid() ? orc_Index.parent().parent().row() : orc_Index.parent().row());
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(s32_MessageRow);
            if (pc_CurMessage != NULL)
            {
               const uint32_t u32_SignalIndex = this->TranslateTreeRowsToSignalIndex(orc_Index);
               if (u32_SignalIndex < pc_CurMessage->c_Signals.size())
               {
                  const C_OscComMessageLoggerDataSignal & rc_Signal = pc_CurMessage->c_Signals[u32_SignalIndex];
                  if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT)
                  {
                     //Use message value
                     c_Retval =
                        this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg];
                  }
                  else
                  {
                     //Special handling necessary
                     int32_t s32_MuxValue = -1;
                     if (rc_Signal.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
                     {
                        if (rc_Signal.q_DlcError == false)
                        {
                           s32_MuxValue = rc_Signal.c_RawValueDec.ToInt();
                        }
                     }
                     else
                     {
                        const uint32_t u32_MultiplexerSignalIndex = this->TranslateTreeRowsToSignalIndex(
                           orc_Index.parent());
                        const C_OscComMessageLoggerDataSignal & rc_MultiplexerSignal =
                           pc_CurMessage->c_Signals[u32_MultiplexerSignalIndex];
                        if (rc_MultiplexerSignal.q_DlcError == false)
                        {
                           s32_MuxValue = rc_MultiplexerSignal.c_RawValueDec.ToInt();
                        }
                     }
                     const std::map<int32_t,
                                    int32_t>::const_iterator c_It =
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
                           this->mc_FontTransparcencyColors[pc_CurMessage->c_GreyOutInformation.s32_GrayOutValueMsg];
                     }
                  }
               }
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::DecorationRole))
      {
         //Although you should never see this icon we still need to return something so the default drawing engine
         // reserves the space for the manually drawn icon
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (e_Col == eTIME_STAMP)
         {
            if (orc_Index.parent().isValid() == false)
            {
               // Message
               c_Retval = C_CamMetTreeModel::mhc_ICON_MESSAGE;
            }
            else
            {
               // Signal
               c_Retval = C_CamMetTreeModel::mhc_ICON_SIGNAL;
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (e_Col == eTIME_STAMP)
         {
            QStringList c_Tmp;
            if (orc_Index.parent().isValid() == false)
            {
               // Message
               c_Tmp.append(C_CamMetTreeModel::mhc_ICON_MESSAGE);
               c_Tmp.append(C_CamMetTreeModel::mhc_ICON_MESSAGE_SELECTED);
            }
            else
            {
               // Signal
               c_Tmp.append(C_CamMetTreeModel::mhc_ICON_SIGNAL);
               c_Tmp.append(C_CamMetTreeModel::mhc_ICON_SIGNAL_SELECTED);
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
C_CamMetTreeModel::E_Columns C_CamMetTreeModel::h_ColumnToEnum(const int32_t os32_Column)
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
      e_Retval = eCAN_COUNTER;
      break;
   default:
      e_Retval = eCAN_STATUS;
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
int32_t C_CamMetTreeModel::h_EnumToColumn(const C_CamMetTreeModel::E_Columns oe_Value)
{
   int32_t s32_Retval;

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
   case eCAN_STATUS:
      s32_Retval = 7;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message at row

   \param[in]  os32_Row  Current row

   \return
   NULL Message not found
   Else Valid message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CamMetTreeLoggerData * C_CamMetTreeModel::GetMessageData(const int32_t os32_Row) const
{
   const C_CamMetTreeLoggerData * pc_Retval;

   if (this->mq_UniqueMessageMode == false)
   {
      //If not unique messages: look in "queue"/vector
      const uint32_t u32_Index = (this->mu32_OldestItemIndex + static_cast<uint32_t>(os32_Row)) %
                                 this->mu32_TraceBufferSizeUsed;
      pc_Retval = &this->mc_DataBase[u32_Index];
   }
   else
   {
      //If unique messages: look in map
      const QMap<C_SclString, C_SclString>::const_iterator c_ItOrder =
         this->mc_UniqueMessagesOrdering.begin() + os32_Row;
      const QMap<C_SclString, C_CamMetTreeLoggerData>::const_iterator c_It =
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
   \param[out]      ors32_SignalRow               < 0: No signal of the message as search result
                                                >= 0: If ors32_MultiplexedSignalRow >= 0: Row of Multiplexer signal
                                                      If ors32_MultiplexedSignalRow < 0: Row of checked signal
   \param[out]      ors32_MultiplexedSignalRow   >= 0: Row of Multiplexed signal
                                                 < 0: Signal is not a multiplexed signal

   \retval  >= 0   Message found, row returned
   \retval  < 0    No Message found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMetTreeModel::SearchMessageData(const QString & orc_SearchString, const bool oq_Next,
                                             int32_t & ors32_SignalRow, int32_t & ors32_MultiplexedSignalRow)
{
   int32_t s32_Row = -1;
   const uint32_t u32_NumberEntries = this->rowCount();
   uint32_t u32_NumberEntriesToCheck = u32_NumberEntries;
   const int32_t s32_START_AT_LAST_SIGNAL = -2;

   ors32_SignalRow = -1;
   ors32_MultiplexedSignalRow = -1;

   if (u32_NumberEntries > 0U)
   {
      uint32_t u32_Counter;
      uint32_t u32_StartIndex;
      // -1 means no signal was the match at the last search
      int32_t s32_CurrentSignalIndex = -1;
      int32_t s32_CurrentSignalIndexMultiplexed = -1;

      if ((this->mc_LastSearchedString == orc_SearchString) &&
          (this->ms32_LastSearchedMessageRow >= 0))
      {
         // In case of the same search string, continue search with next or previous index
         if (oq_Next == true)
         {
            // Forward
            if (this->mq_IsLastSearchedSignalLastOfMessage == true)
            {
               // Start with message itself again
               u32_StartIndex = (static_cast<uint32_t>(this->ms32_LastSearchedMessageRow) + 1) % u32_NumberEntries;
            }
            else
            {
               // Start search with next available signal of last message
               u32_StartIndex = static_cast<uint32_t>(this->ms32_LastSearchedMessageRow);
               s32_CurrentSignalIndex = this->ms32_LastSearchedSignalRow + 1;
               s32_CurrentSignalIndexMultiplexed = this->ms32_LastSearchedSignalRowMultiplexed + 1;
               if (s32_CurrentSignalIndex >= 0)
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
            if (this->ms32_LastSearchedSignalRow < 0)
            {
               // Prevent a minus value here
               if (this->ms32_LastSearchedMessageRow == 0)
               {
                  u32_StartIndex = u32_NumberEntries - 1U;
               }
               else
               {
                  u32_StartIndex = (static_cast<uint32_t>(this->ms32_LastSearchedMessageRow) - 1);
               }

               // special case: starting at the last signal of the message
               // but at this point the number of signals of the messages is here not available
               s32_CurrentSignalIndex = s32_START_AT_LAST_SIGNAL;
               s32_CurrentSignalIndexMultiplexed = s32_START_AT_LAST_SIGNAL;
            }
            else
            {
               // Start search with previous available signal of last message or the message itself (-1)
               u32_StartIndex = static_cast<uint32_t>(this->ms32_LastSearchedMessageRow);
               s32_CurrentSignalIndex = this->ms32_LastSearchedSignalRow - 1;
               s32_CurrentSignalIndexMultiplexed = this->ms32_LastSearchedSignalRowMultiplexed - 1;

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
         uint32_t u32_Row;
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
            const uint32_t u32_Offset = (u32_NumberEntries - u32_StartIndex);
            const uint32_t u32_RevCounter = (u32_NumberEntries - u32_Counter);

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
               s32_CurrentSignalIndex = s32_START_AT_LAST_SIGNAL;
               s32_CurrentSignalIndexMultiplexed = s32_START_AT_LAST_SIGNAL;
            }
         }

         pc_Data = this->GetMessageData(u32_Row);

         if (pc_Data != NULL)
         {
            bool q_SearchMessageResult;

            if (s32_CurrentSignalIndex <= s32_START_AT_LAST_SIGNAL)
            {
               // special case: starting at the last signal of the message due to backward search
               s32_CurrentSignalIndex = static_cast<int32_t>(pc_Data->c_Signals.size()) - 1;
               s32_CurrentSignalIndexMultiplexed = s32_CurrentSignalIndex;
            }

            // Search the message and all of its signals till something was found or no signals are left to check
            q_SearchMessageResult = this->m_CheckDataForSearch(*pc_Data, orc_SearchString, oq_Next,
                                                               s32_CurrentSignalIndex,
                                                               this->mq_IsLastSearchedSignalLastOfMessage);

            if (q_SearchMessageResult == true)
            {
               s32_Row = static_cast<int32_t>(u32_Row);

               // Save the match for next call
               this->ms32_LastSearchedMessageRow = s32_Row;
               this->ms32_LastSearchedSignalRow = s32_CurrentSignalIndex;
               // Set the multiplexed value in case of a non multiplexed message
               this->ms32_LastSearchedSignalRowMultiplexed = this->ms32_LastSearchedSignalRow;
               this->mc_LastSearchedString = orc_SearchString;

               if (s32_CurrentSignalIndex >= 0)
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
                                                                        s32_CurrentSignalIndexMultiplexed,
                                                                        q_LastMessageDummy);
                     Q_UNUSED(q_LastMessageDummy)

                     if (q_SearchMessageResult == true)
                     {
                        // Should be a true again
                        this->ms32_LastSearchedSignalRowMultiplexed = s32_CurrentSignalIndexMultiplexed;

                        // A concrete signal of the message was found
                        // Get the correct row for the found signal
                        this->m_GetMultiplexedMsgSignalRow(c_CopyData.c_Signals,
                                                           static_cast<uint32_t>(s32_CurrentSignalIndexMultiplexed),
                                                           ors32_SignalRow, ors32_MultiplexedSignalRow);
                     }
                     else
                     {
                        // Should not happen
                        ors32_SignalRow = -1;
                        ors32_MultiplexedSignalRow = -1;
                     }
                  }
                  else
                  {
                     // No multiplexed message, order of vector equals order of tree
                     ors32_SignalRow = s32_CurrentSignalIndex;
                  }
               }

               break;
            }
            else
            {
               s32_CurrentSignalIndexMultiplexed = -1;
            }
         }
      }
   }

   if (s32_Row == -1)
   {
      this->m_ResetSearch();
   }

   return s32_Row;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate message tree indices to signal index

   \param[in]  orc_CurrentIndex  Current child index to get signal index for

   \return
   Translated signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamMetTreeModel::TranslateTreeRowsToSignalIndex(const QModelIndex & orc_CurrentIndex) const
{
   uint32_t u32_Retval;

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
uint32_t C_CamMetTreeModel::TranslateTreeRowsToSignalIndex(const int32_t os32_MessageIndex,
                                                           const int32_t os32_SignalIndex,
                                                           const int32_t os32_SignalIndexL2) const
{
   uint32_t u32_Retval = 0UL;
   const C_CamMetTreeLoggerData * const pc_Message = this->GetMessageData(os32_MessageIndex);

   if (pc_Message != NULL)
   {
      u32_Retval = C_CamMetTreeModel::mh_TranslateTreeRowsToSignalIndex(pc_Message->c_Signals, os32_SignalIndex,
                                                                        os32_SignalIndexL2);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Drag functionality using flags set from base classes

   \param[in]       orc_Index     Index of selected row

   \return
   retun QT::ItemFlags

   \retval   return OR operation of Qt::ItemIsDragEnabled and QAbstractItemModel::flags(index)
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_CamMetTreeModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_ItemFlagsResult;
   if (!orc_Index.isValid())
   {
      c_ItemFlagsResult = Qt::NoItemFlags;
   }
   else
   {
      c_ItemFlagsResult = Qt::ItemIsDragEnabled | QAbstractItemModel::flags(orc_Index);
   }

   return c_ItemFlagsResult;
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
std::vector<int32_t> C_CamMetTreeModel::m_AddRowsContinuousMode(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   std::vector<int32_t> c_Retval;
   if (orc_Data.empty() == false)
   {
      //Columns which need to get updated on change of static message
      const uint32_t u32_CompleteSize = static_cast<uint32_t>(this->mc_DataBase.size()) +
                                        static_cast<uint32_t>(orc_Data.size());

      //Check if only append case
      if (u32_CompleteSize <= this->mu32_TraceBufferSizeUsed)
      {
         //Appending so notify model of insert action
         if (this->mq_UniqueMessageMode == false)
         {
            this->beginInsertRows(QModelIndex(), this->rowCount(),
                                  this->rowCount() + (static_cast<int32_t>(orc_Data.size()) - 1));
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
         int32_t s32_Start = static_cast<int32_t>(this->rowCount()) - (static_cast<int32_t>(orc_Data.size()) - 1);
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
         const QMap<stw::scl::C_SclString,
                    C_CamMetTreeLoggerData>::const_iterator c_ItMessage = this->mc_UniqueMessages.find(
            c_ItData->c_CanIdDec);
         const int32_t s32_MuxValue = C_CamMetUtil::h_GetMultiplexerValue(c_ItData->c_Signals);

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
                                                                         const stw::scl::C_SclString & orc_ExistingMessageKey,
                                                                         const int32_t os32_MultiplexerValue)
{
   int32_t s32_MessageCounter = 0L;
   bool q_UpdateDataTimeStamp;
   C_CamMetTreeLoggerData & rc_Message = this->mc_UniqueMessages[orc_Message.c_CanIdDec];
   const uint64_t u64_PreviousAbsoluteTimestampValue = rc_Message.u64_TimeStampAbsoluteStart;
   const uint64_t u64_NewAbsoluteTimestampValue = orc_Message.u64_TimeStampAbsoluteStart;
   uint64_t u64_RelativeTimestamp;

   const uint32_t u32_PrevMsgTimeStamp = static_cast<uint32_t>(rc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);
   const uint32_t u32_NewMsgTimeStamp = static_cast<uint32_t>(orc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);

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
   rc_Message.c_TimeStampRelative = C_OscComMessageLoggerData::h_GetTimestampAsString(u64_RelativeTimestamp);

   //Search existing item index
   for (QMap<stw::scl::C_SclString,
             stw::scl::C_SclString>::const_iterator c_ItMsg = this->mc_UniqueMessagesOrdering.begin();
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
   m_UpdateTreeItemBasedOnMessage(this->mpc_RootItemStatic->c_Children[static_cast<uint32_t>(s32_MessageCounter)],
                                  rc_Message, true, s32_MessageCounter);
   //Notify the model of data changes for all replaced items (current: all items)
   if (this->mq_UniqueMessageMode == true)
   {
      const int32_t s32_ColIdName = C_CamMetTreeModel::h_EnumToColumn(eCAN_NAME);
      const int32_t s32_ColIdDlc = C_CamMetTreeModel::h_EnumToColumn(eCAN_DLC);
      const int32_t s32_ColIdData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
      const int32_t s32_ColIdTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      const int32_t s32_ColIdCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
      const int32_t s32_ColIdStatus = C_CamMetTreeModel::h_EnumToColumn(eCAN_STATUS);
      //Notification for possible new child
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdTime), this->index(s32_MessageCounter, s32_ColIdTime));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdCounter),
                        this->index(s32_MessageCounter, s32_ColIdCounter));
      //Possible changes
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdName), this->index(s32_MessageCounter, s32_ColIdName));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdDlc), this->index(s32_MessageCounter, s32_ColIdDlc));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdData), this->index(s32_MessageCounter, s32_ColIdData));
      this->dataChanged(this->index(s32_MessageCounter, s32_ColIdStatus),
                        this->index(s32_MessageCounter, s32_ColIdStatus));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new unique message entry

   \param[in]  orc_Message             New message
   \param[in]  os32_MultiplexerValue   Current multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_HandleNewUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                 const int32_t os32_MultiplexerValue)
{
   bool q_NewRow = false;

   uint8_t u8_DbCounter;

   QMap<C_SclString, C_CamMetTreeLoggerData>::iterator c_NewPos;
   //Handle begin!
   C_TblTreSimpleItem * const pc_NewItem = new C_TblTreSimpleItem();
   const int32_t s32_EstimatedPosIndex = this->m_GetPosIndexForUniqueMessage(orc_Message.c_TimeStampAbsoluteStart);
   //Update tree with known index
   m_UpdateTreeItemBasedOnMessage(pc_NewItem, orc_Message, false, s32_EstimatedPosIndex);
   this->mpc_RootItemStatic->InsertChild(s32_EstimatedPosIndex, pc_NewItem);

   if (this->mq_UniqueMessageMode == true)
   {
      //handle new row with known position index
      q_NewRow = true;

      //Notify the new row count
      this->beginInsertRows(QModelIndex(), s32_EstimatedPosIndex, s32_EstimatedPosIndex);
   }

   //Insert new item
   this->mc_UniqueMessagesOrdering.insert(orc_Message.c_TimeStampAbsoluteStart, orc_Message.c_CanIdDec);
   c_NewPos = this->mc_UniqueMessages.insert(orc_Message.c_CanIdDec, orc_Message);
   // New message, new data. Update the timestamp of the CAN message data and its bytes
   c_NewPos->c_GreyOutInformation.u32_DataChangedTimeStamp =
      static_cast<uint32_t>(c_NewPos->c_CanMsg.u64_TimeStamp / 1000ULL);
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
int32_t C_CamMetTreeModel::m_GetPosIndexForUniqueMessage(const C_SclString & orc_AbsoluteTimeStamp) const
{
   int32_t s32_EstimatedPosIndex = 0;
   //Get first valid item after the position it should be at
   const QMap<C_SclString, C_SclString>::const_iterator c_EstimatedPos = this->mc_UniqueMessagesOrdering.upperBound(
      orc_AbsoluteTimeStamp);

   //Calc estimated new position index
   for (QMap<C_SclString, C_SclString>::const_iterator c_ItUniqueMessages = this->mc_UniqueMessagesOrdering.begin();
        (c_ItUniqueMessages != this->mc_UniqueMessagesOrdering.end()) && (c_ItUniqueMessages != c_EstimatedPos);
        ++c_ItUniqueMessages)
   {
      ++s32_EstimatedPosIndex;
   }
   return s32_EstimatedPosIndex;
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
      // The TglGetTickCountUs can have a difference of 1 to 2 seconds because of a different rounding
      const uint32_t u32_CurrentTime = static_cast<uint32_t>(TglGetTickCountUs() / 1000ULL);
      QMap<C_SclString, C_CamMetTreeLoggerData>::iterator c_It;
      int32_t s32_RowCounter = 0;

      for (c_It = this->mc_UniqueMessages.begin(); c_It != this->mc_UniqueMessages.end(); ++c_It)
      {
         C_CamMetTreeLoggerData & rc_Data = c_It.value();
         const uint32_t u32_DiffMsg = u32_CurrentTime - static_cast<uint32_t>(rc_Data.c_CanMsg.u64_TimeStamp / 1000ULL);
         // TimeStamp for data is set by AddRows when new CAN messages are added to the model
         int32_t s32_TransparencyStepMsg;
         int32_t s32_CounterDataByte;
         bool q_DataByteChangedTransparency = false;

         // Check message gray out adaption
         s32_TransparencyStepMsg = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffMsg);

         // Use each value and check if gray out value changed
         for (std::map<int32_t, uint32_t>::const_iterator c_ItValue =
                 rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.begin();
              c_ItValue != rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToChangedTimeStamps.end(); ++c_ItValue)
         {
            const uint32_t u32_DiffDataByte = u32_CurrentTime - c_ItValue->second;
            int32_t s32_TransparencyStepDataByte;

            s32_TransparencyStepDataByte = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffDataByte);

            if (s32_TransparencyStepDataByte !=
                rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue[c_ItValue->first])
            {
               QVector<int32_t> c_Roles;
               const int32_t s32_Row = C_CamMetUtil::h_GetRowForMultiplexerValue(rc_Data.c_Signals, c_ItValue->first);
               // Save the new value
               rc_Data.c_GreyOutInformation.c_MapMultiplexerValueToGrayOutValue[c_ItValue->first] =
                  s32_TransparencyStepDataByte;
               //Trigger ui update

               // Send the change only if the value changed really and let update the foreground only
               c_Roles.push_back(static_cast<int32_t>(Qt::ForegroundRole));
               this->dataChanged(this->index(s32_Row, 0, this->index(s32_RowCounter, 0)),
                                 this->index(s32_Row, 0, this->index(s32_RowCounter, 0)), c_Roles);
            }
         }

         // Check data byte gray out adaption
         for (s32_CounterDataByte = 0; s32_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++s32_CounterDataByte)
         {
            const uint32_t u32_DiffDataByte = u32_CurrentTime -
                                              rc_Data.c_GreyOutInformation.c_DataBytesChangedTimeStamps[
               s32_CounterDataByte
                                              ];
            int32_t s32_TransparencyStepDataByte;

            s32_TransparencyStepDataByte = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffDataByte);

            if (s32_TransparencyStepDataByte !=
                rc_Data.c_GreyOutInformation.c_GrayOutValueDataBytes[s32_CounterDataByte])
            {
               // At least one data byte has a changed transparency value
               q_DataByteChangedTransparency = true;
               // Save the new value
               rc_Data.c_GreyOutInformation.c_GrayOutValueDataBytes[s32_CounterDataByte] = s32_TransparencyStepDataByte;
            }
         }

         if (s32_TransparencyStepMsg != rc_Data.c_GreyOutInformation.s32_GrayOutValueMsg)
         {
            // Update the gray out value. The gray out value for data will be adapted too
            const int32_t s32_ColIdTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
            const int32_t s32_ColIdCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
            QVector<int32_t> c_Roles;

            rc_Data.c_GreyOutInformation.s32_GrayOutValueMsg = s32_TransparencyStepMsg;

            // Send the change only if the value changed really and let update the foreground only
            c_Roles.push_back(static_cast<int32_t>(Qt::ForegroundRole));
            this->dataChanged(this->index(s32_RowCounter, s32_ColIdTime), this->index(s32_RowCounter, s32_ColIdCounter),
                              c_Roles);
         }
         else if (q_DataByteChangedTransparency == true)
         {
            // Update the gray out value of CAN message data only.
            const int32_t s32_ColData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
            QVector<int32_t> c_Roles;

            // Send the change only if the value changed really and let update the foreground only
            c_Roles.push_back(static_cast<int32_t>(Qt::ForegroundRole));
            this->dataChanged(this->index(s32_RowCounter, s32_ColData), this->index(s32_RowCounter, s32_ColData),
                              c_Roles);
         }
         else
         {
            // Nothing to do
         }

         ++s32_RowCounter;
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
int32_t C_CamMetTreeModel::mh_GetTransparencyStep(const uint32_t ou32_DiffTime)
{
   int32_t s32_TransparencyStep;

   if (ou32_DiffTime > mu32_TRACE_TRANSPARENCY_TIME_START)
   {
      if (ou32_DiffTime >= mu32_TRACE_TRANSPARENCY_TIME_END)
      {
         // Maximum reached
         s32_TransparencyStep = ms32_TRACE_TRANSPARENCY_STEPS;
      }
      else
      {
         // Interpolation
         const uint32_t u32_DiffPart = ou32_DiffTime - mu32_TRACE_TRANSPARENCY_TIME_START;
         s32_TransparencyStep =
            static_cast<int32_t>((u32_DiffPart * static_cast<uint32_t>(ms32_TRACE_TRANSPARENCY_STEPS)) /
                                 mu32_TRACE_TRANSPARENCY_TIME_DIFF);
      }
   }
   else
   {
      s32_TransparencyStep = 0;
   }

   return s32_TransparencyStep;
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
   for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      const C_OscComMessageLoggerDataSignal & rc_Signal = orc_Message.c_Signals[u32_ItSig];
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
void C_CamMetTreeModel::mh_ResizeIfNecessary(QString & orc_Str, const int32_t os32_MaxLength)
{
   orc_Str.resize(std::min(orc_Str.count(), static_cast<int32_t>(os32_MaxLength)));
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
                                                       const bool oq_SignalInsert, const int32_t os32_MessageRow)
{
   const std::vector<int32_t> c_Order = C_CamMetUtil::h_GetMultiplexerOrder(orc_Message.c_Signals);

   if (c_Order.size() == 0UL)
   {
      //Normal message
      if (opc_Item->c_Children.size() < orc_Message.c_Signals.size())
      {
         if ((oq_SignalInsert) && (os32_MessageRow >= 0))
         {
            this->beginInsertRows(this->index(os32_MessageRow, 0), static_cast<int32_t>(opc_Item->c_Children.size()),
                                  static_cast<int32_t>(opc_Item->c_Children.size() + orc_Message.c_Signals.size()));
         }
         //Add
         for (uint32_t u32_ItSig = opc_Item->c_Children.size(); u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
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
         for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
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
      std::vector<uint32_t> c_Expected;
      std::vector<uint32_t> c_Current;
      //Multiplexer
      for (uint32_t u32_ItOr = 0UL; u32_ItOr < c_Order.size(); ++u32_ItOr)
      {
         uint32_t u32_Counter = 0UL;
         for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            const C_OscComMessageLoggerDataSignal & rc_SignalData = orc_Message.c_Signals[u32_ItSig];
            if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               if (static_cast<int32_t>(rc_SignalData.c_OscSignal.u16_MultiplexValue) == c_Order[u32_ItOr])
               {
                  ++u32_Counter;
               }
            }
         }
         c_Expected.push_back(u32_Counter);
      }
      //Others
      for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
      {
         const C_OscComMessageLoggerDataSignal & rc_SignalData = orc_Message.c_Signals[u32_ItSig];
         if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT)
         {
            c_Expected.push_back(0UL);
         }
      }
      //Current state
      c_Current.reserve(opc_Item->c_Children.size());
      for (uint32_t u32_ItChild = 0UL; u32_ItChild < opc_Item->c_Children.size(); ++u32_ItChild)
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
         uint32_t u32_InsertAt = 0UL;
         uint32_t u32_InsertNum = 0UL;
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
            for (uint32_t u32_ItChild = 0UL; u32_ItChild < u32_InsertNum; ++u32_ItChild)
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
            for (uint32_t u32_ItExpected = 0UL; u32_ItExpected < c_Expected.size(); ++u32_ItExpected)
            {
               C_TblTreSimpleItem * const pc_Parent = new C_TblTreSimpleItem();
               for (uint32_t u32_ItChild = 0UL; u32_ItChild < c_Expected[u32_ItExpected]; ++u32_ItChild)
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
} //lint !e429  no memory leak because of the parent pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares one data entry with the search string

   \param[in]     orc_Data              Message to search
   \param[in]     orc_SearchString      String to search in data for
   \param[in]     oq_Next               Flag for search direction of signals
                                          true:  Search the next entry, forward
                                          false: Search the previous entry, backward
   \param[in,out] ors32_SignalIndex      Input: >= 0: signal index to start search; -1: start with message
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
                                             const bool oq_Next, int32_t & ors32_SignalIndex,
                                             bool & orq_LastSignalChecked) const
{
   bool q_Return = false;

   if (orc_SearchString != "")
   {
      // Check only the string variant, which are visible
      // Strings which has no letters, no upper case necessary
      const int32_t os32_StartSignalRow = ors32_SignalIndex;
      bool q_SkipSignals = false;

      // Reset the signal row for not found as default output
      ors32_SignalIndex = -1;

      // Check message strings when no start signal is defined
      if (os32_StartSignalRow < 0)
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
               uint32_t u32_Counter = 0U;
               bool q_SearchFinished = false;

               if (os32_StartSignalRow > 0)
               {
                  // Set start signal
                  u32_Counter = static_cast<uint32_t>(os32_StartSignalRow);
               }

               do
               {
                  // Check each signal
                  q_Return = this->m_CheckSignalDataForSearch(orc_Data.c_Signals[u32_Counter], orc_SearchString);

                  if (q_Return == true)
                  {
                     // Found a match, search finished
                     ors32_SignalIndex = static_cast<int32_t>(u32_Counter);
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
               if (u32_Counter == (static_cast<uint32_t>(orc_Data.c_Signals.size()) - 1UL))
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
   const C_SclString c_SearchString = orc_SearchString.toUpper().toStdString().c_str();

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
bool C_CamMetTreeModel::m_CheckSignalDataForSearch(const C_OscComMessageLoggerDataSignal & orc_SignalData,
                                                   const QString & orc_SearchString) const
{
   bool q_Return = false;

   // Compare all in upper case to be case insensitive
   const C_SclString c_SearchString = orc_SearchString.toUpper().toStdString().c_str();

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

   If ors32_MultiplexedSignalRow is >= 0, the checked  signal is a multiplexed signal and the signal is on
   the third level of the model. Then os32_SignalRow equals the row of the multiplexer signal.

   If ors32_MultiplexedSignalRow is < 0, the checked signal can be a normal signal or the multiplexer signal and is on
   the second level of the model.

   Condition: The vector of signals must be sorted by function mh_SortMultiplexedSignals or by default

   \param[in]       orc_Signals                All signals to compare and check with
   \param[in]       ou32_SignalIndexToCheck    The signal to check
   \param[out]      ors32_SignalRow              >= 0: If ors32_MultiplexedSignalRow >= 0: Row of Multiplexer signal
                                                       If ors32_MultiplexedSignalRow < 0: Row of checked signal
                                                 < 0: ou32_SignalIndexToCheck not valid
   \param[out]      ors32_MultiplexedSignalRow   >= 0: Row of Multiplexed signal
                                                 < 0: Signal is not a multiplexed signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_GetMultiplexedMsgSignalRow(const std::vector<C_OscComMessageLoggerDataSignal> & orc_Signals,
                                                     const uint32_t ou32_SignalIndexToCheck, int32_t & ors32_SignalRow,
                                                     int32_t & ors32_MultiplexedSignalRow) const
{
   ors32_SignalRow = -1;
   ors32_MultiplexedSignalRow = -1;

   if (ou32_SignalIndexToCheck < orc_Signals.size())
   {
      uint32_t u32_Counter;
      const C_OscCanSignal & rc_OscSginalToCheck = orc_Signals[ou32_SignalIndexToCheck].c_OscSignal;

      // Multiplexer signals come first
      // Multiplexed signals has the matching multiplexer signal as parent
      // Default signals come after the multiplexer signals with its multiplexed signals
      if (rc_OscSginalToCheck.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT)
      {
         ors32_SignalRow = 0;

         for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
         {
            const C_OscCanSignal & rc_OscSginal = orc_Signals[u32_Counter].c_OscSignal;

            // Count all multiplexer signals which are always at the beginning and all default signals
            // which have a lower index
            // Multiplexed signals are not on the same level
            if ((rc_OscSginal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL) ||
                ((rc_OscSginal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT) &&
                 (u32_Counter < ou32_SignalIndexToCheck)))
            {
               ++ors32_SignalRow;
            }
         }
      }
      else if (rc_OscSginalToCheck.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
      {
         ors32_MultiplexedSignalRow = 0;

         // The signal is on third level of the tree below the multiplexer signal
         for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
         {
            const C_OscComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_Counter];
            const C_OscCanSignal & rc_OscSginal = rc_Signal.c_OscSignal;

            if ((u32_Counter < ou32_SignalIndexToCheck) &&
                (rc_OscSginal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                (rc_OscSginal.u16_MultiplexValue == rc_OscSginalToCheck.u16_MultiplexValue))
            {
               // Search the multiplexed signals with the same multiplexer value with a lower index to get the row of
               // the checked signal below the multiplexer signal
               ++ors32_MultiplexedSignalRow;
            }
            else if (rc_OscSginal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
            {
               const int32_t s32_Value = rc_Signal.c_RawValueDec.ToIntDef(-1);

               if (s32_Value >= 0)
               {
                  const uint16_t u16_Value = static_cast<uint16_t>(s32_Value);
                  if (u16_Value <= rc_OscSginalToCheck.u16_MultiplexValue)
                  {
                     // In this case, this is the row of the multiplexer signal
                     // Starting with -1 all multiplexer signals whit a concrete value lower or equal to the
                     // expected value count for the row
                     ++ors32_SignalRow;
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
         const int32_t s32_CheckValue = orc_Signals[ou32_SignalIndexToCheck].c_RawValueDec.ToIntDef(-1);

         if (s32_CheckValue >= 0)
         {
            for (u32_Counter = 0U; u32_Counter < orc_Signals.size(); ++u32_Counter)
            {
               const C_OscComMessageLoggerDataSignal & rc_Signal = orc_Signals[u32_Counter];
               const C_OscCanSignal & rc_OscSginal = rc_Signal.c_OscSignal;
               const int32_t s32_Value = rc_Signal.c_RawValueDec.ToIntDef(-1);

               if (s32_Value >= 0)
               {
                  // Starting with -1 all multiplexer signals whit a concrete value lower or equal to the
                  // expected value count for the row
                  if ((rc_OscSginal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
                      (s32_Value <= s32_CheckValue))
                  {
                     ++ors32_SignalRow;
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
   this->ms32_LastSearchedMessageRow = -1;
   this->ms32_LastSearchedSignalRow = -1;
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
   const uint32_t ou32_PrevMsgTimeStamp, const uint32_t ou32_NewMsgTimeStamp, const int32_t os32_MultiplexerValue,
   bool & orq_UpdateDataTimeStamp)
{
   C_CamMetTreeLoggerDataGreyOutInformation c_Retval;

   orq_UpdateDataTimeStamp = false;

   c_Retval.s32_GrayOutValueMsg = orc_MessagePrev.c_GreyOutInformation.s32_GrayOutValueMsg;
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
      for (uint8_t u8_DbCounter = orc_MessagePrev.c_CanMsg.u8_DLC; u8_DbCounter < orc_MessageNew.c_CanMsg.u8_DLC;
           ++u8_DbCounter)
      {
         // Set the current timestamp for the changed byte
         c_Retval.c_DataBytesChangedTimeStamps[u8_DbCounter] = ou32_PrevMsgTimeStamp;
      }
   }

   //reset the gap between old and new dlc
   for (uint8_t u8_DbCounter = 0; u8_DbCounter < orc_MessagePrev.c_CanMsg.u8_DLC; ++u8_DbCounter)
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
                                                           const uint32_t ou32_PrevMsgTimeStamp)
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
   for (uint8_t u8_DbCounter = 0; u8_DbCounter < 8U; ++u8_DbCounter)
   {
      orc_Message.c_GreyOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter] =
         orc_StoredGrayOutInformation.c_DataBytesChangedTimeStamps[u8_DbCounter];
   }

   // Restore previous values after the assignment to avoid further temporary copies of the instance
   orc_Message.c_GreyOutInformation.s32_GrayOutValueMsg = orc_StoredGrayOutInformation.s32_GrayOutValueMsg;
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
bool C_CamMetTreeModel::mh_CheckForFixByInsertingNewChild(const std::vector<uint32_t> & orc_ExpectedVec,
                                                          const std::vector<uint32_t> & orc_CurrentVec,
                                                          uint32_t & oru32_InsertAt, uint32_t & oru32_InsertNum)
{
   bool q_FixByInsertingNewChild = false;

   oru32_InsertAt = 0UL;
   oru32_InsertNum = 0UL;
   if (orc_ExpectedVec.size() == (orc_CurrentVec.size() + 1UL))
   {
      uint32_t u32_Counter = 0UL;
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
         oru32_InsertNum = orc_ExpectedVec[static_cast<std::vector<uint32_t>::size_type>(orc_ExpectedVec.size() - 1UL)];
      }
      for (uint32_t u32_ItCurrent = 0; (u32_ItCurrent < orc_CurrentVec.size()) && q_FixByInsertingNewChild;
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
                                                                const int32_t os32_MuxValue)

{
   std::vector<C_OscComMessageLoggerDataSignal> c_KeptSignals;

   //Only if mux value valid there is a chance that some signals should be kept
   if (os32_MuxValue >= 0)
   {
      //Look for signals which might not be present in the new message
      for (std::vector<C_OscComMessageLoggerDataSignal>::iterator c_ItSig = orc_PreviousMessage.c_Signals.begin();
           c_ItSig != orc_PreviousMessage.c_Signals.end(); ++c_ItSig)
      {
         const C_OscComMessageLoggerDataSignal & rc_Sig = *c_ItSig;
         if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT)
         {
            //Guaranteed to be replaced
         }
         else
         {
            if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               //Multiplexed
               if (rc_Sig.c_OscSignal.u16_MultiplexValue == static_cast<uint16_t>(os32_MuxValue))
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
               // In case of a DLC error the signal will be replaced in any case and avoid getting the invalid value
               // as integer
               if ((rc_Sig.q_DlcError == true) ||
                   (rc_Sig.c_RawValueDec.ToInt() == static_cast<uint16_t>(os32_MuxValue)))
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
   for (std::vector<C_OscComMessageLoggerDataSignal>::const_iterator c_ItSig = c_KeptSignals.begin();
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
uint32_t C_CamMetTreeModel::mh_TranslateTreeRowsToSignalIndex(
   const std::vector<C_OscComMessageLoggerDataSignal> & orc_Signals, const int32_t os32_SignalIndex,
   const int32_t os32_SignalIndexL2)
{
   uint32_t u32_Retval = 0UL;
   const std::vector<int32_t> c_Order = C_CamMetUtil::h_GetMultiplexerOrder(orc_Signals);
   uint32_t u32_Counter = 0UL;

   if (static_cast<uint32_t>(os32_SignalIndex) < c_Order.size())
   {
      //Part of multiplexer
      for (uint32_t u32_ItSigL1 = 0UL; u32_ItSigL1 < orc_Signals.size(); ++u32_ItSigL1)
      {
         const C_OscComMessageLoggerDataSignal & rc_SignalData = orc_Signals[u32_ItSigL1];
         if (os32_SignalIndexL2 >= 0)
         {
            if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               //Multiplexed
               if (static_cast<int32_t>(rc_SignalData.c_OscSignal.u16_MultiplexValue) ==
                   c_Order[static_cast<uint32_t>(os32_SignalIndex)])
               {
                  if (u32_Counter == static_cast<uint32_t>(os32_SignalIndexL2))
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
            if ((rc_SignalData.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
                (rc_SignalData.q_DlcError == false) &&
                (rc_SignalData.c_RawValueDec.ToInt() == c_Order[static_cast<uint32_t>(os32_SignalIndex)]))
            {
               //Just use first multiplexer
               u32_Retval = u32_ItSigL1;
               //lint -e{9011} break is more efficient
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
      for (uint32_t u32_ItSig = 0UL; u32_ItSig < orc_Signals.size(); ++u32_ItSig)
      {
         const C_OscComMessageLoggerDataSignal & rc_SignalData = orc_Signals[u32_ItSig];
         if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT)
         {
            if (u32_Counter == static_cast<uint32_t>(os32_SignalIndex))
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
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < orc_Data.c_Signals.size(); ++u32_Counter)
   {
      if (orc_Data.c_Signals[u32_Counter].c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
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
void C_CamMetTreeModel::mh_SortMultiplexedSignals(std::vector<C_OscComMessageLoggerDataSignal> & orc_Signals)
{
   uint32_t u32_MultiplexerCounter = 0U;
   uint32_t u32_SignalCounter;

   //lint -e{864} Call as expected by interface
   std::sort(orc_Signals.begin(), orc_Signals.end(), C_LoggerDataComparatorForMultiplexer());

   // Search all multiplexed signal for each the multiplexer. Iterate over the multiplexer signals
   while (u32_MultiplexerCounter < orc_Signals.size())
   {
      const C_OscComMessageLoggerDataSignal & rc_MultiplexerSig = orc_Signals[u32_MultiplexerCounter];
      if (rc_MultiplexerSig.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         const int32_t s32_MultiplexerValue = rc_MultiplexerSig.c_RawValueDec.ToIntDef(-1);

         if (s32_MultiplexerValue >= 0)
         {
            // Compare the multiplexer value of this multiplexer signal with all multiplexed signals
            for (u32_SignalCounter = u32_MultiplexerCounter + 1U; u32_SignalCounter < orc_Signals.size();
                 ++u32_SignalCounter)
            {
               if ((orc_Signals[u32_SignalCounter].c_OscSignal.e_MultiplexerType ==
                    C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                   (orc_Signals[u32_SignalCounter].c_OscSignal.u16_MultiplexValue ==
                    static_cast<uint16_t>(s32_MultiplexerValue)))
               {
                  const C_OscComMessageLoggerDataSignal c_SigCopy = orc_Signals[u32_SignalCounter];

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
/*! \brief  Checks if status of the message is valid

   Checks status for an ECeS message

   \param[in]       opc_CurrentMessage     Detailed input parameter description

   \return
   true if valid
   false otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::m_IsStatusValid(const C_CamMetTreeLoggerData & orc_CurrentMessage) const
{
   bool q_StatusValid = true;

   // If status is "invalid" (Invalid counter or CRC for ECeS message)
   const QString c_Status(orc_CurrentMessage.c_Status.AsStdString()->c_str());

   if ((c_Status.contains("invalid", Qt::CaseInsensitive)) || (c_Status.contains("incorrect", Qt::CaseInsensitive)))
   {
      q_StatusValid = false;
   }

   return q_StatusValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Custom operator for sorting C_OscComMessageLoggerDataSignal

   All multiplexer signals will be sorted to the beginning (less than) in order of the set multiplexer value

   \param[in]       orc_Data1     Detailed input parameter description
   \param[in]       orc_Data2     Detailed input parameter description

   \retval   true    orc_Data1 is less than orc_Data2
   \retval   false   orc_Data2 is less or equal than orc_Data1
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetTreeModel::C_LoggerDataComparatorForMultiplexer::operator ()(
   const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Data1,
   const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Data2) const
{
   bool q_Return;

   if ((orc_Data1.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
       (orc_Data2.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL))
   {
      // Both are multiplexer signals, compare the values
      const int32_t s32_Value1 = orc_Data1.c_RawValueDec.ToIntDef(-1);
      const int32_t s32_Value2 = orc_Data2.c_RawValueDec.ToIntDef(-1);

      q_Return = s32_Value1 < s32_Value2;
   }
   else if (orc_Data1.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
   {
      // orc_Data2 is not a multiplexer signal
      q_Return = true;
   }
   else if (orc_Data2.c_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
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
