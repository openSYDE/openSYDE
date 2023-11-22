//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance model (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREEMODEL_HPP
#define C_CAMMETTREEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QMap>
#include <QAbstractItemModel>
#include <QColor>
#include <QTimer>
#include <QPixmap>

#include "C_SclString.hpp"
#include "C_TblTreSimpleModel.hpp"
#include "C_CamMetTreeLoggerData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamMetTreeModel :
   public C_TblTreSimpleModel
{
public:
   enum E_Columns
   {
      eTIME_STAMP,
      eCAN_ID,
      eCAN_NAME,
      eCAN_DIR,
      eCAN_DLC,
      eCAN_DATA,
      eCAN_COUNTER,
      eCAN_STATUS,
   };

   C_CamMetTreeModel(QObject * const opc_Parent = NULL);
   ~C_CamMetTreeModel(void) override;

   void Continue(void);
   void Pause(void);
   void Stop(void);
   void Start(void);

   //Necessary to reduce number of calls
   void LockData(void);
   void UnlockData(void);

   //Access
   void ActionClearData(void);
   std::vector<int32_t> AddRows(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void SetSelection(const int32_t os32_SelectedParentRow, const int32_t os32_SelectedChildIndex);
   void SetDisplayTree(const bool oq_Value);
   void SetDisplayUniqueMessages(const bool oq_Value);
   void SetDisplayAsHex(const bool oq_Value);
   void SetDisplayTimestampRelative(const bool oq_Value);
   void SetDisplayTimestampAbsoluteTimeOfDay(const bool oq_Value);
   bool GetDisplayAsHex(void) const;
   bool GetDisplayTimestampRelative(void) const;
   bool GetDisplayTimestampAbsoluteTimeOfDay(void) const;
   void SetTraceBufferSize(const uint32_t ou32_Value);

   std::vector<C_CamMetTreeLoggerData *> GetAllMessagesForProtocolChange(void);
   void SignalProtocolChange(void);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                       const int32_t os32_Role = static_cast<int32_t>(Qt::DisplayRole))
   const override;
   QModelIndex index(const int32_t os32_Row, const int32_t os32_Column,
                     const QModelIndex & orc_Parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex & orc_Index) const override; //lint !e1511  we want to hide the base
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t os32_Column);
   static int32_t h_EnumToColumn(const E_Columns oe_Value);
   const C_CamMetTreeLoggerData * GetMessageData(const int32_t os32_Row) const;
   int32_t SearchMessageData(const QString & orc_SearchString, const bool oq_Next, int32_t & ors32_SignalRow,
                             int32_t & ors32_MultiplexedSignalRow);
   uint32_t TranslateTreeRowsToSignalIndex(const QModelIndex & orc_CurrentIndex) const;
   uint32_t TranslateTreeRowsToSignalIndex(const int32_t os32_MessageIndex, const int32_t os32_SignalIndex,
                                           const int32_t os32_SignalIndexL2 = -1) const;

   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

private:
   bool mq_DisplayTree;
   bool mq_UniqueMessageMode;
   bool mq_DisplayAsHex;
   bool mq_DisplayTimestampRelative;
   bool mq_DisplayTimestampAbsoluteTimeOfDay;
   uint32_t mu32_TraceBufferSizeUsed;   // The set and used buffer size
   uint32_t mu32_TraceBufferSizeConfig; // The set buffer size by user. Will be set after restart the
   // communication
   QMap<stw::scl::C_SclString, C_CamMetTreeLoggerData> mc_UniqueMessages;
   QMap<stw::scl::C_SclString, stw::scl::C_SclString> mc_UniqueMessagesOrdering;
   std::vector<C_CamMetTreeLoggerData> mc_DataBase;
   bool mq_DataUnlocked;
   uint32_t mu32_OldestItemIndex;
   int32_t ms32_SelectedParentRow;
   std::set<uint16_t> mc_SelectedChildBytes;
   std::vector<QColor> mc_FontTransparcencyColors;
   QTimer mc_GrayOutTimer;
   bool mq_GrayOutPause;
   uint32_t mu32_GrayOutPauseTimeStamp;
   C_TblTreSimpleItem * const mpc_RootItemContinuous;
   C_TblTreSimpleItem * const mpc_RootItemStatic;
   int32_t ms32_LastSearchedMessageRow;
   int32_t ms32_LastSearchedSignalRow;
   int32_t ms32_LastSearchedSignalRowMultiplexed;
   bool mq_IsLastSearchedSignalLastOfMessage;
   QString mc_LastSearchedString;
   static const QString mhc_ICON_MESSAGE;
   static const QString mhc_ICON_MESSAGE_SELECTED;
   static const QString mhc_ICON_SIGNAL;
   static const QString mhc_ICON_SIGNAL_SELECTED;

   //Avoid call
   C_CamMetTreeModel(const C_CamMetTreeModel &);
   C_CamMetTreeModel & operator =(const C_CamMetTreeModel &) &;

   void m_AdaptTraceBufferSize(void);

   std::vector<int32_t> m_AddRowsContinuousMode(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void m_AddRowsUnique(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void m_HandleNewUniqueMessageForExistingUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                         const stw::scl::C_SclString & orc_ExistingMessageKey,
                                                         const int32_t os32_MultiplexerValue);
   void m_HandleNewUniqueMessage(const C_CamMetTreeLoggerData & orc_Message, const int32_t os32_MultiplexerValue);
   int32_t m_GetPosIndexForUniqueMessage(const stw::scl::C_SclString & orc_AbsoluteTimeStamp) const;

   void m_GrayOutTimer(void);
   static int32_t mh_GetTransparencyStep(const uint32_t ou32_DiffTime);
   static std::vector<QStringList> mh_GetCount(const C_CamMetTreeLoggerData & orc_Message);
   static void mh_ResizeIfNecessary(QString & orc_Str, const int32_t os32_MaxLength);
   void m_UpdateTreeItemBasedOnMessage(C_TblTreSimpleItem * const opc_Item, const C_CamMetTreeLoggerData & orc_Message,
                                       const bool oq_SignalInsert, const int32_t os32_MessageRow);
   bool m_CheckDataForSearch(const C_CamMetTreeLoggerData & orc_Data, const QString & orc_SearchString,
                             const bool oq_Next, int32_t & ors32_SignalIndex, bool & orq_LastSignalChecked) const;
   bool m_CheckMessageDataForSearch(const C_CamMetTreeLoggerData & orc_Data, const QString & orc_SearchString) const;
   bool m_CheckSignalDataForSearch(const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_SignalData,
                                   const QString & orc_SearchString) const;
   void m_GetMultiplexedMsgSignalRow(
      const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals,
      const uint32_t ou32_SignalIndexToCheck, int32_t & ors32_SignalRow,
      int32_t & ors32_MultiplexedSignalRow) const;

   void m_ResetSearch(void);
   static C_CamMetTreeLoggerDataGreyOutInformation mh_ExtractPreviousGreyOutInformation(
      const C_CamMetTreeLoggerData & orc_MessagePrev, const C_CamMetTreeLoggerData & orc_MessageNew,
      const uint32_t ou32_PrevMsgTimeStamp, const uint32_t ou32_NewMsgTimeStamp,
      const int32_t os32_MultiplexerValue, bool & orq_UpdateDataTimeStamp);
   static void mh_ApplyPreviousGreyOutInformation(C_CamMetTreeLoggerData & orc_Message,
                                                  const C_CamMetTreeLoggerDataGreyOutInformation & orc_StoredGrayOutInformation, const bool oq_UpdateDataTimeStamp, const uint32_t ou32_PrevMsgTimeStamp);
   static bool mh_CheckForFixByInsertingNewChild(const std::vector<uint32_t> & orc_ExpectedVec,
                                                 const std::vector<uint32_t> & orc_CurrentVec,
                                                 uint32_t & oru32_InsertAt, uint32_t & oru32_InsertNum);
   static void mh_CopyMessageWhileKeepingUniqueSignals(C_CamMetTreeLoggerData & orc_PreviousMessage,
                                                       const C_CamMetTreeLoggerData & orc_NewMessage,
                                                       const int32_t os32_MuxValue);
   static uint32_t mh_TranslateTreeRowsToSignalIndex(
      const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals,
      const int32_t os32_SignalIndex, const int32_t os32_SignalIndexL2 = -1);

   static bool mh_IsMessageMultiplexed(const C_CamMetTreeLoggerData & orc_Data);
   static void mh_SortMultiplexedSignals(
      std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals);

   bool m_IsStatusValid(const C_CamMetTreeLoggerData & orc_CurrentMessage) const;

   class C_LoggerDataComparatorForMultiplexer
   {
   public:
      bool operator ()(const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Data1,
                       const stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Data2) const;
   };
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
