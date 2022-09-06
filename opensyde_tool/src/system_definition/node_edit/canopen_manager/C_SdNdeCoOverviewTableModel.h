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
#include "stwtypes.h"
#include "C_OSCNode.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   stw_types::uint32 GetNodeIndex(void) const;
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 MapRowToNodeConfig(const stw_types::sint32 os32_RowIndex, E_NodeType & ore_NodeType,
                                        stw_types::uint8 & oru8_InterfaceNumber,
                                        stw_opensyde_core::C_OSCCanInterfaceId & orc_CanInterfaceId);

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

private:
   struct C_CoTableData
   {
      QString c_Interface;
      QString c_Node;
      QString c_Role;
      QString c_CANopenId;
      QString c_TPDOs;
      QString c_RPDOs;
   };

   struct C_CoNodeConfig
   {
      E_NodeType e_NodeType;
      stw_types::uint8 u8_InterfaceNumber;
      stw_opensyde_core::C_OSCCanInterfaceId c_CanInterfaceId;
   };

   struct C_CoInfo
   {
      C_CoTableData c_CoTableData;
      C_CoNodeConfig c_CoNodeConfig;
   };

   enum E_PdoType
   {
      eTX,
      eRX
   };

   struct C_PdoCount
   {
      stw_types::uint16 u16_TxTotal;
      stw_types::uint16 u16_TxActive;
      stw_types::uint16 u16_RxTotal;
      stw_types::uint16 u16_RxActive;
   };

   void m_GetPdoCountsByManager(const stw_opensyde_core::C_OSCNode * const opc_Node,
                                const stw_types::uint8 ou8_InterfaceIndex, C_PdoCount * const opc_PdoManagerCnt,
                                std::map<stw_types::uint32,
                                         C_PdoCount> * const opc_PdoDeviceMap)
   const;
   void m_GetResultStringsOfPdos(const C_PdoCount * const opc_PdoCount, QString & orc_ActiveTPDOs,
                                 QString & orc_ActiveRPDOs) const;
   void m_AddToMessageDeviceCount(const stw_types::uint32 ou32_NodeId, const bool oq_MessageActive,
                                  const E_PdoType oe_PdoType, std::map<stw_types::uint32,
                                                                       C_PdoCount> * const opc_PdoDeviceMap)
   const;

   void m_FillCoInfo(void);

   stw_types::uint32 mu32_NodeIndex;
   std::vector<C_CoInfo> mc_CoInfoAll;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
