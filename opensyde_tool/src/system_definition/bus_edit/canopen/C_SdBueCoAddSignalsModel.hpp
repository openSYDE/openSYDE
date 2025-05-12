//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model part for add signals tree
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSMODEL_HPP
#define C_SDBUECOADDSIGNALSMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreItem.hpp"
#include "C_TblTreModel.hpp"
#include "C_OscCanOpenObjectDictionary.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"
#include "C_OscCanOpenManagerMappableSignal.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   ~C_SdBueCoAddSignalsModel(void) override;
   void SetIndex(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void PrepareCleanUp(void);
   const stw::opensyde_core::C_OscCanOpenManagerMappableSignal * GetDataForIndex(const uint32_t ou32_ObjectIndex,
                                                                                 const uint32_t ou32_SignalIndex) const;
   static std::map<uint32_t, std::vector<uint32_t> > h_GetUniqueIndices(const QModelIndexList & orc_ModelIndices);

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

private:
   const QIcon mc_IconSignal;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_MessageId;
   std::map<uint32_t,  std::map<uint32_t, uint32_t> > mc_MapObjectIndexToVectorIndex;

   void m_Init(void);
   void m_InitObjectMap(void);
   void m_InitTopLevel(C_TblTreItem & orc_RootNode);
   void m_InitObjectNode(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex);
   void m_InitObjectNodeContent(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex);
   void m_InitObjectNodeChildren(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex);
   void m_InitSignalNodeContent(C_TblTreItem & orc_SignalNode, const uint32_t ou32_ObjectIndex,
                                const uint32_t ou32_ObjectSubIndex);
   static void mh_DecodeIndex(const QModelIndex & orc_ModelIndex, uint32_t & oru32_ObjectIndex,
                              uint32_t & oru32_SignalIndex, bool & orq_IsSignal);
   void m_GetData(const C_SdBueCoAddSignalsModel::E_Columns oe_Column, const uint32_t ou32_ObjectIndex,
                  const uint32_t ou32_SignalIndex, const bool oq_IsSignal, const int32_t os32_Role,
                  QVariant & orc_Output) const;
   const stw::opensyde_core::C_OscCanOpenObjectDictionary * m_GetEdsDictionary(void) const;
   const std::vector<stw::opensyde_core::C_OscCanOpenManagerMappableSignal> * m_GetMappableSignals(void) const;
   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo * m_GetDeviceInfo(void) const;
   static const stw::opensyde_core::C_OscCanOpenObjectData * mh_GetCanOpenObject(
      const stw::opensyde_core::C_OscCanOpenObjectDictionary & orc_Dictionary, const uint32_t ou32_ObjectIndex,
      const uint32_t ou32_SignalIndex);
   bool m_CheckSignalRelevant(const stw::opensyde_core::C_OscCanOpenManagerMappableSignal & orc_Signal,
                              const stw::opensyde_core::C_OscCanOpenObjectDictionary & orc_Dictionary) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
