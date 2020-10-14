//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGTREEMODEL_H
#define C_SDNDEHALCCONFIGTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QBitArray>

#include "C_TblTreModel.h"
#include "C_OSCHalcConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   ~C_SdNdeHalcConfigTreeModel(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation,
                               const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::DisplayRole))
   const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   stw_types::uint32 GetNodeIndex(void) const;
   void SetHalcChannelUseCase(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                              const stw_types::uint32 ou32_UseCaseIndex, const bool oq_UseChannelIndex);
   void Clear(void);

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const C_SdNdeHalcConfigTreeModel::E_Columns & ore_Value);

private:
   C_SdNdeHalcConfigTreeModel(const C_SdNdeHalcConfigTreeModel &);
   C_SdNdeHalcConfigTreeModel & operator =(const C_SdNdeHalcConfigTreeModel &);

   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DomainIndex;
   stw_types::uint32 mu32_ChannelIndex; ///< Only valid if mq_ChannelCase is true
   stw_types::uint32 mu32_UseCaseIndex; ///< Only valid if mq_ChannelCase is true
   bool mq_ChannelCase;                 ///< Flag to distinguish between domain only case and channel & use case case

   static bool mh_GetParameterElementIndexe(const QModelIndex & orc_Index, stw_types::uint32 & oru32_ParameterIndex,
                                            stw_types::uint32 & oru32_ParameterElementIndex);
   const stw_opensyde_core::C_OSCHalcConfigParameter * m_GetParameterElement(const QModelIndex & orc_Index) const;
   const stw_opensyde_core::C_OSCHalcDefElement * m_GetDefParameterElement(const QModelIndex & orc_Index) const;
   static QStringList mh_ConvertEnumsToStringList(const stw_opensyde_core::C_OSCHalcDefContent & orc_Value);
   static QStringList mh_ConvertBitmasksToStringList(const stw_opensyde_core::C_OSCHalcDefContent & orc_Value);
   static QString mh_ConvertBitmasksToString(const stw_opensyde_core::C_OSCHalcDefContent & orc_Value);
   static QBitArray mh_ConvertBitmasksToBitArray(const stw_opensyde_core::C_OSCHalcDefContent & orc_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
