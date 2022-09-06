//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filter for data elements tree model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREESORTFILTER_H
#define C_SYVDAPEDATAELEMENTTREESORTFILTER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreDataElementSortFilter :
   public QSortFilterProxyModel
{
public:
   C_TblTreDataElementSortFilter(QObject * const opc_Parent = NULL);

   void SetFilter(const QString & orc_Text);

protected:
   virtual bool filterAcceptsRow(const stw_types::sintn osn_SourceRow,
                                 const QModelIndex & orc_SourceParent) const override;
   virtual bool m_Contains(const QModelIndex & orc_Index) const;

private:
   bool m_CheckParents(const QModelIndex & orc_SourceParent) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
