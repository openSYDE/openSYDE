//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance model (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREEMODEL_H
#define C_CAMMETTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QMap>
#include <QAbstractItemModel>
#include <QColor>
#include <QTimer>
#include <QPixmap>

#include "CSCLString.h"
#include "C_TblTreSimpleModel.h"
#include "C_CamMetTreeLoggerData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
      eCAN_COUNTER
   };

   C_CamMetTreeModel(QObject * const opc_Parent = NULL);
   ~C_CamMetTreeModel(void);

   void Continue(void);
   void Pause(void);
   void Stop(void);
   void Start(void);

   //Necessary to reduce number of calls
   void LockData(void);
   void UnlockData(void);

   //Access
   void ActionClearData(void);
   std::vector<stw_types::sint32> AddRows(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void SetSelection(const stw_types::sint32 os32_SelectedParentRow, const stw_types::sint32 os32_SelectedChildIndex);
   void SetDisplayTree(const bool oq_Value);
   void SetDisplayUniqueMessages(const bool oq_Value);
   void SetDisplayAsHex(const bool oq_Value);
   void SetDisplayTimestampRelative(const bool oq_Value);
   bool GetDisplayAsHex(void) const;
   bool GetDisplayTimestampRelative(void) const;

   std::vector<C_CamMetTreeLoggerData *> GetAllMessagesForProtocolChange(void);
   void SignalProtocolChange(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation,
                               const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::DisplayRole))
   const override;
   virtual QModelIndex index(const stw_types::sintn osn_Row, const stw_types::sintn osn_Column,
                             const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QModelIndex parent(const QModelIndex & orc_Index) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);
   const C_CamMetTreeLoggerData * GetMessageData(const stw_types::sintn osn_Row) const;
   stw_types::uint32 TranslateTreeRowsToSignalIndex(const QModelIndex & orc_CurrentIndex) const;
   stw_types::uint32 TranslateTreeRowsToSignalIndex(const stw_types::sint32 os32_MessageIndex,
                                                    const stw_types::sint32 os32_SignalIndex,
                                                    const stw_types::sint32 os32_SignalIndexL2 = -1) const;

private:
   bool mq_DisplayTree;
   bool mq_UniqueMessageMode;
   bool mq_DisplayAsHex;
   bool mq_DisplayDataAsHex;
   bool mq_DisplayTimestampRelative;
   QMap<stw_scl::C_SCLString, C_CamMetTreeLoggerData> mc_UniqueMessages;
   QMap<stw_scl::C_SCLString, stw_scl::C_SCLString> mc_UniqueMessagesOrdering;
   std::vector<C_CamMetTreeLoggerData> mc_DataBase;
   static const stw_types::uint32 mhu32_MAX_STORAGE;
   bool mq_DataUnlocked;
   stw_types::uint32 mu32_OldestItemIndex;
   stw_types::sint32 ms32_SelectedParentRow;
   std::set<stw_types::uint16> mc_SelectedChildBytes;
   std::vector<QColor> mc_FontTransparcencyColors;
   QTimer mc_GrayOutTimer;
   bool mq_GrayOutPause;
   stw_types::uint32 mu32_GrayOutPauseTimeStamp;
   C_TblTreSimpleItem * const mpc_RootItemContinuous;
   C_TblTreSimpleItem * const mpc_RootItemStatic;
   static const QString mhc_IconMessage;
   static const QString mhc_IconMessageSelected;
   static const QString mhc_IconSignal;
   static const QString mhc_IconSignalSelected;

   //Avoid call
   C_CamMetTreeModel(const C_CamMetTreeModel &);
   C_CamMetTreeModel & operator =(const C_CamMetTreeModel &);

   std::vector<stw_types::sint32> m_AddRowsContinuousMode(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void m_AddRowsUnique(const std::list<C_CamMetTreeLoggerData> & orc_Data);
   void m_HandleNewUniqueMessageForExistingUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                                         const stw_scl::C_SCLString & orc_ExistingMessageKey,
                                                         const stw_types::sint32 os32_MultiplexerValue);
   void m_HandleNewUniqueMessage(const C_CamMetTreeLoggerData & orc_Message,
                                 const stw_types::sint32 os32_MultiplexerValue);
   stw_types::sint32 m_GetPosIndexForUniqueMessage(const stw_scl::C_SCLString & orc_AbsoluteTimeStamp) const;

   void m_GrayOutTimer(void);
   static stw_types::sintn mh_GetTransparencyStep(const stw_types::uint32 ou32_DiffTime);
   static std::vector<QStringList> mh_GetCount(const C_CamMetTreeLoggerData & orc_Message);
   static void mh_ResizeIfNecessary(QString & orc_Str, const stw_types::sint32 os32_MaxLength);
   void m_UpdateTreeItemBasedOnMessage(C_TblTreSimpleItem * const opc_Item, const C_CamMetTreeLoggerData & orc_Message,
                                       const bool oq_SignalInsert, const stw_types::sint32 os32_Row);
   static C_CamMetTreeLoggerDataGreyOutInformation mh_ExtractPreviousGreyOutInformation(
      const C_CamMetTreeLoggerData & orc_MessagePrev, const C_CamMetTreeLoggerData & orc_MessageNew,
      const stw_types::uint32 ou32_PrevMsgTimeStamp, const stw_types::uint32 ou32_NewMsgTimeStamp,
      const stw_types::sint32 os32_MultiplexerValue, bool & orq_UpdateDataTimeStamp);
   static void mh_ApplyPreviousGreyOutInformation(C_CamMetTreeLoggerData & orc_Message,
                                                  const C_CamMetTreeLoggerDataGreyOutInformation & orc_StoredGrayOutInformation, const bool oq_UpdateDataTimeStamp, const stw_types::uint32 ou32_PrevMsgTimeStamp);
   static std::vector<stw_types::sintn> mh_GetMultiplexerOrder(const C_CamMetTreeLoggerData & orc_Message);
   static bool mh_CheckForFixByInsertingNewChild(const std::vector<stw_types::uint32> & orc_ExpectedVec,
                                                 const std::vector<stw_types::uint32> & orc_CurrentVec,
                                                 stw_types::uint32 & oru32_InsertAt,
                                                 stw_types::uint32 & oru32_InsertNum);
   static void mh_CopyMessageWhileKeepingUniqueSignals(C_CamMetTreeLoggerData & orc_PreviousMessage,
                                                       const C_CamMetTreeLoggerData & orc_NewMessage,
                                                       const stw_types::sint32 os32_MuxValue);
   static stw_types::sint32 mh_GetMultiplexerValue(const C_CamMetTreeLoggerData & orc_Message);
   static stw_types::sint32 mh_GetRowForMultiplexerValue(const C_CamMetTreeLoggerData & orc_Message,
                                                         const stw_types::sint32 os32_MultiplexerValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
