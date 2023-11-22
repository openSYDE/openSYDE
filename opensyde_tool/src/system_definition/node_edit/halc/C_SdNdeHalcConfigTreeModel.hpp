//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGTREEMODEL_HPP
#define C_SDNDEHALCCONFIGTREEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QBitArray>

#include "C_TblTreModel.hpp"
#include "C_OscHalcConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigTreeModel :
   public C_TblTreModel
{
public:
   enum E_Columns
   {
      eNAME = 0,
      eVALUE,
      eDESCRIPTION
   };

   C_SdNdeHalcConfigTreeModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcConfigTreeModel(void) override;

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                       const int32_t os32_Role = static_cast<int32_t>(Qt::DisplayRole))
   const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   void SetNode(const uint32_t ou32_NodeIndex);
   uint32_t GetNodeIndex(void) const;
   void SetHalcChannelUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                              const uint32_t ou32_UseCaseIndex, const bool oq_UseChannelIndex);
   void Clear(void);

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const C_SdNdeHalcConfigTreeModel::E_Columns & ore_Value);

private:
   C_SdNdeHalcConfigTreeModel(const C_SdNdeHalcConfigTreeModel &);
   C_SdNdeHalcConfigTreeModel & operator =(const C_SdNdeHalcConfigTreeModel &) &;

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DomainIndex;
   uint32_t mu32_ChannelIndex; ///< Only valid if mq_ChannelCase is true
   uint32_t mu32_UseCaseIndex; ///< Only valid if mq_ChannelCase is true
   bool mq_ChannelCase;        ///< Flag to distinguish between domain only case and channel & use case case

   static bool mh_GetParameterElementIndexe(const QModelIndex & orc_Index, uint32_t & oru32_ParameterIndex,
                                            uint32_t & oru32_ParameterElementIndex);
   const stw::opensyde_core::C_OscHalcConfigParameter * m_GetParameterElement(const QModelIndex & orc_Index) const;
   const stw::opensyde_core::C_OscHalcDefElement * m_GetDefParameterElement(const QModelIndex & orc_Index) const;
   bool m_CheckAvailability(const stw::opensyde_core::C_OscHalcDefElement & orc_Parameter) const;

   static QStringList mh_ConvertEnumsToStringList(const stw::opensyde_core::C_OscHalcDefContent & orc_Value);
   static QStringList mh_ConvertBitmasksToStringList(const stw::opensyde_core::C_OscHalcDefContent & orc_Value);
   static QString mh_ConvertBitmasksToString(const stw::opensyde_core::C_OscHalcDefContent & orc_Value);
   static QBitArray mh_ConvertBitmasksToBitArray(const stw::opensyde_core::C_OscHalcDefContent & orc_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
