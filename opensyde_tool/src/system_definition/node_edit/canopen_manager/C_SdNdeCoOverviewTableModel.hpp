//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOOVERVIEWTABLEMODEL_H
#define C_SDNDECOOVERVIEWTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include <QString>
#include "stwtypes.hpp"
#include "C_OscNode.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoOverviewTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eINTERFACE,
      eNODE,
      eROLE,
      eCANOPENNODEID,
      eTPDOS,
      eRPDOS
   };

   enum E_NodeType
   {
      eMANAGER = 0,
      eDEVICE
   };

   C_SdNdeCoOverviewTableModel(QObject * const opc_Parent = NULL);

   void UpdateData(void);
   uint32_t GetNodeIndex(void) const;
   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   int32_t MapRowToNodeConfig(const int32_t os32_RowIndex, E_NodeType & ore_NodeType, uint8_t & oru8_InterfaceNumber,
                              stw::opensyde_core::C_OscCanInterfaceId & orc_CanInterfaceId);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

private:
   class C_CoTableData
   {
   public:
      QString c_Interface;
      QString c_Node;
      QString c_Role;
      QString c_CanOpenId;
      QString c_TxPdos;
      QString c_RxPdos;
   };

   class C_CoNodeConfig
   {
   public:
      E_NodeType e_NodeType;
      uint8_t u8_InterfaceNumber;
      stw::opensyde_core::C_OscCanInterfaceId c_CanInterfaceId;
   };

   class C_CoInfo
   {
   public:
      C_CoTableData c_CoTableData;
      C_CoNodeConfig c_CoNodeConfig;
   };

   enum E_PdoType
   {
      eTX,
      eRX
   };

   class C_PdoCount
   {
   public:
      uint16_t u16_TxTotal;
      uint16_t u16_TxActive;
      uint16_t u16_RxTotal;
      uint16_t u16_RxActive;
   };

   void m_GetPdoCountsByManager(const stw::opensyde_core::C_OscNode * const opc_Node, const uint8_t ou8_InterfaceIndex,
                                C_PdoCount * const opc_PdoManagerCnt, std::map<uint32_t,
                                                                               C_PdoCount> * const opc_PdoDeviceMap)
   const;
   void m_GetResultStringsOfPdos(const C_PdoCount * const opc_PdoCount, QString & orc_ActiveTxPdos,
                                 QString & orc_ActiveRxPdos) const;
   void m_AddToMessageDeviceCount(const uint32_t ou32_NodeId, const bool oq_MessageActive, const E_PdoType oe_PdoType,
                                  std::map<uint32_t,
                                           C_PdoCount> * const opc_PdoDeviceMap)
   const;

   void m_FillCoInfo(void);

   uint32_t mu32_NodeIndex;
   std::vector<C_CoInfo> mc_CoInfoAll;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
