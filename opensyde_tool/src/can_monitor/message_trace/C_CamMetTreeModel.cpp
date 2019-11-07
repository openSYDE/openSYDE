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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeModel::C_CamMetTreeModel(QObject * const opc_Parent) :
   C_TblTreSimpleModel(opc_Parent),
   mq_DisplayTree(false),
   mq_UniqueMessageMode(false),
   mq_DisplayAsHex(false),
   mq_DisplayTimestampRelative(false),
   mq_DataUnlocked(false),
   mu32_OldestItemIndex(0),
   ms32_SelectedParentRow(-1),
   mq_GrayOutPause(false),
   mu32_GrayOutPauseTimeStamp(0U),
   mpc_RootItemContinuous(new C_TblTreSimpleItem()),
   mpc_RootItemStatic(new C_TblTreSimpleItem())
{
   sintn sn_Counter;

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mc_DataBase.reserve(C_CamMetTreeModel::mhu32_MAX_STORAGE);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_RootItemContinuous->ReserveChildrenSpace(C_CamMetTreeModel::mhu32_MAX_STORAGE);

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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified strings as rows

   \param[in] orc_Data New row content

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

   \param[in] os32_SelectedParentRow  Current selected parent index
   \param[in] os32_SelectedChildIndex Current selected child index
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
/*! \brief  Set display mode: display unique messages

   \param[in] oq_Value New value
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set display style for CAN ID and CAN data

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
/*! \brief  Set display style for timestamp

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
               "Absolute time (hh:mm:ss.ms.us) from the measurement start, or relative to the previous event.");
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
/*! \brief  Get data index

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

   \param[in] orc_Index Index

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

   \param[in] orc_Parent Parent

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

   \param[in] os32_Column Column

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
/*! \brief  Get message at row

   \param[in] osn_Row Current row

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
      const QMap<C_SCLString, C_SCLString>::const_iterator c_ItOrder =
         this->mc_UniqueMessagesOrdering.begin() + osn_Row;
      const QMap<C_SCLString, C_CamMetTreeLoggerData>::const_iterator c_It =
         this->mc_UniqueMessages.find(c_ItOrder.value());
      pc_Retval = &c_It.value();
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate message tree indices to signal index

   \param[in] orc_CurrentIndex Current child index to get signal index for

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

   \param[in] os32_MessageIndex  Top level row
   \param[in] os32_SignalIndex   Signal index
   \param[in] os32_SignalIndexL2 Second level row (optional)

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
      const std::vector<sintn> c_Order = C_CamMetTreeModel::mh_GetMultiplexerOrder(*pc_Message);
      uint32 u32_Counter = 0UL;
      if (static_cast<uint32>(os32_SignalIndex) < c_Order.size())
      {
         //Part of multiplexer
         for (uint32 u32_ItSigL1 = 0UL; u32_ItSigL1 < pc_Message->c_Signals.size(); ++u32_ItSigL1)
         {
            const C_OSCComMessageLoggerDataSignal & rc_SignalData = pc_Message->c_Signals[u32_ItSigL1];
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
         for (uint32 u32_ItSig = 0UL; u32_ItSig < pc_Message->c_Signals.size(); ++u32_ItSig)
         {
            const C_OSCComMessageLoggerDataSignal & rc_SignalData = pc_Message->c_Signals[u32_ItSig];
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
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified strings as rows

   Only handle continuous mode storage

   \param[in] orc_Data New row content

   \return
   Indices of added rows (only valid if not in unique message mode)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_CamMetTreeModel::m_AddRowsContinuousMode(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   std::vector<sint32> c_Retval;
   if (orc_Data.size() > 0)
   {
      //Columns which need to get updated on change of static message
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
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            this->mc_DataBase.push_back(*c_ItData);
            //Add tree item
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, true, false, -1);
            this->mpc_RootItemContinuous->AddChild(pc_Item);
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
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, true, false, -1);
            //1. insert until space is no longer available
            if (this->mc_DataBase.size() < C_CamMetTreeModel::mhu32_MAX_STORAGE)
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
            C_TblTreSimpleItem * const pc_Item = new C_TblTreSimpleItem();
            this->mc_DataBase[this->mu32_OldestItemIndex] = *c_ItData;
            //Replace tree item
            m_UpdateTreeItemBasedOnMessage(pc_Item, *c_ItData, true, false, -1);
            delete (this->mpc_RootItemContinuous->c_Children[0UL]);
            this->mpc_RootItemContinuous->c_Children.erase(this->mpc_RootItemContinuous->c_Children.begin());
            this->mpc_RootItemContinuous->AddChild(pc_Item);
            //Iterate
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

   \param[in] orc_Data New row content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_AddRowsUnique(const std::list<C_CamMetTreeLoggerData> & orc_Data)
{
   if (orc_Data.size() > 0)
   {
      //Every time
      for (std::list<C_CamMetTreeLoggerData>::const_iterator c_ItData = orc_Data.begin();
           c_ItData != orc_Data.end(); ++c_ItData)
      {
         const QMap<stw_scl::C_SCLString,
                    C_CamMetTreeLoggerData>::const_iterator c_ItMessage = this->mc_UniqueMessages.find(
            c_ItData->c_CanIdDec);
         const sint32 s32_MuxValue = C_CamMetTreeModel::mh_GetMultiplexerValue(*c_ItData);

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

   \param[in] orc_Message            New message
   \param[in] orc_ExistingMessageKey Key to use to find the existing message position
   \param[in] os32_MultiplexerValue  Current multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_HandleNewUniqueMessageForExistingUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                                         const stw_scl::C_SCLString & orc_ExistingMessageKey,
                                                                         const sint32 os32_MultiplexerValue)
{
   sint32 s32_Counter = 0L;
   bool q_UpdateDataTimeStamp;
   C_CamMetTreeLoggerData & rc_Message = this->mc_UniqueMessages[orc_Message.c_CanIdDec];
   const uint64 u64_PreviousAbsoluteTimestampValue = rc_Message.u64_TimeStampAbsolute;
   const uint64 u64_NewAbsoluteTimestampValue = orc_Message.u64_TimeStampAbsolute;
   uint64 u64_RelativeTimestamp;

   const uint32 u32_MsgTimeStamp = static_cast<uint32>(rc_Message.c_CanMsg.u64_TimeStamp / 1000ULL);

   // Save previous values which are not filled by C_SyvComMessageMonitor
   const C_CamMetTreeLoggerDataGreyOutInformation c_PreviousInfo =
      C_CamMetTreeModel::mh_ExtractPreviousGreyOutInformation(rc_Message, orc_Message, u32_MsgTimeStamp,
                                                              os32_MultiplexerValue,
                                                              q_UpdateDataTimeStamp);

   //Keep not redundant signal information (multiplexer + multiplexed signal)
   C_CamMetTreeModel::mh_CopyMessageWhileKeepingUniqueSignals(rc_Message, orc_Message, os32_MultiplexerValue);

   C_CamMetTreeModel::mh_ApplyPreviousGreyOutInformation(rc_Message, c_PreviousInfo, q_UpdateDataTimeStamp,
                                                         u32_MsgTimeStamp);

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
         ++s32_Counter;
      }
   }
   //Update existing tree item
   m_UpdateTreeItemBasedOnMessage(this->mpc_RootItemStatic->c_Children[static_cast<uint32>(s32_Counter)], rc_Message,
                                  false, true, s32_Counter);
   //Notify the model of data changes for all replaced items (current: all items)
   if (this->mq_UniqueMessageMode == true)
   {
      const sint32 s32_ColIDName = C_CamMetTreeModel::h_EnumToColumn(eCAN_NAME);
      const sint32 s32_ColIDDLC = C_CamMetTreeModel::h_EnumToColumn(eCAN_DLC);
      const sint32 s32_ColIDData = C_CamMetTreeModel::h_EnumToColumn(eCAN_DATA);
      const sint32 s32_ColIDTime = C_CamMetTreeModel::h_EnumToColumn(eTIME_STAMP);
      const sint32 s32_ColIDCounter = C_CamMetTreeModel::h_EnumToColumn(eCAN_COUNTER);
      //Notification for possible new child
      this->dataChanged(this->index(s32_Counter, s32_ColIDTime), this->index(s32_Counter, s32_ColIDTime));
      this->dataChanged(this->index(s32_Counter, s32_ColIDCounter), this->index(s32_Counter, s32_ColIDCounter));
      //Possible changes
      this->dataChanged(this->index(s32_Counter, s32_ColIDName), this->index(s32_Counter, s32_ColIDName));
      this->dataChanged(this->index(s32_Counter, s32_ColIDDLC), this->index(s32_Counter, s32_ColIDDLC));
      this->dataChanged(this->index(s32_Counter, s32_ColIDData), this->index(s32_Counter, s32_ColIDData));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new unique message entry

   \param[in] orc_Message           New message
   \param[in] os32_MultiplexerValue Current multiplexer value
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
   const sintn sn_EstimatedPosIndex = this->m_GetPosIndexForUniqueMessage(orc_Message.c_TimeStampAbsolute);
   //Update tree with known index
   m_UpdateTreeItemBasedOnMessage(pc_NewItem, orc_Message, false, false, sn_EstimatedPosIndex);
   this->mpc_RootItemStatic->InsertChild(sn_EstimatedPosIndex, pc_NewItem);

   if (this->mq_UniqueMessageMode == true)
   {
      //handle new row with known position index
      q_NewRow = true;

      //Notify the new row count
      this->beginInsertRows(QModelIndex(), sn_EstimatedPosIndex, sn_EstimatedPosIndex);
   }

   //Insert new item
   this->mc_UniqueMessagesOrdering.insert(orc_Message.c_TimeStampAbsolute, orc_Message.c_CanIdDec);
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

   \param[in] orc_AbsoluteTimeStamp Time stamp to work with

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
               const sint32 s32_Row = C_CamMetTreeModel::mh_GetRowForMultiplexerValue(rc_Data, c_ItValue->first);
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

   \param[in] ou32_DiffTime Difference time

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
/*! \brief  Resize string to the specified maximum length if the string exceeds this limit

   \param[in,out] orc_Str        String that will get resized if necessary
   \param[in]     os32_MaxLength Maximum length the string may not exceed after calling this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::mh_ResizeIfNecessary(QString & orc_Str, const sint32 os32_MaxLength)
{
   orc_Str.resize(std::min(orc_Str.count(), static_cast<sintn>(os32_MaxLength)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update tree item based on message

   \param[in,out] opc_Item        Item to update
   \param[in]     orc_Message     Message to update from
   \param[in]     oq_IsContinuous Flag if tree item is for continuos mode (simplified handling)
   \param[in]     oq_SignalInsert Flag if begin and end insert should be called
   \param[in]     os32_Row        Message row index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeModel::m_UpdateTreeItemBasedOnMessage(C_TblTreSimpleItem * const opc_Item,
                                                       const C_CamMetTreeLoggerData & orc_Message,
                                                       const bool oq_IsContinuous, const bool oq_SignalInsert,
                                                       const sint32 os32_Row)
{
   if (oq_IsContinuous)
   {
      //Each signal gets one child element
      if (opc_Item->c_Children.size() < orc_Message.c_Signals.size())
      {
         if ((oq_SignalInsert) && (os32_Row >= 0))
         {
            this->beginInsertRows(this->index(os32_Row, 0), static_cast<sintn>(opc_Item->c_Children.size()),
                                  static_cast<sintn>(opc_Item->c_Children.size() + orc_Message.c_Signals.size()));
         }
         //Add
         for (uint32 u32_ItSig = opc_Item->c_Children.size(); u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            opc_Item->AddChild(new C_TblTreSimpleItem());
         }
         if ((oq_SignalInsert) && (os32_Row >= 0))
         {
            this->endInsertRows();
         }
      }
      else if (opc_Item->c_Children.size() > orc_Message.c_Signals.size())
      {
         if ((oq_SignalInsert) && (os32_Row >= 0))
         {
            this->beginResetModel();
         }
         //Remove
         opc_Item->ClearChildren();
         for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
         {
            opc_Item->AddChild(new C_TblTreSimpleItem());
         }
         if ((oq_SignalInsert) && (os32_Row >= 0))
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
      const std::vector<sintn> c_Order = C_CamMetTreeModel::mh_GetMultiplexerOrder(orc_Message);
      if (c_Order.size() == 0UL)
      {
         //Normal message
         if (opc_Item->c_Children.size() < orc_Message.c_Signals.size())
         {
            //Insert
            for (uint32 u32_ItSig = opc_Item->c_Children.size(); u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
            {
               opc_Item->AddChild(new C_TblTreSimpleItem());
            }
         }
         else if (opc_Item->c_Children.size() > orc_Message.c_Signals.size())
         {
            //Reset
            opc_Item->ClearChildren();
            for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
            {
               opc_Item->AddChild(new C_TblTreSimpleItem());
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
               if ((oq_SignalInsert) && (os32_Row >= 0))
               {
                  this->beginInsertRows(this->index(os32_Row, 0), u32_InsertAt, u32_InsertAt);
               }
               C_TblTreSimpleItem * const pc_Parent = new C_TblTreSimpleItem();
               for (uint32 u32_ItChild = 0UL; u32_ItChild < u32_InsertNum; ++u32_ItChild)
               {
                  pc_Parent->AddChild(new C_TblTreSimpleItem());
               }
               opc_Item->InsertChild(u32_InsertAt, pc_Parent);
               if ((oq_SignalInsert) && (os32_Row >= 0))
               {
                  this->endInsertRows();
               }
            }
            else
            {
               if ((oq_SignalInsert) && (os32_Row >= 0))
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
               if ((oq_SignalInsert) && (os32_Row >= 0))
               {
                  this->endResetModel();
               }
            }
         }
      }
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract previous gray out value information to handle new message

   \param[in]  orc_MessagePrev         Previous message information
   \param[in]  orc_MessageNew          New message information
   \param[in]  ou32_PrevMsgTimeStamp   Previous message timestamp
   \param[in]  os32_MultiplexerValue   New multiplexer value
   \param[out] orq_UpdateDataTimeStamp Flag if data timestamp should be updated

   \return
   Current gray out value information
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeLoggerDataGreyOutInformation C_CamMetTreeModel::mh_ExtractPreviousGreyOutInformation(
   const C_CamMetTreeLoggerData & orc_MessagePrev, const C_CamMetTreeLoggerData & orc_MessageNew,
   const uint32 ou32_PrevMsgTimeStamp, const sint32 os32_MultiplexerValue, bool & orq_UpdateDataTimeStamp)
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
      c_Retval.c_MapMultiplexerValueToChangedTimeStamps[os32_MultiplexerValue] = ou32_PrevMsgTimeStamp;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply previous gray out value to new message

   \param[in,out] orc_Message                  Message to overwrite
   \param[in]     orc_StoredGrayOutInformation Stored gray out information
   \param[in]     oq_UpdateDataTimeStamp       Flag to update the data timestamps
   \param[in]     ou32_PrevMsgTimeStamp        Previous message timestamp
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
/*! \brief  Get multiplexer value ordering

   \param[in] orc_Message Message to look for multiplexer

   \return
   Multiplexer value ordering
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sintn> C_CamMetTreeModel::mh_GetMultiplexerOrder(const C_CamMetTreeLoggerData & orc_Message)
{
   std::vector<sintn> c_Order;
   //Multiplexer order
   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Message.c_Signals[u32_ItSig];
      if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         c_Order.push_back(rc_SignalData.c_RawValueDec.ToInt());
      }
   }
   //lint -e{864} Call as expected by interface
   std::sort(c_Order.begin(), c_Order.end());
   return c_Order;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the difference between the two vectors can be fixed by inserting a new item

   \param[in]  orc_ExpectedVec Expected vector
   \param[in]  orc_CurrentVec  Current vector
   \param[out] oru32_InsertAt  Position to insert new item at
   \param[out] oru32_InsertNum New value to insert at this position

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
      //Assuming we hit no problem
      q_FixByInsertingNewChild = true;
      //If orc_CurrentVec is empty, that's also valid but needs to be handled
      oru32_InsertNum = orc_ExpectedVec[0UL];
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
               //Try next position
               oru32_InsertAt = u32_Counter;
               oru32_InsertNum = orc_ExpectedVec[u32_Counter];
               ++u32_Counter;
               if (u32_Counter < orc_ExpectedVec.size())
               {
                  if (orc_ExpectedVec[u32_Counter] == orc_CurrentVec[u32_ItCurrent])
                  {
                     //pass
                     ++u32_Counter;
                  }
                  else
                  {
                     //No match possible
                     q_FixByInsertingNewChild = false;
                  }
               }
               else
               {
                  //No match possible
                  q_FixByInsertingNewChild = false;
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

   \param[in,out] orc_PreviousMessage Previous message content
   \param[in]     orc_NewMessage      New message content
   \param[in]     os32_MuxValue       Mux value
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
/*! \brief  Get multiplexer value for this instance of a message

   \param[in] orc_Message Message to search the multiplexer value in

   \return
   -1:   invalid
   else: found multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetTreeModel::mh_GetMultiplexerValue(const C_CamMetTreeLoggerData & orc_Message)
{
   sint32 s32_Value = -1;

   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      const C_OSCComMessageLoggerDataSignal & rc_Sig = orc_Message.c_Signals[u32_ItSig];
      if (rc_Sig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         s32_Value = rc_Sig.c_RawValueDec.ToInt();
         break;
      }
   }
   return s32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get row in message for multiplexer value

   \param[in] orc_Message           Message to search in
   \param[in] os32_MultiplexerValue Multiplexer value to search for

   \return
   -1:   invalid
   else: valid row (of message index)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetTreeModel::mh_GetRowForMultiplexerValue(const C_CamMetTreeLoggerData & orc_Message,
                                                       const sint32 os32_MultiplexerValue)
{
   sint32 s32_Retval = -1;
   const std::vector<sintn> c_MuxValues = C_CamMetTreeModel::mh_GetMultiplexerOrder(orc_Message);

   for (uint32 u32_It = 0UL; u32_It < c_MuxValues.size(); ++u32_It)
   {
      if (c_MuxValues[u32_It] == os32_MultiplexerValue)
      {
         s32_Retval = static_cast<sint32>(u32_It);
      }
   }
   return s32_Retval;
}
