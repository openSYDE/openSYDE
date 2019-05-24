//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for topology item list (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPLISTMODEL_H
#define C_NAGTOPLISTMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractListModel>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopListModel :
   public QAbstractListModel
{
public:
   C_NagTopListModel(QObject * const opc_Parent = NULL);

   void SetTypeNode(const bool oq_IsNode);
   void SetSelectedIndex(const stw_types::sint32 os32_Selected);
   void SetContent(const std::vector<QString> & orc_Content);
   void SetError(const std::vector<bool> & orc_Error);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   //lint -restore

private:
   stw_types::sint32 ms32_Selected;
   std::vector<QString> mc_Content;
   std::vector<bool> mc_Error;
   bool mq_TypeNode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
