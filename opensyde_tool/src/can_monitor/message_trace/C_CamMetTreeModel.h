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
#include "C_CamMetTreeLoggerData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamMetTreeModel :
   public QAbstractItemModel
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
   void SetSelection(const stw_types::sint32 os32_SelectedParentRow, const stw_types::sint32 os32_SelectedChildRow);
   void SetDisplayTree(const bool oq_Value);
   void SetDisplayUniqueMessages(const bool oq_Value);
   void SetDisplayAsHex(const bool oq_Value);
   void SetDisplayTimestampRelative(const bool oq_Value);
   bool GetDisplayAsHex(void) const;
   bool GetDisplayTimestampRelative(void) const;

   std::vector<C_CamMetTreeLoggerData *> GetAllMessagesForProtocolChange(void);
   void SignalProtocolChange(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Header:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QModelIndex index(const stw_types::sintn osn_Row, const stw_types::sintn osn_Column,
                             const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QModelIndex parent(const QModelIndex & orc_Index) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);
   const C_CamMetTreeLoggerData * GetMessageData(const stw_types::sintn osn_Row) const;

private:
   bool mq_DisplayTree;
   bool mq_UniqueMessageMode;
   bool mq_DisplayAsHex;
   bool mq_DisplayDataAsHex;
   bool mq_DisplayTimestampRelative;
   QMap<stw_scl::C_SCLString, C_CamMetTreeLoggerData> mc_UniqueMessages;
   std::vector<C_CamMetTreeLoggerData> mc_DataBase;
   static const stw_types::uint32 mhu32_MAX_STORAGE;
   bool mq_DataUnlocked;
   stw_types::uint32 mu32_OldestItemIndex;
   stw_types::sint32 ms32_SelectedParentRow;
   stw_types::sint32 ms32_SelectedChildRow;
   std::vector<QColor> mc_FontTransparcencyColors;
   QTimer mc_GrayOutTimer;
   bool mq_GrayOutPause;
   stw_types::uint32 mu32_GrayOutPauseTimeStamp;
   static const QString mhc_IconMessage;
   static const QString mhc_IconMessageSelected;
   static const QString mhc_IconSignal;
   static const QString mhc_IconSignalSelected;

   void m_GrayOutTimer(void);
   static stw_types::sintn mh_GetTransparencyStep(const stw_types::uint32 ou32_DiffTime);
   static std::vector<QStringList> mh_GetCount(const C_CamMetTreeLoggerData & orc_Message);
   static void mh_ResizeIfNecessary(QString & orc_Str, const stw_types::sint32 os32_MaxLength);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
