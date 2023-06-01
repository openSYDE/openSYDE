//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for adding messages from J1939 Catalog
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939ADDMESSAGESFROMCATALOGTABLEMODEL_H
#define C_SDBUEJ1939ADDMESSAGESFROMCATALOGTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QAbstractTableModel>

#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_CieConverter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueJ1939AddMessagesFromCatalogTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eICON = 0,
      eCAN_ID,
      eJ1939_PGN,
      eNAME,
      eCOMMENT
   };

   C_SdBueJ1939AddMessagesFromCatalogTableModel(QObject * const opc_Parent = NULL);
   void UpdateData(const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported);
   const C_CieConverter::C_CieNodeMessage * GetDataForIndex(const uint32_t ou32_MessageIndex) const;

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

private:
   class C_MsgTableData
   {
   public:
      QVariant c_Icon;
      QVariant c_CanId;
      QVariant c_J1939Pgn;
      QString c_Name;
      QString c_Comment;
   };

   bool m_IsJ1939SpecificMessage(const C_CieConverter::C_CieNodeMessage & orc_Message) const;
   void m_FillMessageInfo(void);
   std::vector<C_MsgTableData> mc_MessageInfoList;
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> mc_MessagesImportedFromCatalog;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
