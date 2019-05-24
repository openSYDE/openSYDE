//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAARMODEL_H
#define C_SYVDAITPAARMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_PuiSvDbDataElementHandler.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaArModel :
   public QAbstractTableModel
{
public:
   explicit C_SyvDaItPaArModel(QObject * const opc_Parent = NULL);

   void SetElement(const stw_types::uint32 ou32_ElementIndex,
                   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget, const bool oq_ECUValues);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Header:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Editable:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   void HandleDataChange(const stw_types::uint32 & oru32_Column);
   stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type GetType(void) const;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCElement(void) const;
   const stw_opensyde_core::C_OSCNodeDataPoolContent * GetElementData(void) const;
   void Reset(void);

private:
   bool mq_ECUValues;
   stw_types::uint32 mu32_ElementIndex;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * mpc_DataWidget;

   const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId * m_GetElementId(void) const;
   stw_types::sint32 m_GetDataSetIndex(void) const;
   bool m_CheckError(const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
