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

#include "stwtypes.h"
#include "CCMONProtocol.h"
#include "C_GtGetText.h"
#include "C_CamMetTreeModel.h"
#include "TGLTime.h"

#include "cam_constants.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::uint32 C_CamMetTreeModel::mhu32_MAX_STORAGE = 1000;
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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeModel::C_CamMetTreeModel(QObject * const opc_Parent) :
   QAbstractItemModel(opc_Parent),
   mq_DisplayTree(false),
   mq_UniqueMessageMode(false),
   mq_DisplayAsHex(false),
   mq_DisplayTimestampRelative(false),
   mq_DataUnlocked(false),
   mu32_OldestItemIndex(0),
   ms32_SelectedParentRow(-1),
   ms32_SelectedChildRow(-1),
   mq_GrayOutPause(false),
   mu32_GrayOutPauseTimeStamp(0U)
{
   sintn sn_Counter;

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mc_DataBase.reserve(C_CamMetTreeModel::mhu32_MAX_STORAGE);

   // Prepare the transparency colors for the gray out font
   this->mc_FontTransparcencyColors.resize((msn_TRACE_TRANSPARENCY_START - msn_TRACE_TRANSPARENCY_END) + 1,
                                           mc_STYLE_GUIDE_COLOR_1);

   // Value on position 0 equals no transparency and at the end equals maximum transparency
   for (sn_Counter = 0U; sn_Counter < static_cast<sintn>(this->mc_FontTransparcencyColors.size()); ++sn_Counter)
   {
      this->mc_FontTransparcencyColors[sn_Counter].setAlpha(msn_TRACE_TRANSPARENCY_START - sn_Counter);
   }

   connect(&this->mc_GrayOutTimer, &QTimer::timeout, this, &C_CamMetTreeModel::m_GrayOutTimer);
   this->mc_GrayOutTimer.start(msn_TRACE_TRANSPARENCY_REFRESH_TIME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Continues the gray out engine

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
      rc_Data.u32_DataChangedTimeStamp += u32_GrayOutPauseOffset;

      // Timestamp of each data byte
      for (sn_CounterDataByte = 0; sn_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++sn_CounterDataByte)
      {
         rc_Data.c_DataBytesChangedTimeStamps[sn_CounterDataByte] += u32_GrayOutPauseOffset;
      }
   }

   this->mq_GrayOutPause = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Pauses the gray out engine
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
/*! \brief   Pauses the gray out engine

   Calculation of the offset is not necessary. Stop can only be followed by start
   and start resets the offset in any case.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Stop(void)
{
   this->mq_GrayOutPause = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Continues the gray out engine and resets the pause offset

   The offset is not necessary on a restart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::Start(void)
{
   this->mq_GrayOutPause = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to lock up all data requests (performance)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::LockData(void)
{
   this->mq_DataUnlocked = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to unlock up all data requests (performance)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::UnlockData(void)
{
   this->mq_DataUnlocked = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle action: clear data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::ActionClearData(void)
{
   this->beginResetModel();
   this->mu32_OldestItemIndex = 0UL;
   this->mc_DataBase.clear();
   this->mc_UniqueMessages.clear();
   //Every reset will clear the selection
   this->SetSelection(-1, -1);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add specified strings as rows

   \param[in] orc_Data New row content

   \return
   Indices of added rows
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_CamMetTreeModel::AddRows(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   std::vector<sint32> c_Retval;
   if (orc_Data.size() > 0)
   {
      //Columns which need to get updated on change of static message
      const sint32 s32_ColIDName = C_CamMetTreeModel::h_EnumToColumn(eCAN_NAME);
      const sint32 s32_ColIDDLC = C_CamMetTreeModel::h_EnumToColumn(eCAN_DLC);
      const sint32 s32_ColIDData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
      const sint32 s32_ColIDTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      const sint32 s32_ColIDCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
      const uint32 u32_CompleteSize = static_cast<uint32>(this->mc_DataBase.size()) +
                                      static_cast<uint32>(orc_Data.size());

      //Check if only append case
      if (u32_CompleteSize <= C_CamMetTreeModel::mhu32_MAX_STORAGE)
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
            this->mc_DataBase.push_back(*c_ItData);
         }
         //Appending so notify model of insert action
         if (this->mq_UniqueMessageMode == false)
         {
            this->endInsertRows();
         }
      }
      //Check if we need to append any items to reach the max storage size
      else if (this->mc_DataBase.size() < C_CamMetTreeModel::mhu32_MAX_STORAGE)
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
            //1. insert until space is no longer available
            if (this->mc_DataBase.size() < C_CamMetTreeModel::mhu32_MAX_STORAGE)
            {
               //Append in reserved space
               this->mc_DataBase.push_back(*c_ItData);
            }
            //2. start replacing oldest ones
            else
            {
               //Shift item
               this->mc_DataBase[this->mu32_OldestItemIndex] = *c_ItData;
               ++this->mu32_OldestItemIndex;
               this->mu32_OldestItemIndex %= C_CamMetTreeModel::mhu32_MAX_STORAGE;
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
            this->mc_DataBase[this->mu32_OldestItemIndex] = *c_ItData;
            ++this->mu32_OldestItemIndex;
            this->mu32_OldestItemIndex %= C_CamMetTreeModel::mhu32_MAX_STORAGE;
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
      //Every time
      for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
           c_ItData != orc_Data.end(); ++c_ItData)
      {
         uint8 u8_DbCounter;

         //Check if there is a new row
         if (this->mc_UniqueMessages.contains(c_ItData->c_CanIdDec) == true)
         {
            bool q_UpdateDataTimeStamp = false;
            uint32 u32_PreviousDataTimeStamp = 0U;
            C_CamMetTreeLoggerData & rc_Message = this->mc_UniqueMessages[c_ItData->c_CanIdDec];
            const float64 f64_PreviousAbsoluteTimestampValue = rc_Message.c_TimeStampAbsolute.ToDouble();
            const float64 f64_NewAbsoluteTimestampValue = c_ItData->c_TimeStampAbsolute.ToDouble();
            float64 f64_RelativeTimestamp;
            stw_types::sintn sn_PreviousGrayOutValueMsg;
            stw_types::sintn sn_PreviousGrayOutValueData;
            std::vector<stw_types::sintn> c_PreviousGrayOutValueDataBytes;
            std::vector<uint32> c_DataBytesTimeStampCopy;
            const uint32 u32_MsgTimeStamp = static_cast<uint32>(rc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);

            // Save previous values which are not filled by C_SyvComMessageMonitor
            sn_PreviousGrayOutValueMsg = rc_Message.sn_GrayOutValueMsg;
            sn_PreviousGrayOutValueData = rc_Message.sn_GrayOutValueData;
            c_PreviousGrayOutValueDataBytes = rc_Message.c_GrayOutValueDataBytes;

            // Check if the CAN message data changed
            if (rc_Message.c_CanDataHex != c_ItData->c_CanDataHex)
            {
               // Old message with changed data. Update the timestamp of the CAN message data
               q_UpdateDataTimeStamp = true;
            }
            else
            {
               // Using the previous timestamp again. It may not be overwritten by the new message with an
               // unset timestamp
               u32_PreviousDataTimeStamp = rc_Message.u32_DataChangedTimeStamp;
            }

            // Check if the CAN message data bytes changed
            c_DataBytesTimeStampCopy.resize(8);

            //if old DLC < than new DLC
            if (rc_Message.c_CanMsg.u8_DLC < c_ItData->c_CanMsg.u8_DLC)
            {
               //reset the gap between old and new dlc
               for (u8_DbCounter = rc_Message.c_CanMsg.u8_DLC; u8_DbCounter < c_ItData->c_CanMsg.u8_DLC; ++u8_DbCounter)
               {
                  // Set the current timestamp for the changed byte
                  c_DataBytesTimeStampCopy[u8_DbCounter] = u32_MsgTimeStamp;
               }
            }

            //reset the gap between old and new dlc
            for (u8_DbCounter = 0; u8_DbCounter < rc_Message.c_CanMsg.u8_DLC; ++u8_DbCounter)
            {
               //replace timestamp when databyte has changed
               if (rc_Message.c_CanMsg.au8_Data[u8_DbCounter] != c_ItData->c_CanMsg.au8_Data[u8_DbCounter])
               {
                  // Set the current timestamp for the changed byte
                  c_DataBytesTimeStampCopy[u8_DbCounter] = u32_MsgTimeStamp;
               }
               else
               {
                  // Use the previous timestamp
                  c_DataBytesTimeStampCopy[u8_DbCounter] = rc_Message.c_DataBytesChangedTimeStamps[u8_DbCounter];
               }
            }

            //No new row: just replace the last known "newest" message at this ID
            rc_Message = *c_ItData;

            if (q_UpdateDataTimeStamp == true)
            {
               // Update the timestamp. Update it after the assignment to avoid further temporary copies of the instance
               rc_Message.u32_DataChangedTimeStamp = u32_MsgTimeStamp;
            }
            else
            {
               // Restore the previous timestamp
               rc_Message.u32_DataChangedTimeStamp = u32_PreviousDataTimeStamp;
            }

            // Reassign the data byte timestamps
            for (u8_DbCounter = 0; u8_DbCounter < 8U; ++u8_DbCounter)
            {
               rc_Message.c_DataBytesChangedTimeStamps[u8_DbCounter] = c_DataBytesTimeStampCopy[u8_DbCounter];
            }

            // Restore previous values after the assignment to avoid further temporary copies of the instance
            rc_Message.sn_GrayOutValueMsg = sn_PreviousGrayOutValueMsg;
            rc_Message.sn_GrayOutValueData = sn_PreviousGrayOutValueData;
            rc_Message.c_GrayOutValueDataBytes = c_PreviousGrayOutValueDataBytes;

            //Handle relative timestamp value
            f64_RelativeTimestamp = (f64_NewAbsoluteTimestampValue - f64_PreviousAbsoluteTimestampValue) * 1000.0;
            if (f64_RelativeTimestamp < 0.0)
            {
               f64_RelativeTimestamp = 0.0;
            }
            rc_Message.c_TimeStampRelative =
               stw_cmon_protocol::C_CMONProtocols::FormatTimeStamp(static_cast<uint64>(f64_RelativeTimestamp),
                                                                   true);
         }
         else
         {
            bool q_NewRow = false;
            QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_NewPos;

            if (this->mq_UniqueMessageMode == true)
            {
               //Handle begin!
               if (this->mc_UniqueMessages.contains(c_ItData->c_CanIdDec) == false)
               {
                  sintn sn_EstimatedPosIndex = 0;
                  //Get first valid item after the position it should be at
                  QMap<C_SCLString,
                       C_CamMetTreeLoggerData>::iterator c_EstimatedPos = this->mc_UniqueMessages.upperBound(
                     c_ItData->c_CanIdDec);
                  //Calc estimated new position index
                  for (QMap<C_SCLString, C_CamMetTreeLoggerData>::iterator c_ItUniqueMessages =
                          this->mc_UniqueMessages.begin();
                       (c_ItUniqueMessages != this->mc_UniqueMessages.end()) && (c_ItUniqueMessages != c_EstimatedPos);
                       ++c_ItUniqueMessages)
                  {
                     ++sn_EstimatedPosIndex;
                  }

                  //handle new row with known position index
                  q_NewRow = true;
                  c_Retval.push_back(sn_EstimatedPosIndex);

                  //Notify the new row count
                  this->beginInsertRows(QModelIndex(), sn_EstimatedPosIndex, sn_EstimatedPosIndex);
               }
            }

            //Insert new item
            c_NewPos = this->mc_UniqueMessages.insert(c_ItData->c_CanIdDec, *c_ItData);
            // New message, new data. Update the timestamp of the CAN message data and its bytes
            c_NewPos->u32_DataChangedTimeStamp = static_cast<uint32>(c_NewPos->c_CanMsg.u64_TimeStamp / 1000ULL);
            for (u8_DbCounter = 0; u8_DbCounter < c_NewPos->c_CanMsg.u8_DLC; ++u8_DbCounter)
            {
               c_NewPos->c_DataBytesChangedTimeStamps[u8_DbCounter] = c_NewPos->u32_DataChangedTimeStamp;
            }

            //Check if begin was called so we properly close the insert operation
            if (q_NewRow)
            {
               this->endInsertRows();
            }
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
      //Notify the model of data changes for all replaced items (current: all items)
      if (this->mq_UniqueMessageMode == true)
      {
         this->dataChanged(this->index(0, s32_ColIDTime), this->index(this->rowCount() - 1, s32_ColIDTime));
         this->dataChanged(this->index(0, s32_ColIDCounter), this->index(this->rowCount() - 1, s32_ColIDCounter));
         //Possible changes
         this->dataChanged(this->index(0, s32_ColIDName), this->index(this->rowCount() - 1, s32_ColIDName));
         this->dataChanged(this->index(0, s32_ColIDDLC), this->index(this->rowCount() - 1, s32_ColIDDLC));
         this->dataChanged(this->index(0, s32_ColIDData), this->index(this->rowCount() - 1, s32_ColIDData));
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current selection

   \param[in] os32_SelectedParentRow Current selected parent index
   \param[in] os32_SelectedChildRow  Current selected child index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetSelection(const sint32 os32_SelectedParentRow, const sint32 os32_SelectedChildRow)
{
   const sint32 s32_ColIDData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   this->ms32_SelectedParentRow = os32_SelectedParentRow;
   this->ms32_SelectedChildRow = os32_SelectedChildRow;
   //Update data row
   this->dataChanged(this->index(0, s32_ColIDData), this->index(this->rowCount() - 1, s32_ColIDData));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set display mode: display tree

   \param[in] oq_Value New value
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
/*! \brief   Set display mode: display unique messages

   \param[in] oq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayUniqueMessages(const bool oq_Value)
{
   this->beginResetModel();
   this->mq_UniqueMessageMode = oq_Value;
   //Every reset will clear the selection
   this->SetSelection(-1, -1);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set display style for CAN ID and CAN data

   \param[in] oq_Value New value
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
/*! \brief   Set display style for timestamp

   \param[in] oq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SetDisplayTimestampRelative(const bool oq_Value)
{
   const sint32 s32_ColID = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);

   //Update value
   this->mq_DisplayTimestampRelative = oq_Value;
   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColID), this->index(this->rowCount() - 1, s32_ColID));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns display style for CAN ID and CAN data

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
/*! \brief   Returns display style for timestamp

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
/*! \brief   Get all messages

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
/*! \brief   Signal a change of protocol related data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::SignalProtocolChange(void)
{
   const sint32 s32_ColID = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);

   //Update UI
   Q_EMIT this->dataChanged(this->index(0, s32_ColID), this->index(this->rowCount() - 1, s32_ColID));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

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
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING))
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
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eTIME_STAMP:
            c_Retval = C_GtGetText::h_GetText(
               "Absolute time (ms.us) from the measurement start, or relative to the previous event.");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN identifier of the message.");
            break;
         case eCAN_NAME:
            c_Retval = C_GtGetText::h_GetText(
               "Symbolic name of the CAN message. Empty if not defined in on of the active database(s).");
            break;
         case eCAN_DIR:
            c_Retval = C_GtGetText::h_GetText(
               "Direction of the CAN message.\n"
               "   - Rx: Received message\n"
               "   - Tx: Sent by this instance of openSYDE CAN Monitor");
            break;
         case eCAN_DLC:
            c_Retval = C_GtGetText::h_GetText("Data length code. Number of data bytes in decimal representation.");
            break;
         case eCAN_DATA:
            c_Retval = C_GtGetText::h_GetText("CAN message data in bytes.");
            break;
         case eCAN_COUNTER:
            c_Retval = C_GtGetText::h_GetText(
               "Indicates the number of times the event has appeared since measurement start.");
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
/*! \brief   Get data index

   \param[in] osn_Row    Row
   \param[in] osn_Column Column
   \param[in] orc_Parent Parent

   \return
   Data index (may be invalid = invalid parameters)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMetTreeModel::index(const sintn osn_Row, const sintn osn_Column, const QModelIndex & orc_Parent) const
{
   QModelIndex c_Retval;

   if (hasIndex(osn_Row, osn_Column, orc_Parent) == true)
   {
      if (orc_Parent.isValid() == true)
      {
         const uintn un_Id = static_cast<uintn>(orc_Parent.row() + 2);
         c_Retval = this->createIndex(osn_Row, osn_Column, un_Id);
      }
      else
      {
         //Top level
         c_Retval = this->createIndex(osn_Row, osn_Column, 1);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parent index

   \param[in] orc_Index Index

   \return
   Parent index (may be invalid = root level)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMetTreeModel::parent(const QModelIndex & orc_Index) const
{
   QModelIndex c_Retval;

   if (orc_Index.internalId() > 1)
   {
      const uintn un_Row = orc_Index.internalId() - 2U;
      //Always use column 0 to always have the same parent index (necessary for selection)
      c_Retval = this->createIndex(un_Row, 0);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tree column count

   \param[in] orc_Parent Parent

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
   else
   {
      // Nothing to do
   }

   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tree row count

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMetTreeModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (orc_Parent.isValid() == false)
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
   else if ((orc_Parent.parent().isValid() == false) && (this->mq_DisplayTree == true))
   {
      const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Parent.row());
      if (pc_CurMessage != NULL)
      {
         sn_Retval = pc_CurMessage->c_Signals.size();
      }
   }
   else
   {
      // Nothing to do
   }

   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

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
                  else
                  {
                     c_Retval = pc_CurMessage->c_TimeStampAbsolute.c_str();
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
            const C_CamMetTreeLoggerData * const pc_CurMessage = GetMessageData(orc_Index.parent().row());

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
                  const C_OSCComMessageLoggerDataSignal & rc_CurSignal = pc_CurMessage->c_Signals[orc_Index.row()];
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
                  c_Value = rc_CurSignal.c_Comment.c_str();
                  c_List.push_back(c_Value);
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
               if ((((this->ms32_SelectedParentRow >= 0) && (this->ms32_SelectedChildRow >= 0)) &&
                    (orc_Index.parent().isValid() == false)) && (orc_Index.row() == this->ms32_SelectedParentRow))
               {
                  //Check which child/signal should be selected
                  const uint32 u32_ChildIndex = static_cast<uint32>(this->ms32_SelectedChildRow);

                  //Check if the signal is valid
                  if (u32_ChildIndex < pc_CurMessage->c_Signals.size())
                  {
                     std::set<uint16> c_Bits;
                     std::set<uint16> c_Bytes;
                     const C_OSCComMessageLoggerDataSignal & rc_CurSignal = pc_CurMessage->c_Signals[u32_ChildIndex];
                     //Get all bit positions as intel
                     rc_CurSignal.c_OscSignal.GetDataBytesBitPositionsOfSignal(c_Bits);
                     //Convert bit positions into byte indices
                     for (std::set<uint16>::const_iterator c_ItBit = c_Bits.begin(); c_ItBit != c_Bits.end();
                          ++c_ItBit)
                     {
                        c_Bytes.insert(*c_ItBit / 8U);
                     }
                     //Convert byte indices into bit array
                     for (std::set<uint16>::const_iterator c_ItByte = c_Bytes.begin(); c_ItByte != c_Bytes.end();
                          ++c_ItByte)
                     {
                        if (*c_ItByte < c_Array.size())
                        {
                           //Set all affected bytes to be part of the highlighted area
                           c_Array.setBit(*c_ItByte, true);
                        }
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
               c_Array.reserve(pc_CurMessage->c_GrayOutValueDataBytes.size());
               //Copy over
               for (std::vector<sintn>::const_iterator c_ItVal = pc_CurMessage->c_GrayOutValueDataBytes.begin();
                    c_ItVal != pc_CurMessage->c_GrayOutValueDataBytes.end(); ++c_ItVal)
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
                     if (pc_CurMessage->sn_GrayOutValueMsg <
                         static_cast<sintn>(this->mc_FontTransparcencyColors.size()))
                     {
                        c_Retval = this->mc_FontTransparcencyColors[pc_CurMessage->sn_GrayOutValueMsg];
                     }
                  }
               }
               else if (e_Col == eCAN_DATA)
               {
                  if (pc_CurMessage->sn_GrayOutValueData < static_cast<sintn>(this->mc_FontTransparcencyColors.size()))
                  {
                     c_Retval = this->mc_FontTransparcencyColors[pc_CurMessage->sn_GrayOutValueData];
                  }
               }
               else
               {
                  if (pc_CurMessage->sn_GrayOutValueMsg < static_cast<sintn>(this->mc_FontTransparcencyColors.size()))
                  {
                     c_Retval = this->mc_FontTransparcencyColors[pc_CurMessage->sn_GrayOutValueMsg];
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
/*! \brief   Column to enum conversion

   \param[in]  os32_Column Column

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
/*! \brief   Enum to column conversion

   \param[in] oe_Value Enum value

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
/*! \brief   Get message at row

   TODO: invalid index handling

   \param[in] osn_Row input parameter description

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
                               C_CamMetTreeModel::mhu32_MAX_STORAGE;
      pc_Retval = &this->mc_DataBase[u32_Index];
   }
   else
   {
      //If unique messages: look in map
      const QMap<C_SCLString, C_CamMetTreeLoggerData>::const_iterator c_It =
         this->mc_UniqueMessages.begin() + osn_Row;
      pc_Retval = &c_It.value();
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Timer function for handling gray out engine
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
         const uint32 u32_DiffData = u32_CurrentTime - rc_Data.u32_DataChangedTimeStamp;
         sintn sn_TransparencyStepMsg;
         sintn sn_TransparencyStepData;
         sintn sn_CounterDataByte;
         bool q_DataByteChangedTransparency = false;

         // Check message gray out adaption
         sn_TransparencyStepMsg = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffMsg);

         // Check data gray out adaption
         sn_TransparencyStepData = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffData);

         // Check data byte gray out adaption
         for (sn_CounterDataByte = 0; sn_CounterDataByte < rc_Data.c_CanMsg.u8_DLC; ++sn_CounterDataByte)
         {
            const uint32 u32_DiffDataByte = u32_CurrentTime - rc_Data.c_DataBytesChangedTimeStamps[sn_CounterDataByte];
            sintn sn_TransparencyStepDataByte;

            sn_TransparencyStepDataByte = C_CamMetTreeModel::mh_GetTransparencyStep(u32_DiffDataByte);

            if (sn_TransparencyStepDataByte != rc_Data.c_GrayOutValueDataBytes[sn_CounterDataByte])
            {
               // At least one data byte has a changed transparency value
               q_DataByteChangedTransparency = true;
               // Save the new value
               rc_Data.c_GrayOutValueDataBytes[sn_CounterDataByte] = sn_TransparencyStepDataByte;
            }
         }

         if (sn_TransparencyStepMsg != rc_Data.sn_GrayOutValueMsg)
         {
            // Update the gray out value. The gray out value for data will be adapted too
            const sint32 s32_ColIDTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
            const sint32 s32_ColIDCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
            QVector<sintn> c_Roles;

            rc_Data.sn_GrayOutValueMsg = sn_TransparencyStepMsg;
            rc_Data.sn_GrayOutValueData = sn_TransparencyStepData;

            // Send the change only if the value changed really and let update the foreground only
            c_Roles.push_back(static_cast<sintn>(Qt::ForegroundRole));
            this->dataChanged(this->index(sn_RowCounter, s32_ColIDTime), this->index(sn_RowCounter, s32_ColIDCounter),
                              c_Roles);
         }
         else if ((sn_TransparencyStepData != rc_Data.sn_GrayOutValueData) ||
                  (q_DataByteChangedTransparency == true))
         {
            // Update the gray out value of CAN message data only.
            const sint32 s32_ColData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
            QVector<sintn> c_Roles;

            rc_Data.sn_GrayOutValueData = sn_TransparencyStepData;

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
/*! \brief   Detects the necessary transparency step dependent of the difference time

   \param[in]     ou32_DiffTime  Difference time

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
/*! \brief   Get list of strings

   \param[in] orc_Message Message to evaluate

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
/*! \brief   Resize string to the specified maximum length if the string exceeds this limit

   \param[in,out] orc_Str        String that will get resized if necessary
   \param[in]     os32_MaxLength Maximum length the string may not exceed after calling this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_ResizeIfNecessary(QString & orc_Str, const sint32 os32_MaxLength)
{
   orc_Str.resize(std::min(orc_Str.count(), static_cast<sintn>(os32_MaxLength)));
}
