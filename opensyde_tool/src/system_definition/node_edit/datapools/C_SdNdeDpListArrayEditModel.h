//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTARRAYEDITMODEL_H
#define C_SDNDEDPLISTARRAYEDITMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_SdNdeDpUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListArrayEditModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListArrayEditModel(QObject * const opc_Parent = NULL);

   void SetElement(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                   const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                   const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                   const stw_types::uint32 & oru32_DataSetIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
               const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
               const stw_types::uint32 & oru32_DataSetIndex) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   void HandleDataChange(const stw_types::uint32 & oru32_Column);
   stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type GetType(void) const;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCElement(void) const;
   C_SdNdeDpUtil::E_ArrayEditType GetArrayEditType(void) const;
   void Reset(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                               const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                               const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                               const stw_types::uint32 & oru32_DataSetIndex);
   void SigDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                      const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                      const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                      const stw_types::uint32 & oru32_DataSetIndex, const stw_types::uint32 & oru32_ArrayElementIndex,
                      const QVariant & orc_NewData);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   stw_types::uint32 mu32_ElementIndex;
   C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   stw_types::uint32 mu32_DataSetIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
