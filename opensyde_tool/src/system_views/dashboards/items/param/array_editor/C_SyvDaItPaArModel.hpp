//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAARMODEL_HPP
#define C_SYVDAITPAARMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaArModel :
   public QAbstractTableModel
{
public:
   explicit C_SyvDaItPaArModel(QObject * const opc_Parent = NULL);

   void SetElement(const uint32_t ou32_ElementIndex,
                   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                   const bool oq_EcuValues);

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
   const stw::opensyde_core::C_OscNodeDataPoolContent * GetElementData(void) const;
   void Reset(void);

private:
   bool mq_EcuValues;
   uint32_t mu32_ElementIndex;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * mpc_DataWidget;

   const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId * m_GetElementId(void) const;
   int32_t m_GetDataSetIndex(void) const;
   bool m_CheckError(const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
