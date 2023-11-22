//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTARRAYEDITMODEL_HPP
#define C_SDNDEDPLISTARRAYEDITMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListArrayEditModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListArrayEditModel(QObject * const opc_Parent = NULL);

   void SetElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                   const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                   const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType, const uint32_t & oru32_DataSetIndex);
   bool Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_ListIndex,
               const uint32_t & oru32_ElementIndex, const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
               const uint32_t & oru32_DataSetIndex) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   void HandleDataChange(const uint32_t & oru32_Column);
   stw::opensyde_core::C_OscNodeDataPoolContent::E_Type GetType(void) const;
   const stw::opensyde_core::C_OscNodeDataPoolListElement * GetOscElement(void) const;
   C_SdNdeDpUtil::E_ArrayEditType GetArrayEditType(void) const;
   void Reset(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                               const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                               const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                               const uint32_t & oru32_DataSetIndex);
   void SigDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                      const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                      const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType, const uint32_t & oru32_DataSetIndex,
                      const uint32_t & oru32_ArrayElementIndex, const QVariant & orc_NewData);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   uint32_t mu32_ElementIndex;
   C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   uint32_t mu32_DataSetIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
