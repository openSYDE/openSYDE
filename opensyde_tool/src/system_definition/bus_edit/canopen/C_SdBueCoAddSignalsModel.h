//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model part for add signals tree
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSMODEL_H
#define C_SDBUECOADDSIGNALSMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreItem.h"
#include "C_TblTreModel.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"
#include "C_OSCCanOpenManagerMappableSignal.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueCoAddSignalsModel :
   public C_TblTreModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eNAME,
      eLENGTH
   };

   C_SdBueCoAddSignalsModel(QObject * const opc_Parent = NULL);
   ~C_SdBueCoAddSignalsModel(void);
   void SetIndex(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void PrepareCleanUp(void);
   const stw_opensyde_core::C_OSCCanOpenManagerMappableSignal * GetDataForIndex(
      const stw_types::uint32 ou32_ObjectIndex, const stw_types::uint32 ou32_SignalIndex) const;
   static std::map<stw_types::uint32, std::vector<stw_types::uint32> > h_GetUniqueIndices(
      const QModelIndexList & orc_ModelIndices);

   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);

private:
   const QIcon mc_IconSignal;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   std::map<stw_types::uint32,  std::map<stw_types::uint32, stw_types::uint32> > mc_MapObjectIndexToVectorIndex;

   void m_Init(void);
   void m_InitObjectMap(void);
   void m_InitTopLevel(C_TblTreItem & orc_RootNode);
   void m_InitObjectNode(C_TblTreItem & orc_ObjectNode, const stw_types::uint32 ou32_ObjectIndex);
   void m_InitObjectNodeContent(C_TblTreItem & orc_ObjectNode, const stw_types::uint32 ou32_ObjectIndex);
   void m_InitObjectNodeChildren(C_TblTreItem & orc_ObjectNode, const stw_types::uint32 ou32_ObjectIndex);
   void m_InitSignalNodeContent(C_TblTreItem & orc_SignalNode, const stw_types::uint32 ou32_ObjectIndex,
                                const stw_types::uint32 ou32_ObjectSubIndex);
   static void mh_DecodeIndex(const QModelIndex & orc_ModelIndex, stw_types::uint32 & oru32_ObjectIndex,
                              stw_types::uint32 & oru32_SignalIndex, bool & orq_IsSignal);
   void m_GetData(const C_SdBueCoAddSignalsModel::E_Columns oe_Column, const stw_types::uint32 ou32_ObjectIndex,
                  const stw_types::uint32 ou32_SignalIndex, const bool oq_IsSignal, const stw_types::sintn osn_Role,
                  QVariant & orc_Output) const;
   const stw_opensyde_core::C_OSCCanOpenObjectDictionary * m_GetEdsDictionary(void) const;
   const std::vector<stw_opensyde_core::C_OSCCanOpenManagerMappableSignal> * m_GetMappableSignals(void) const;
   const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo * m_GetDeviceInfo(void) const;
   static const stw_opensyde_core::C_OSCCanOpenObject * mh_GetCanOpenObject(
      const stw_opensyde_core::C_OSCCanOpenObjectDictionary & orc_Dictionary,
      const stw_types::uint32 ou32_ObjectIndex, const stw_types::uint32 ou32_SignalIndex);
   bool m_CheckSignalRelevant(const stw_opensyde_core::C_OSCCanOpenManagerMappableSignal & orc_Signal,
                              const stw_opensyde_core::C_OSCCanOpenObjectDictionary & orc_Dictionary) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
